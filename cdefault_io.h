#ifndef CDEFAULT_IO_H_
#define CDEFAULT_IO_H_

#include "cdefault_std.h"

// API for handling OS files, with some generic convenience funcs. All routines are synchronous.
//
// NOTE: This API simplifies file access semantics. If opening a file to write, it places an exclusive
// (read & write) lock on that file. If opening a file in read-only mode, it places a shared (read)
// lock on that file. Any written data is flushed when the file handle is closed.

// TODO: file search / better directory navigation
// TODO: more extensive testing
// TODO: use string8s instead of cstrings?
// TODO: support in-memory files
// TODO: support stdout / stderr

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

B32 FileStat(U8* file_path, FileStats* stats); // NOTE: Retrieves stats / info on the file with the given path.
B32 FileReadAll(Arena* arena, U8* file_path, U8** buffer, S32* buffer_size); // NOTE: Places the data in file_path in *buffer.
B32 FileDump(U8* file_path, U8* buffer, S32 buffer_size);   // NOTE: Replaces data in file_path with buffer (removes any \0 suffix).
B32 FileAppend(U8* file_path, U8* buffer, S32 buffer_size); // NOTE: Appends the data with buffer (removes any \0 suffix). If you will append many times, prefer opening a FileHandle manually.
B32 FileCopy(U8* src_path, U8* dest_path); // NOTE: Replaces all data in dest_path with the data in src_path.

B32 DirSetCurrentToExeDir();      // NOTE: Sets the current working directory to wherever the executable is.
B32 DirSetCurrent(U8* file_path); // NOTE: Sets the current working directory to the provided location.
B32 DirGetCurrent(Arena* arena, U8** file_path, S32* file_path_size); // NOTE: Gets the current working directory.
B32 DirGetExe(Arena* arena, U8** file_path, S32* file_path_size);     // NOTE: Gets the path to the currently running executable.
B32 DirGetExeDir(Arena* arena, U8** file_path, S32* file_path_size);  // NOTE: Gets the directory to the currently running executable.

B32 PathPop(U8** path); // NOTE: Pops the right most part of the path off. E.g. /a/b/c -> /a/b

B32 FileHandleOpen(FileHandle* file, U8* file_path, FileMode mode);  // NOTE: Opens a file. Mode must include read and / or write. Implicitly places a shared or exclusive lock depending on the mode.
B32 FileHandleClose(FileHandle* file);                               // NOTE: Closes a file, releases any locks held on that file.
B32 FileHandleStat(FileHandle* file, FileStats* stats);              // NOTE: Like FileStat, but on a FileHandle instead of a path.
B32 FileHandleSeek(FileHandle* file, S32 distance, FileSeekPos pos); // NOTE: Seeks to a given position in the file.
B32 FileHandleRead(FileHandle* file, U8* buffer, S32 buffer_size, S32* bytes_read); // NOTE: Reads / places buffer_size bytes into buffer, stopping if EOF is observed. Num bytes read is placed into bytes_read.
B32 FileHandleWrite(FileHandle* file, U8* buffer, S32 buffer_size);  // NOTE: Writes / places buffer_size bytes from buffer into the file.

#endif // CDEFAULT_IO_H_

#ifdef CDEFAULT_IO_IMPLEMENTATION
#undef CDEFAULT_IO_IMPLEMENTATION

#if defined(OS_WINDOWS)
#define CDEFAULT_IO_BACKEND_NAMESPACE WIN_

#include <windows.h>

