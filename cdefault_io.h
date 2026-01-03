#ifndef CDEFAULT_IO_H_
#define CDEFAULT_IO_H_

#include "cdefault_std.h"

// API for handling OS files, with some generic convenience funcs. All routines are synchronous.
// Also contains routines for logging (either to stdout or a file, based on initialization).
//
// NOTE: This API simplifies file access semantics. If opening a file to write, it places an exclusive
// (read & write) lock on that file. If opening a file in read-only mode, it places a shared (read)
// lock on that file. Any written data is flushed when the file handle is closed.

// TODO: file search / better directory navigation
// TODO: more extensive testing
// TODO: support in-memory files
// TODO: support stdout / stderr
// TODO: be more consistent with file separator conversions

typedef struct FileHandle FileHandle;

typedef enum FileMode FileMode;
enum FileMode {
  FileMode_Read     = BIT(0), // NOTE: Allow reading of file data.
  FileMode_Write    = BIT(1), // NOTE: Allow writing of file data.
  FileMode_Create   = BIT(2), // NOTE: Allow creation of the file if it does not exist.
  FileMode_Truncate = BIT(3), // NOTE: Truncate the file on successful open.
};

typedef enum FileSeekPos FileSeekPos;
enum FileSeekPos {
  FileSeekPos_Begin,
  FileSeekPos_Current,
  FileSeekPos_End,
};

typedef struct FileStats FileStats;
struct FileStats {
  U32 size;
  U64 last_write_time;
  U64 last_access_time;
};

B32 FileStat(String8 file_path, FileStats* stats); // NOTE: Retrieves stats / info on the file with the given path.
B32 FileReadAll(Arena* arena, String8 file_path, U8** buffer, S32* buffer_size); // NOTE: Places the data in file_path in *buffer.
B32 FileDump(String8 file_path, U8* buffer, S32 buffer_size);   // NOTE: Replaces data in file_path with buffer (removes any \0 suffix).
B32 FileAppend(String8 file_path, U8* buffer, S32 buffer_size); // NOTE: Appends the data with buffer (removes any \0 suffix). If you will append many times, prefer opening a FileHandle manually.
B32 FileCopy(String8 src_path, String8 dest_path); // NOTE: Replaces all data in dest_path with the data in src_path.

B32 DirSetCurrentToExeDir();      // NOTE: Sets the current working directory to wherever the executable is.
B32 DirSetCurrent(String8 file_path); // NOTE: Sets the current working directory to the provided location.
B32 DirGetCurrent(Arena* arena, String8* file_path); // NOTE: Gets the current working directory.
B32 DirGetExe(Arena* arena, String8* file_path);     // NOTE: Gets the path to the currently running executable.
B32 DirGetExeDir(Arena* arena, String8* file_path);  // NOTE: Gets the directory to the currently running executable.
B32 DirListFiles(Arena* arena, String8 dir_path, String8List* file_paths); // NOTE: Given a path to a directory, returns the files in that directory.

B32     PathPop(String8* path); // NOTE: Pops the right most part of the path off. E.g. /a/b/c -> /a/b
String8 PathJoin(Arena* arena, String8List path_parts);

FileHandle* FileHandleAllocate(Arena* arena);
B32 FileHandleOpenStdOut(FileHandle* file);                              // NOTE: Opens a file to stdout / the console.
B32 FileHandleOpen(FileHandle* file, String8 file_path, FileMode mode);  // NOTE: Opens a file. Mode must include read and / or write. Implicitly places a shared or exclusive lock depending on the mode.
B32 FileHandleClose(FileHandle* file);                               // NOTE: Closes a file, releases any locks held on that file.
B32 FileHandleStat(FileHandle* file, FileStats* stats);              // NOTE: Like FileStat, but on a FileHandle instead of a path.
B32 FileHandleSeek(FileHandle* file, S32 distance, FileSeekPos pos); // NOTE: Seeks to a given position in the file.
B32 FileHandleRead(FileHandle* file, U8* buffer, S32 buffer_size, S32* bytes_read); // NOTE: Reads / places buffer_size bytes into buffer, stopping if EOF is observed. Num bytes read is placed into bytes_read.
B32 FileHandleWrite(FileHandle* file, U8* buffer, S32 buffer_size);  // NOTE: Writes / places buffer_size bytes from buffer into the file.