struct FileHandle {
  Arena* arena;
  HANDLE handle;
  U8* file_path;
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

B32 WIN_FileStat(U8* file_path, FileStats* stats) {
  WIN32_FILE_ATTRIBUTE_DATA attributes;
  if (!GetFileAttributesExA((LPCSTR) file_path, GetFileExInfoStandard, &attributes)) {
    WIN_IO_LOG_ERROR_EX(GetLastError(), "[IO] Failed to stat file: %s", file_path);
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
    WIN_IO_LOG_ERROR_EX(GetLastError(), "[IO] Failed to stat file: %s", file->file_path);
    return false;
  }
  stats->size = (((U32) attributes.nFileSizeHigh) << 16) | attributes.nFileSizeLow;
  stats->last_write_time = WIN_FileTimeToEpochSeconds(&attributes.ftLastWriteTime);
  stats->last_access_time = WIN_FileTimeToEpochSeconds(&attributes.ftLastAccessTime);
  return true;
}

B32 WIN_DirGetExe(Arena* arena, U8** file_path, S32* file_path_size) {
  U16 size = 0;
  do {
    ARENA_POP_ARRAY(arena, U8, size);
    if (size > KB(1)) {
      LOG_ERROR("[IO] File name of the current EXE is greater than 1KB...?");
      return false;
    }
    size += 256;
    *file_path = ARENA_PUSH_ARRAY(arena, U8, size);
    *file_path_size = GetModuleFileNameA(NULL, (LPSTR) *file_path, size);
  } while (*file_path_size == size);
  (*file_path)[*file_path_size] = '\0';
  ARENA_POP_ARRAY(arena, U8, size - (*file_path_size + 1)); // NOTE: +1 incl. null term.
  return true;
}

B32 WIN_DirGetCurrent(Arena* arena, U8** file_path, S32* file_path_size) {
  U16 size = 0;
  do {
    ARENA_POP_ARRAY(arena, U8, size);
    if (size > KB(1)) {
      LOG_ERROR("[IO] File name of the current directory is greater than 1KB...?");
      return false;
    }
    size += 256;
    *file_path = ARENA_PUSH_ARRAY(arena, U8, size);
    *file_path_size = GetCurrentDirectoryA(size, (LPSTR) *file_path);
  } while (*file_path_size == size);
  (*file_path)[*file_path_size] = '\0';
  ARENA_POP_ARRAY(arena, U8, size - (*file_path_size + 1)); // NOTE: +1 incl. null term.
  return true;
}

B32 WIN_DirSetCurrent(U8* file_path) {
  CStrReplaceAllChar(file_path, '/', '\\');
  BOOL result = SetCurrentDirectory((LPCSTR) file_path);
  if (!result) {
    WIN_IO_LOG_ERROR_EX(GetLastError(), "[IO] Failed to set current directory: %s", file_path);
    return false;
  }
  return true;
}

B32 WIN_FileHandleOpen(FileHandle* file, U8* file_path, FileMode mode) {
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

  file->handle = CreateFileA((const char*) file_path, desired_access, FILE_SHARE_READ, NULL, disposition, FILE_ATTRIBUTE_NORMAL, NULL);
  if (file->handle == INVALID_HANDLE_VALUE) {
    WIN_IO_LOG_ERROR_EX(GetLastError(), "[IO] Failed to open file: %s", file_path);
    return false;
  }
  file->arena = ArenaAllocate();
  file->file_path = CStrCopy(file->arena, file_path);
  CStrReplaceAllChar(file->file_path, '/', '\\');
  file->is_writing = write;
  return true;
}

B32 WIN_FileHandleClose(FileHandle* file) {
  if (file->is_writing && !FlushFileBuffers(file->handle)) {
    WIN_IO_LOG_ERROR_EX(GetLastError(), "[IO] Failed to flush file: %s", file->file_path);
    return false;
  }
  if (!CloseHandle(file->handle)) {
    WIN_IO_LOG_ERROR_EX(GetLastError(), "[IO] Failed to close file: %s", file->file_path);
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
    WIN_IO_LOG_ERROR_EX(GetLastError(), "[IO] Failed to seek: %s", file->file_path);
    return false;
  }
  return true;
}

B32 WIN_FileHandleRead(FileHandle* file, U8* buffer, S32 buffer_size, S32* bytes_read) {
  DWORD read;
  BOOL result = ReadFile(file->handle, (LPVOID) buffer, buffer_size, &read, NULL);
  if (!result) {
    WIN_IO_LOG_ERROR_EX(GetLastError(), "[IO] Failed to read file: %s", file->file_path);
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
      WIN_IO_LOG_ERROR_EX(GetLastError(), "[IO] Failed to write to file: %s", file->file_path);
      return false;
    }
    if (w == 0) {
      LOG_ERROR("[IO] Attempt to write returned 0 bytes: %s", file->file_path);
      return false;
    }
    total += w;
  }
  return true;
}

#elif defined(OS_LINUX)
#define CDEFAULT_IO_BACKEND_NAMESPACE LINUX_

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

struct FileHandle {
  Arena* arena;
  S32 fd;
  U8* file_path;
  B8 is_writing;
};

#define LINUX_IO_LOG_ERROR_EX(result, fmt, ...) \
  LOG_ERROR(fmt " - %s: %s", ##__VA_ARGS__, strerror(result))
#define LINUX_IO_LOG_ERROR(result, err) \
  LINUX_IO_LOG_ERROR_EX(result, "%s", err)

B32 LINUX_FileStat(U8* file_path, FileStats* stats) {
  struct stat st;
  if (stat(file_path, &st) == -1) {
    LINUX_IO_LOG_ERROR_EX(errno, "[IO] Failed to stat file: %s", file_path);
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
    LINUX_IO_LOG_ERROR_EX(errno, "[IO] Failed to stat file: %s", file->file_path);
    return false;
  }
  stats->size = st.st_size;
  stats->last_write_time = st.st_mtime;
  stats->last_access_time = st.st_atime;
  return true;
}

B32 LINUX_DirGetExe(Arena* arena, U8** file_path, S32* file_path_size) {
  U16 size = 0;
  do {
    ARENA_POP_ARRAY(arena, U8, size);
    if (size > KB(1)) {
      LOG_ERROR("[IO] File name of the current EXE is greater than 1KB...?");
      return false;
    }
    size += 256;
    *file_path = ARENA_PUSH_ARRAY(arena, U8, size);
    // TODO: this only works on linux, not other unix OSs.
    *file_path_size = readlink("/proc/self/exe", *file_path, size);
  } while (*file_path_size == size);
  (*file_path)[*file_path_size] == '\0';
  ARENA_POP_ARRAY(arena, U8, size - (*file_path_size + 1));
  return true;
}

B32 LINUX_DirGetCurrent(Arena* arena, U8** file_path, S32* file_path_size) {
  U16 size = 0;
  do {
    ARENA_POP_ARRAY(arena, U8, size);
    if (size > KB(1)) {
      LOG_ERROR("[IO] File name of the current directory is greater than 1KB...?");
      return false;
    }
    size += 256;
    *file_path = ARENA_PUSH_ARRAY(arena, U8, size);
    getcwd(*file_path, size);
  } while (*file_path == NULL);
  *file_path_size = CStrSize(*file_path);
  ARENA_POP_ARRAY(arena, U8, size - (*file_path_size + 1));
  return true;
}

B32 LINUX_DirSetCurrent(U8* file_path) {
  S32 result = chdir(file_path);
  if (result == -1) {
    LINUX_IO_LOG_ERROR_EX(errno, "[IO] Failed to set current directory: %s", file_path);
    return false;
  }
  return true;
}

B32 LINUX_FileHandleOpen(FileHandle* file, U8* file_path, FileMode mode) {
  S32 flags = 0;

  B32 read  = mode & FileMode_Read;
  B32 write = mode & FileMode_Write;
  DEBUG_ASSERT(read || write);
  if      (read && write) { flags |= O_RDWR;   }
  else if (read)          { flags |= O_RDONLY; }
  else if (write)         { flags |= O_WRONLY; }

  if (mode & FileMode_Truncate) { flags |= O_TRUNC; }
  if (mode & FileMode_Create)   { flags |= O_CREAT; }
  file->fd = open(file_path, flags, 0770);
  if (file->fd == -1) {
    LINUX_IO_LOG_ERROR_EX(errno, "[IO] Failed to open file: %s", file_path);
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
    LINUX_IO_LOG_ERROR_EX(errno, "[IO] Failed to set read lock on file: %s", file_path);
    DEBUG_ASSERT(close(file->fd) == 0);
    return false;
  }

  file->arena = ArenaAllocate();
  file->file_path = CStrCopy(file->arena, file_path);
  CStrReplaceAllChar(file->file_path, '\\', '/');
  file->is_writing = write;
  return true;
}

B32 LINUX_FileHandleClose(FileHandle* file) {
  if (file->is_writing && (fsync(file->fd) == -1)) {
    LINUX_IO_LOG_ERROR_EX(errno, "[IO] Failed to flush file: %s", file->file_path);
    return false;
  }
  if (close(file->fd) == -1) {
    // NOTE: linux docs warn against retrying close(), so log but don't report this error.
    LINUX_IO_LOG_ERROR_EX(errno, "[IO] Failed to close file, ignoring: %s", file->file_path);
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
    LINUX_IO_LOG_ERROR_EX(errno, "[IO] Failed to seek for file: %s", file->file_path);
    return false;
  }
  return true;
}

B32 LINUX_FileHandleRead(FileHandle* file, U8* buffer, S32 buffer_size, S32* bytes_read) {
  S32 r = read(file->fd, buffer, buffer_size);
  if (r == -1) {
    LINUX_IO_LOG_ERROR_EX(errno, "[IO] Failed to read file: %s", file->file_path);
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
      LINUX_IO_LOG_ERROR_EX(errno, "[IO] Failed to write file: %s", file->file_path);
      return false;
    }
    if (w == 0) {
      LOG_ERROR("[IO] Attempt to write returned 0 bytes: %s", file->file_path);
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

B32 FileStat(U8* file_path, FileStats* stats) {
  return CDEFAULT_IO_BACKEND_FN(FileStat(file_path, stats));
}

B32 FileReadAll(Arena* arena, U8* file_path, U8** buffer, S32* buffer_size) {
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

B32 FileDump(U8* file_path, U8* buffer, S32 buffer_size) {
  B32 success = false;
  FileHandle handle;
  if (!FileHandleOpen(&handle, file_path, FileMode_Write | FileMode_Create | FileMode_Truncate)) { return false; }
  if (!FileHandleWrite(&handle, buffer, buffer_size)) { goto file_dump_exit; }
  success = true;
file_dump_exit:
  DEBUG_ASSERT(FileHandleClose(&handle));
  return success;
}

B32 FileAppend(U8* file_path, U8* buffer, S32 buffer_size) {
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

B32 FileCopy(U8* src_path, U8* dest_path) {
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

B32 DirGetExe(Arena* arena, U8** file_path, S32* file_path_size) {
  return CDEFAULT_IO_BACKEND_FN(DirGetExe(arena, file_path, file_path_size));
}

B32 DirGetExeDir(Arena* arena, U8** file_path, S32* file_path_size) {
  B32 result = DirGetExe(arena, file_path, file_path_size);
  if (result) { PathPop(file_path); }
  return result;
}

B32 DirGetCurrent(Arena* arena, U8** file_path, S32* file_path_size) {
  return CDEFAULT_IO_BACKEND_FN(DirGetCurrent(arena, file_path, file_path_size));
}

B32 DirSetCurrent(U8* file_path) {
  return CDEFAULT_IO_BACKEND_FN(DirSetCurrent(file_path));
}

B32 DirSetCurrentToExeDir() {
  B32 success = false;
  Arena* arena = ArenaAllocate();
  U8* dir;
  S32 dir_size;
  if (!DirGetExeDir(arena, &dir, &dir_size)) { goto dir_set_current_to_exe_dir_exit; }
  if (!DirSetCurrent(dir)) { goto dir_set_current_to_exe_dir_exit; }
  success = true;
dir_set_current_to_exe_dir_exit:
  ArenaRelease(arena);
  return success;
}

B32 PathPop(U8** path) {
  CStrReplaceAllChar(*path, '\\', '/');
  U8* needle = (U8*) "/";
  S32 result = CStrFindReverse(*path, CStrSize(*path) - 1, needle);
  if (result >= 0) { (*path)[result] = '\0'; }
  return result >= 0;
}

B32 FileHandleStat(FileHandle* file, FileStats* stats) {
  return CDEFAULT_IO_BACKEND_FN(FileHandleStat(file, stats));
}

B32 FileHandleOpen(FileHandle* file, U8* file_path, FileMode mode) {
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

#endif // CDEFAULT_IO_IMPLEMENTATION