B32 LogInitStdOut();
B32 LogInitFile(String8 file_path);
#define LOG_NO_PREFIX(fmt, ...)  Log(LogLevel_NoPrefix, Str8CStr(FILENAME), __LINE__, Str8Lit(fmt), ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)       Log(LogLevel_Info,     Str8CStr(FILENAME), __LINE__, Str8Lit(fmt), ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)       Log(LogLevel_Warning,  Str8CStr(FILENAME), __LINE__, Str8Lit(fmt), ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...)      Log(LogLevel_Error,    Str8CStr(FILENAME), __LINE__, Str8Lit(fmt), ##__VA_ARGS__)
#ifndef NDEBUG
#  define LOG_DEBUG(fmt, ...)    Log(LogLevel_Debug,    Str8CStr(FILENAME), __LINE__, Str8Lit(fmt), ##__VA_ARGS__)
#else
#  define LOG_DEBUG(fmt, ...)
#endif

typedef enum LogLevel LogLevel;
enum LogLevel {
  LogLevel_NoPrefix,
  LogLevel_Debug,
  LogLevel_Info,
  LogLevel_Warning,
  LogLevel_Error,
};
void Log(LogLevel level, String8 filename, U32 loc, String8 fmt, ...);

#endif // CDEFAULT_IO_H_

#ifdef CDEFAULT_IO_IMPLEMENTATION
#undef CDEFAULT_IO_IMPLEMENTATION

#if defined(OS_WINDOWS)
#define CDEFAULT_IO_BACKEND_NAMESPACE WIN_

#include <windows.h>

struct FileHandle {
  Arena* arena;
  HANDLE handle;
  String8 file_path;
  B8 is_writing;
};

#define WIN_IO_LOG_ERROR_EX(result, fmt, ...)                                \
  do {                                                                       \
    char _err_buffer[1024];                                                  \
    _err_buffer[0] = '\0';                                                   \
    FormatMessageA(                                                          \
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK,          \
        NULL, result, 0, _err_buffer, STATIC_ARRAY_SIZE(_err_buffer), NULL); \
    LOG_ERROR(fmt " - %s", ##__VA_ARGS__, _err_buffer);                      \
  } while (0)
#define WIN_IO_LOG_ERROR(result, err)                                        \
  WIN_IO_LOG_ERROR_EX(result, "%s", err)

static U64 WIN_FileTimeToEpochSeconds(FILETIME* filetime) {
  U64 time = (((U64) filetime->dwHighDateTime) << 32) | filetime->dwLowDateTime;
  time -= 116444736000000000ULL;
  return time / 10000000ULL;
}

B32 WIN_FileStat(String8 file_path, FileStats* stats) {
  Arena* temp_arena = ArenaAllocate();
  U8* file_path_cstr = CStrFromStr8(temp_arena, file_path);
  WIN32_FILE_ATTRIBUTE_DATA attributes;
  BOOL status = GetFileAttributesExA((LPCSTR) file_path_cstr, GetFileExInfoStandard, &attributes);
  ArenaRelease(temp_arena);
  if (!status) {
    WIN_IO_LOG_ERROR_EX(GetLastError(), "[IO] Failed to stat file: %.*s", file_path.size, file_path.str);
    return false;
  }
  stats->size = (((U32) attributes.nFileSizeHigh) << 16) | attributes.nFileSizeLow;
  stats->last_write_time = WIN_FileTimeToEpochSeconds(&attributes.ftLastWriteTime);
  stats->last_access_time = WIN_FileTimeToEpochSeconds(&attributes.ftLastAccessTime);
  return true;
}

B32 WIN_FileHandleStat(FileHandle* file, FileStats* stats) {
  BY_HANDLE_FILE_INFORMATION attributes;
  if (!GetFileInformationByHandle(file->handle, &attributes)) {
    WIN_IO_LOG_ERROR_EX(GetLastError(), "[IO] Failed to stat file: %.*s", file->file_path.size, file->file_path.str);
    return false;
  }
  stats->size = (((U32) attributes.nFileSizeHigh) << 16) | attributes.nFileSizeLow;
  stats->last_write_time = WIN_FileTimeToEpochSeconds(&attributes.ftLastWriteTime);
  stats->last_access_time = WIN_FileTimeToEpochSeconds(&attributes.ftLastAccessTime);
  return true;
}

B32 WIN_DirGetExe(Arena* arena, String8* file_path) {
  U16 size = 0;
  do {
    ARENA_POP_ARRAY(arena, U8, size);
    if (size > KB(1)) {
      LOG_ERROR("[IO] File name of the current EXE is greater than 1KB...?");
      return false;
    }
    size += 256;
    file_path->str  = ARENA_PUSH_ARRAY(arena, U8, size);
    file_path->size = GetModuleFileNameA(NULL, (LPSTR) file_path->str, size);
  } while (file_path->size == size);
  ARENA_POP_ARRAY(arena, U8, size - file_path->size);
  return true;
}

B32 WIN_DirGetCurrent(Arena* arena, String8* file_path) {
  U16 size = 0;
  do {
    ARENA_POP_ARRAY(arena, U8, size);
    if (size > KB(1)) {
      LOG_ERROR("[IO] File name of the current directory is greater than 1KB...?");
      return false;
    }
    size += 256;
    file_path->str  = ARENA_PUSH_ARRAY(arena, U8, size);
    file_path->size = GetCurrentDirectoryA(size, (LPSTR) file_path->str);
  } while (file_path->size == size);
  ARENA_POP_ARRAY(arena, U8, size - file_path->size);
  return true;
}

B32 WIN_DirSetCurrent(String8 file_path) {
  Arena* temp_arena = ArenaAllocate();
  U8* file_path_cstr = CStrFromStr8(temp_arena, file_path);
  CStrReplaceAllChar(file_path_cstr, '/', '\\');
  BOOL result = SetCurrentDirectory((LPCSTR) file_path_cstr);
  ArenaRelease(temp_arena);
  if (!result) {
    WIN_IO_LOG_ERROR_EX(GetLastError(), "[IO] Failed to set current directory: %.*s", file_path.size, file_path.str);
    return false;
  }
  return true;
}

B32 WIN_DirListFiles(Arena* arena, String8 dir_path, String8List* file_paths) {
  B32 success = false;
  MEMORY_ZERO_STRUCT(file_paths);
  Arena* temp_arena = ArenaAllocate();

  String8List query_path;
  MEMORY_ZERO_STRUCT(&query_path);
  Str8ListAppend(temp_arena, &query_path, dir_path);
  Str8ListAppend(temp_arena, &query_path, Str8Lit("*"));
  U8* query_path_cstr = CStrFromStr8(temp_arena, PathJoin(temp_arena, query_path));
  CStrReplaceAllChar(query_path_cstr, '/', '\\');

  WIN32_FIND_DATAA find_data;
  HANDLE handle = FindFirstFileA((char*) query_path_cstr, &find_data);
  if (handle == INVALID_HANDLE_VALUE) {
    WIN_IO_LOG_ERROR_EX(GetLastError(), "[IO] Failed to read contents of directory: %.*s", dir_path.size, dir_path.str);
    goto win_dir_list_files_exit;
  }

  do {
    if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) { continue; }
    ArenaClear(temp_arena);
    String8List path_parts;
    MEMORY_ZERO_STRUCT(&path_parts);
    Str8ListAppend(temp_arena, &path_parts, dir_path);
    Str8ListAppend(temp_arena, &path_parts, Str8CStr(find_data.cFileName));
    String8 file_path = PathJoin(temp_arena, path_parts);
    Str8ListAppend(arena, file_paths, Str8Copy(arena, file_path));
  } while (FindNextFileA(handle, &find_data));
  FindClose(handle);
  success = true;

win_dir_list_files_exit:
  ArenaRelease(temp_arena);
  return success;
}

B32 WIN_FileHandleOpenStdOut(FileHandle* file) {
  MEMORY_ZERO_STRUCT(file);
  file->handle = CreateFileA("CON", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (file->handle == INVALID_HANDLE_VALUE) {
    WIN_IO_LOG_ERROR(GetLastError(), "[IO] Failed to open handle to stdout.");
    return false;
  }
  file->file_path  = Str8Lit("stdout");
  file->is_writing = true;
  return true;
}

B32 WIN_FileHandleOpen(FileHandle* file, String8 file_path, FileMode mode) {
  DWORD desired_access = 0;
  B32 read  = mode & FileMode_Read;
  B32 write = mode & FileMode_Write;
  DEBUG_ASSERT(read || write);
  if (read)  { desired_access |= GENERIC_READ;  }
  if (write) { desired_access |= GENERIC_WRITE; }

  DWORD disposition = 0;
  B32 truncate = mode & FileMode_Truncate;
  B32 create   = mode & FileMode_Create;
  if (create && truncate) { disposition = CREATE_ALWAYS;     }
  else if (create)        { disposition = OPEN_ALWAYS;       }
  else if (truncate)      { disposition = TRUNCATE_EXISTING; }
  else                    { disposition = OPEN_EXISTING;     }

  // NOTE: exclusive access, unless in read-only mode, in which case allow shared reads.
  // TODO: consider allowing shared writes? e.g. for hotloading?
  DWORD share_mode = 0;
  if (!write && read) { share_mode |= FILE_SHARE_READ; }

  Arena* temp_arena = ArenaAllocate();
  U8* file_path_cstr = CStrFromStr8(temp_arena, file_path);
  file->handle = CreateFileA((const char*) file_path_cstr, desired_access, FILE_SHARE_READ, NULL, disposition, FILE_ATTRIBUTE_NORMAL, NULL);
  ArenaRelease(temp_arena);
  if (file->handle == INVALID_HANDLE_VALUE) {
    WIN_IO_LOG_ERROR_EX(GetLastError(), "[IO] Failed to open file: %.*s", file_path.size, file_path.str);
    return false;
  }
  file->arena = ArenaAllocate();
  file->file_path = Str8Copy(file->arena, file_path);
  Str8ReplaceAllChar(&file->file_path, '/', '\\');
  file->is_writing = write;
  return true;
}

B32 WIN_FileHandleClose(FileHandle* file) {
  if (file->is_writing && !FlushFileBuffers(file->handle)) {
    WIN_IO_LOG_ERROR_EX(GetLastError(), "[IO] Failed to flush file: %.*s", file->file_path.size, file->file_path.str);
    return false;
  }
  if (!CloseHandle(file->handle)) {
    WIN_IO_LOG_ERROR_EX(GetLastError(), "[IO] Failed to close file: %.*s", file->file_path.size, file->file_path.str);
    return false;
  }
  ArenaRelease(file->arena);
  return true;
}

B32 WIN_FileHandleSeek(FileHandle* file, S32 distance, FileSeekPos pos) {
  DWORD move_method = 0;
  switch (pos) {
    case FileSeekPos_Begin:   { move_method = FILE_BEGIN;   } break;
    case FileSeekPos_Current: { move_method = FILE_CURRENT; } break;
    case FileSeekPos_End:     { move_method = FILE_END;     } break;
    default: UNIMPLEMENTED(); break;
  }
  if (SetFilePointer(file->handle, distance, NULL, move_method) == INVALID_SET_FILE_POINTER) {
    WIN_IO_LOG_ERROR_EX(GetLastError(), "[IO] Failed to seek: %.*s", file->file_path.size, file->file_path.str);
    return false;
  }
  return true;
}

B32 WIN_FileHandleRead(FileHandle* file, U8* buffer, S32 buffer_size, S32* bytes_read) {
  DWORD read;
  BOOL result = ReadFile(file->handle, (LPVOID) buffer, buffer_size, &read, NULL);
  if (!result) {
    WIN_IO_LOG_ERROR_EX(GetLastError(), "[IO] Failed to read file: %.*s", file->file_path.size, file->file_path.str);
    return false;
  }
  if (bytes_read != NULL) { *bytes_read = read; }
  return true;
}

B32 WIN_FileHandleWrite(FileHandle* file, U8* buffer, S32 buffer_size) {
  if (buffer_size < 0) {
    LOG_ERROR("[IO] Attempting to write a buffer size of: %d.", buffer_size);
    return false;
  }
  DWORD total = 0;
  while (total < (U32) buffer_size) {
    DWORD w;
    BOOL result = WriteFile(file->handle, buffer + total, buffer_size - total, &w, NULL);
    if (!result) {
      WIN_IO_LOG_ERROR_EX(GetLastError(), "[IO] Failed to write to file: %.*s", file->file_path.size, file->file_path.str);
      return false;
    }
    if (w == 0) {
      LOG_ERROR("[IO] Attempt to write returned 0 bytes: %.*s", file->file_path.size, file->file_path.str);
      return false;
    }
    total += w;
  }
  return true;
}

#elif defined(OS_LINUX)
#define CDEFAULT_IO_BACKEND_NAMESPACE LINUX_

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

struct FileHandle {
  Arena* arena;
  S32 fd;
  String8 file_path;
  B8 is_writing;
};

#define LINUX_IO_LOG_ERROR_EX(result, fmt, ...) \
  LOG_ERROR(fmt " - %s: %s", ##__VA_ARGS__, strerror(result))
#define LINUX_IO_LOG_ERROR(result, err) \
  LINUX_IO_LOG_ERROR_EX(result, "%s", err)

B32 LINUX_FileStat(String8 file_path, FileStats* stats) {
  Arena* temp_arena = ArenaAllocate();
  U8* file_path_cstr = CStrFromStr8(temp_arena, file_path);
  struct stat st;
  S32 result = stat(file_path_cstr, &st);
  ArenaRelease(temp_arena);
  if (result == -1) {
    LINUX_IO_LOG_ERROR_EX(errno, "[IO] Failed to stat file: %.*s", file_path.size, file_path.str);
    return false;
  }
  stats->size = st.st_size;
  stats->last_write_time = st.st_mtime;
  stats->last_access_time = st.st_atime;
  return true;
}

B32 LINUX_FileHandleStat(FileHandle* file, FileStats* stats) {
  struct stat st;
  if (fstat(file->fd, &st) == -1) {
    LINUX_IO_LOG_ERROR_EX(errno, "[IO] Failed to stat file: %.*s", file->file_path.size, file->file_path.str);
    return false;
  }
  stats->size = st.st_size;
  stats->last_write_time = st.st_mtime;
  stats->last_access_time = st.st_atime;
  return true;
}

B32 LINUX_DirGetExe(Arena* arena, String8* file_path) {
  U16 size = 0;
  do {
    ARENA_POP_ARRAY(arena, U8, size);
    if (size > KB(1)) {
      LOG_ERROR("[IO] File name of the current EXE is greater than 1KB...?");
      return false;
    }
    size += 256;
    file_path->str = ARENA_PUSH_ARRAY(arena, U8, size);
    // TODO: this only works on linux, not other unix OSs.
    file_path->size = readlink("/proc/self/exe", file_path->str, size);
  } while (file_path->size == size);
  ARENA_POP_ARRAY(arena, U8, size - file_path->size);
  return true;
}

B32 LINUX_DirGetCurrent(Arena* arena, String8* file_path) {
  U16 size = 0;
  do {
    ARENA_POP_ARRAY(arena, U8, size);
    if (size > KB(1)) {
      LOG_ERROR("[IO] File name of the current directory is greater than 1KB...?");
      return false;
    }
    size += 256;
    file_path->str = ARENA_PUSH_ARRAY(arena, U8, size);
    getcwd(file_path->str, size);
  } while (file_path->str == NULL);
  file_path->size = CStrSize(file_path->str) - 1; // NOTE: remove null terminator
  ARENA_POP_ARRAY(arena, U8, size - file_path->size);
  return true;
}

B32 LINUX_DirSetCurrent(String8 file_path) {
  Arena* temp_arena = ArenaAllocate();
  U8* file_path_cstr = CStrFromStr8(temp_arena, file_path);
  S32 result = chdir(file_path_cstr);
  ArenaRelease(temp_arena);
  if (result == -1) {
    LINUX_IO_LOG_ERROR_EX(errno, "[IO] Failed to set current directory: %.*s", file_path.size, file_path.str);
    return false;
  }
  return true;
}

B32 LINUX_DirListFiles(Arena* arena, String8 dir_path, String8List* file_paths) {
  B32 success = false;
  Arena* temp_arena = ArenaAllocate();

  U8* dir_path_cstr = CStrFromStr8(temp_arena, dir_path);
  CStrReplaceAllChar(dir_path_cstr, '\\', '/');
  DIR* d = opendir(dir_path_cstr);
  if (d == NULL) {
    LINUX_IO_LOG_ERROR_EX(errno, "[IO] Failed to read contents of directory: %.*s", dir_path.size, dir_path.str);
    goto linux_dir_list_files_exit;
  }

  struct stat dir_stat;
  struct dirent* dir_entry;
  while (dir_entry = readdir(d)) {
    ArenaClear(temp_arena);
    String8List path_parts;
    MEMORY_ZERO_STRUCT(&path_parts);
    Str8ListAppend(temp_arena, &path_parts, dir_path);
    Str8ListAppend(temp_arena, &path_parts, Str8CStr(dir_entry->d_name));
    String8 file_path = PathJoin(temp_arena, path_parts);
    Str8ReplaceAllChar(&file_path, '\\', '/');
    if (stat(CStrFromStr8(temp_arena, file_path), &dir_stat) != 0) {
      LINUX_IO_LOG_ERROR_EX(errno, "[IO] Failed to stat file %.*s", file_path.size, file_path.str);
      continue;
    }
    if (dir_stat.st_mode & S_IFDIR) { continue; }
    Str8ListAppend(arena, file_paths, Str8Copy(arena, file_path));
  }
  closedir(d);
  success = true;

linux_dir_list_files_exit:
  ArenaRelease(temp_arena);
  return success;
}

B32 LINUX_FileHandleOpenStdOut(FileHandle* file) {
  MEMORY_ZERO_STRUCT(file);
  file->fd = dup(STDOUT_FILENO);
  file->file_path = Str8Lit("stdout");
  file->is_writing = true;
  return true;
}

B32 LINUX_FileHandleOpen(FileHandle* file, String8 file_path, FileMode mode) {
  S32 flags = 0;

  B32 read  = mode & FileMode_Read;
  B32 write = mode & FileMode_Write;
  DEBUG_ASSERT(read || write);
  if      (read && write) { flags |= O_RDWR;   }
  else if (read)          { flags |= O_RDONLY; }
  else if (write)         { flags |= O_WRONLY; }

  if (mode & FileMode_Truncate) { flags |= O_TRUNC; }
  if (mode & FileMode_Create)   { flags |= O_CREAT; }
  Arena* temp_arena = ArenaAllocate();
  U8* file_path_cstr = CStrFromStr8(temp_arena, file_path);
  file->fd = open(file_path_cstr, flags, 0770);
  ArenaRelease(temp_arena);
  if (file->fd == -1) {
    LINUX_IO_LOG_ERROR_EX(errno, "[IO] Failed to open file: %.*s", file_path.size, file_path.str);
    return false;
  }

  // NOTE: exclusive access, unless in read-only mode, in which case allow shared reads.
  // TODO: consider allowing shared writes? e.g. for hotloading?
  S16 lock_type = 0;
  if (write)      { lock_type |= F_WRLCK; }
  else if (read)  { lock_type |= F_RDLCK; }

  struct flock lock;
  lock.l_type = lock_type;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;
  if (fcntl(file->fd, F_SETLK, &lock) == -1) {
    LINUX_IO_LOG_ERROR_EX(errno, "[IO] Failed to set read lock on file: %.*s", file_path.size, file_path.str);
    DEBUG_ASSERT(close(file->fd) == 0);
    return false;
  }

  file->arena = ArenaAllocate();
  file->file_path = Str8Copy(file->arena, file_path);
  Str8ReplaceAllChar(&file->file_path, '\\', '/');
  file->is_writing = write;
  return true;
}

B32 LINUX_FileHandleClose(FileHandle* file) {
  if (file->is_writing && (fsync(file->fd) == -1)) {
    LINUX_IO_LOG_ERROR_EX(errno, "[IO] Failed to flush file: %.*s", file->file_path.size, file->file_path.str);
    return false;
  }
  if (close(file->fd) == -1) {
    // NOTE: linux docs warn against retrying close(), so log but don't report this error.
    LINUX_IO_LOG_ERROR_EX(errno, "[IO] Failed to close file, ignoring: %.*s", file->file_path.size, file->file_path.str);
  }
  ArenaRelease(file->arena);
  return true;
}

B32 LINUX_FileHandleSeek(FileHandle* file, S32 distance, FileSeekPos pos) {
  S32 whence;
  switch (pos) {
    case FileSeekPos_Begin:   { whence = SEEK_SET; } break;
    case FileSeekPos_Current: { whence = SEEK_CUR; } break;
    case FileSeekPos_End:     { whence = SEEK_END; } break;
    default: UNIMPLEMENTED(); break;
  }
  if (lseek(file->fd, distance, whence) == -1) {
    LINUX_IO_LOG_ERROR_EX(errno, "[IO] Failed to seek for file: %.*s", file->file_path.size, file->file_path.str);
    return false;
  }
  return true;
}

B32 LINUX_FileHandleRead(FileHandle* file, U8* buffer, S32 buffer_size, S32* bytes_read) {
  S32 r = read(file->fd, buffer, buffer_size);
  if (r == -1) {
    LINUX_IO_LOG_ERROR_EX(errno, "[IO] Failed to read file: %.*s", file->file_path.size, file->file_path.str);
    return false;
  }
  if (bytes_read != NULL) { *bytes_read = r; }
  return true;
}

B32 LINUX_FileHandleWrite(FileHandle* file, U8* buffer, S32 buffer_size) {
  if (buffer_size < 0) {
    LOG_ERROR("[IO] Attempting to write a buffer size of: %d.", buffer_size);
    return false;
  }
  U32 total = 0;
  while (total < (U32) buffer_size) {
    U32 w = write(file->fd, buffer + total, buffer_size - total);
    if (w == -1) {
      LINUX_IO_LOG_ERROR_EX(errno, "[IO] Failed to write file: %.*s", file->file_path.size, file->file_path.str);
      return false;
    }
    if (w == 0) {
      LOG_ERROR("[IO] Attempt to write returned 0 bytes: %.*s", file->file_path.size, file->file_path.str);
      return false;
    }
    total += w;
  }
  return true;
}

#else

// TODO: mac support.
#error cdefault IO not supported on this OS.

#endif

#define CDEFAULT_IO_BACKEND_FN_IMPL(ns, fn) GLUE(ns, fn)
#define CDEFAULT_IO_BACKEND_FN(x) CDEFAULT_IO_BACKEND_FN_IMPL(CDEFAULT_IO_BACKEND_NAMESPACE, x)

B32 FileStat(String8 file_path, FileStats* stats) {
  return CDEFAULT_IO_BACKEND_FN(FileStat(file_path, stats));
}

B32 FileReadAll(Arena* arena, String8 file_path, U8** buffer, S32* buffer_size) {
  B32 success = false;
  FileHandle handle;
  if (!FileHandleOpen(&handle, file_path, FileMode_Read)) { return false; }
  FileStats stats;
  if (!FileHandleStat(&handle, &stats)) { goto file_read_all_exit; }
  *buffer = ARENA_PUSH_ARRAY(arena, U8, stats.size);
  S32 bytes_read;
  if (!FileHandleRead(&handle, *buffer, stats.size, &bytes_read)) {
    ARENA_POP_ARRAY(arena, U8, stats.size);
    goto file_read_all_exit;
  }
  if (buffer_size != NULL) { *buffer_size = bytes_read; }
  success = true;
file_read_all_exit:
  DEBUG_ASSERT(FileHandleClose(&handle));
  return success;
}

B32 FileDump(String8 file_path, U8* buffer, S32 buffer_size) {
  B32 success = false;
  FileHandle handle;
  if (!FileHandleOpen(&handle, file_path, FileMode_Write | FileMode_Create | FileMode_Truncate)) { return false; }
  if (!FileHandleWrite(&handle, buffer, buffer_size)) { goto file_dump_exit; }
  success = true;
file_dump_exit:
  DEBUG_ASSERT(FileHandleClose(&handle));
  return success;
}

B32 FileAppend(String8 file_path, U8* buffer, S32 buffer_size) {
  B32 success = false;
  FileHandle handle;
  if (!FileHandleOpen(&handle, file_path, FileMode_Write | FileMode_Create)) { return false; }
  if (!FileHandleSeek(&handle, 0, FileSeekPos_End)) { goto file_append_exit; }
  if (!FileHandleWrite(&handle, buffer, buffer_size)) { goto file_append_exit; }
  success = true;
file_append_exit:
  DEBUG_ASSERT(FileHandleClose(&handle));
  return success;
}

B32 FileCopy(String8 src_path, String8 dest_path) {
  B32 success = false;
  Arena* arena = ArenaAllocate();
  String8 buffer;
  if (!FileReadAll(arena, src_path, &buffer.str, &buffer.size)) { goto file_copy_exit; }
  if (!FileDump(dest_path, buffer.str, buffer.size)) { goto file_copy_exit; }
  success = true;
file_copy_exit:
  ArenaRelease(arena);
  return success;
}

B32 DirGetExe(Arena* arena, String8* file_path) {
  return CDEFAULT_IO_BACKEND_FN(DirGetExe(arena, file_path));
}

B32 DirGetExeDir(Arena* arena, String8* file_path) {
  B32 result = DirGetExe(arena, file_path);
  if (result) { PathPop(file_path); }
  return result;
}

B32 DirGetCurrent(Arena* arena, String8* file_path) {
  return CDEFAULT_IO_BACKEND_FN(DirGetCurrent(arena, file_path));
}

B32 DirSetCurrent(String8 file_path) {
  return CDEFAULT_IO_BACKEND_FN(DirSetCurrent(file_path));
}

B32 DirSetCurrentToExeDir() {
  B32 success = false;
  Arena* arena = ArenaAllocate();
  String8 dir;
  if (!DirGetExeDir(arena, &dir)) { goto dir_set_current_to_exe_dir_exit; }
  if (!DirSetCurrent(dir)) { goto dir_set_current_to_exe_dir_exit; }
  success = true;
dir_set_current_to_exe_dir_exit:
  ArenaRelease(arena);
  return success;
}

B32 DirListFiles(Arena* arena, String8 dir_path, String8List* file_paths) {
  return CDEFAULT_IO_BACKEND_FN(DirListFiles(arena, dir_path, file_paths));
}

B32 PathPop(String8* path) {
  Str8ReplaceAllChar(path, '\\', '/');
  S32 result = Str8FindReverse(*path, 0, Str8Lit("/"));
  if (result >= 0) {
    *path = Str8Substring(*path, 0, result);
    return true;
  }
  return false;
}

String8 PathJoin(Arena* arena, String8List path_parts) {
  for (String8ListNode* node = path_parts.head; node != NULL; node = node->next) {
    node->string = Str8Trim(node->string);
    Str8ReplaceAllChar(&node->string, '\\', '/');
    if (node->string.size == 0) { continue; }

    if (node != path_parts.head) {
      if (node->string.str[0] == '/') {
        node->string = Str8Substring(node->string, 1, node->string.size);
      }
    }
    if (node != path_parts.tail) {
      if (node->string.str[node->string.size - 1] == '/') {
        node->string = Str8Substring(node->string, 0, node->string.size - 1);
      }

      String8ListNode* delim = ARENA_PUSH_STRUCT(arena, String8ListNode);
      delim->string = Str8Lit("/");
      delim->next = node->next;
      node->next = delim;

      node = node->next;
    }
  }
  return Str8ListJoin(arena, &path_parts);
}

B32 FileHandleStat(FileHandle* file, FileStats* stats) {
  return CDEFAULT_IO_BACKEND_FN(FileHandleStat(file, stats));
}

FileHandle* FileHandleAllocate(Arena* arena) {
  return ARENA_PUSH_STRUCT(arena, FileHandle);
}

B32 FileHandleOpenStdOut(FileHandle* file) {
  return CDEFAULT_IO_BACKEND_FN(FileHandleOpenStdOut(file));
}

B32 FileHandleOpen(FileHandle* file, String8 file_path, FileMode mode) {
  return CDEFAULT_IO_BACKEND_FN(FileHandleOpen(file, file_path, mode));
}

B32 FileHandleClose(FileHandle* file) {
  return CDEFAULT_IO_BACKEND_FN(FileHandleClose(file));
}

B32 FileHandleSeek(FileHandle* file, S32 distance, FileSeekPos pos) {
  return CDEFAULT_IO_BACKEND_FN(FileHandleSeek(file, distance, pos));
}

B32 FileHandleRead(FileHandle* file, U8* buffer, S32 buffer_size, S32* bytes_read) {
  return CDEFAULT_IO_BACKEND_FN(FileHandleRead(file, buffer, buffer_size, bytes_read));
}

B32 FileHandleWrite(FileHandle* file, U8* buffer, S32 buffer_size) {
  return CDEFAULT_IO_BACKEND_FN(FileHandleWrite(file, buffer, buffer_size));
}

typedef struct LogConfig LogConfig;
struct LogConfig {
  Arena*      arena;
  FileHandle* handle;
  Mutex       mtx;
  B8          is_initialized;
};
static LogConfig _cdef_log_config;

static void LogInitCommon(LogConfig* c) {
  DEBUG_ASSERT(!c->is_initialized);
  MEMORY_ZERO_STRUCT(c);
  c->arena  = ArenaAllocate();
  c->handle = FileHandleAllocate(c->arena);
  MutexInit(&c->mtx);
}

B32 LogInitStdOut() {
  LogConfig* c = &_cdef_log_config;
  LogInitCommon(c);
  if (!FileHandleOpenStdOut(c->handle)) { return false; }
  c->is_initialized = true;
  return true;
}

B32 LogInitFile(String8 file_path) {
  LogConfig* c = &_cdef_log_config;
  LogInitCommon(c);
  if (!FileHandleOpen(c->handle, file_path, FileMode_Write | FileMode_Create | FileMode_Truncate)) { return false; }
  c->is_initialized = true;
  return true;
}

static void LogV(LogLevel level, String8 filename, U32 loc, String8 fmt, va_list args) {
  LogConfig* c = &_cdef_log_config;
  DEBUG_ASSERT(c->is_initialized);
  U64 arena_base = ArenaPos(c->arena);

  // TODO: make highlighting optional.
  String8 prefix_text;
  switch (level) {
    case LogLevel_NoPrefix: break;
    case LogLevel_Debug:   { prefix_text = Str8Lit(ANSI_COLOR_BLUE "DEBUG" ANSI_COLOR_RESET);  } break;
    case LogLevel_Info:    { prefix_text = Str8Lit("INFO");                                    } break;
    case LogLevel_Warning: { prefix_text = Str8Lit(ANSI_COLOR_YELLOW "WARN" ANSI_COLOR_RESET); } break;
    case LogLevel_Error:   { prefix_text = Str8Lit(ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET);   } break;
    default: UNREACHABLE();
  }

  String8 prefix = Str8Lit("");
  if (level != LogLevel_NoPrefix) {
    prefix = Str8Format(c->arena, "[%S | %S:%d]: ", prefix_text, filename, loc);
  }
  String8 message = Str8FormatV(c->arena, fmt, args);
  String8 suffix  = Str8Lit("\n");

  MutexLock(&_cdef_log_config.mtx);
  FileHandleWrite(c->handle, prefix.str, prefix.size);
  FileHandleWrite(c->handle, message.str, message.size);
  FileHandleWrite(c->handle, suffix.str, suffix.size);
  MutexUnlock(&_cdef_log_config.mtx);

  ArenaPopTo(c->arena, arena_base);
}

void Log(LogLevel level, String8 filename, U32 loc, String8 fmt, ...) {
  va_list args;
  va_start(args, fmt);
  LogV(level, filename, loc, fmt, args);
  va_end(args);
}

#endif // CDEFAULT_IO_IMPLEMENTATION
