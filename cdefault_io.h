#ifndef CDEFAULT_IO_H_
#define CDEFAULT_IO_H_

#include "cdefault_std.h"

// TODO:
// - file search
// - standard file separators

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
  U64 create_time;
  U64 last_write_time;
  U64 last_access_time;
};

B32 FileStat(U8* file_path, FileStats* stats);
B32 FileReadAll(Arena* arena, U8* file_path, U8** buffer, U32* buffer_size); // NOTE: Places the data in file_path in *buffer (adds a \0 suffix).
B32 FileDump(U8* file_path, U8* buffer, U32 buffer_size);                    // NOTE: Replaces data in file_path with buffer (removes any \0 suffix).
B32 FileAppend(U8* file_path, U8* buffer, U32 buffer_size);                  // NOTE: Appends the data with buffer (removes any \0 suffix);
B32 FileCopy(U8* src_path, U8* dest_path);

B32 FileHandleOpen(FileHandle* file, U8* file_path, FileMode mode);
B32 FileHandleClose(FileHandle* file);
B32 FileHandleStat(FileHandle* file, FileStats* stats);
B32 FileHandleSeek(FileHandle* file, S32 distance, FileSeekPos pos);
B32 FileHandleRead(FileHandle* file, U8* buffer, U32 buffer_size, U32* bytes_read);
B32 FileHandleWrite(FileHandle* file, U8* buffer, U32 buffer_size, U32* bytes_written);

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
};

#define WIN_IO_LOG_ERROR(result, err)                                        \
  do {                                                                       \
    char _err_buffer[1024];                                                  \
    _err_buffer[0] = '\0';                                                   \
    FormatMessageA(                                                          \
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK,          \
        NULL, result, 0, _err_buffer, STATIC_ARRAY_SIZE(_err_buffer), NULL); \
    LOG_ERROR(err " - %s", _err_buffer);                                     \
  } while (0)
#define WIN_IO_LOG_ERROR_EX(result, fmt, ...)                                \
  do {                                                                       \
    char _err_buffer[1024];                                                  \
    _err_buffer[0] = '\0';                                                   \
    FormatMessageA(                                                          \
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK,          \
        NULL, result, 0, _err_buffer, STATIC_ARRAY_SIZE(_err_buffer), NULL); \
    LOG_ERROR(fmt " - %s", ##__VA_ARGS__, _err_buffer);                      \
  } while (0)

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
  stats->create_time = WIN_FileTimeToEpochSeconds(&attributes.ftCreationTime);
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
  stats->create_time = WIN_FileTimeToEpochSeconds(&attributes.ftCreationTime);
  stats->last_write_time = WIN_FileTimeToEpochSeconds(&attributes.ftLastWriteTime);
  stats->last_access_time = WIN_FileTimeToEpochSeconds(&attributes.ftLastAccessTime);
  return true;
}

B32 WIN_FileHandleOpen(FileHandle* file, U8* file_path, FileMode mode) {
  DWORD desired_access = 0;
  if (mode & FileMode_Read)  { desired_access |= GENERIC_READ;  }
  if (mode & FileMode_Write) { desired_access |= GENERIC_WRITE; }
  DEBUG_ASSERT(desired_access != 0);

  DWORD disposition = 0;
  B32 truncate = mode & FileMode_Truncate;
  B32 create   = mode & FileMode_Create;
  if (create && truncate) { disposition = CREATE_ALWAYS;     }
  else if (create)        { disposition = OPEN_ALWAYS;       }
  else if (truncate)      { disposition = TRUNCATE_EXISTING; }
  else                    { disposition = OPEN_EXISTING;     }

  // NOTE: allow shared reads, e.g. multiple sfx playing at the same time.
  // TODO: allow shared writes? e.g. for hotloading?
  file->handle = CreateFileA((const char*) file_path, desired_access, FILE_SHARE_READ, NULL, disposition, FILE_ATTRIBUTE_NORMAL, NULL);
  if (file->handle == INVALID_HANDLE_VALUE) {
    WIN_IO_LOG_ERROR_EX(GetLastError(), "[IO] Failed to open file: %s", file_path);
    return false;
  }
  file->arena = ArenaAllocate();
  file->file_path = CStringCopy(file->arena, file_path);
  return true;
}

B32 WIN_FileHandleClose(FileHandle* file) {
  ArenaRelease(file->arena);
  return CloseHandle(file->handle);
}

B32 WIN_FileHandleSeek(FileHandle* file, S32 distance, FileSeekPos pos) {
  DWORD move_method;
  switch (pos) {
    case FileSeekPos_Begin:   { move_method = FILE_BEGIN;   } break;
    case FileSeekPos_Current: { move_method = FILE_CURRENT; } break;
    case FileSeekPos_End:     { move_method = FILE_END;     } break;
  }
  DWORD result = SetFilePointer(file->handle, distance, NULL, move_method);

  if (result == INVALID_SET_FILE_POINTER) {
    WIN_IO_LOG_ERROR_EX(GetLastError(), "[IO] Failed to seek: %s", file->file_path);
    return false;
  }
  return true;
}

B32 WIN_FileHandleRead(FileHandle* file, U8* buffer, U32 buffer_size, U32* bytes_read) {
  DWORD read;
  BOOL result = ReadFile(file->handle, (LPVOID) buffer, buffer_size, &read, NULL);
  if (!result) {
    WIN_IO_LOG_ERROR_EX(GetLastError(), "[IO] Failed to read file: %s", file->file_path);
    return false;
  }
  if (bytes_read != NULL) { *bytes_read = read; }
  return true;
}

B32 WIN_FileHandleWrite(FileHandle* file, U8* buffer, U32 buffer_size, U32* bytes_written) {
  DWORD wrote;
  BOOL result = WriteFile(file->handle, buffer, buffer_size, &wrote, NULL);
  if (!result) {
    WIN_IO_LOG_ERROR_EX(GetLastError(), "[IO] Failed to write to file: %s", file->file_path);
    return false;
  }
  if (bytes_written != NULL) { *bytes_written = wrote; }
  return true;
}

#else

// TODO: linux support.
#error cdefault IO not supported on this OS.

#endif

#define CDEFAULT_IO_BACKEND_FN_IMPL(ns, fn) GLUE(ns, fn)
#define CDEFAULT_IO_BACKEND_FN(x) CDEFAULT_IO_BACKEND_FN_IMPL(CDEFAULT_IO_BACKEND_NAMESPACE, x)

B32 FileStat(U8* file_path, FileStats* stats) {
  return CDEFAULT_IO_BACKEND_FN(FileStat(file_path, stats));
}

B32 FileReadAll(Arena* arena, U8* file_path, U8** buffer, U32* buffer_size) {
  B32 success = false;
  FileHandle handle;
  if (!FileHandleOpen(&handle, file_path, FileMode_Read)) { return false; }
  FileStats stats;
  if (!FileHandleStat(&handle, &stats)) { goto file_read_all_exit; }
  *buffer = ARENA_PUSH_ARRAY(arena, U8, stats.size + 1);
  U32 bytes_read;
  if (!FileHandleRead(&handle, *buffer, stats.size, &bytes_read)) {
    ARENA_POP_ARRAY(arena, U8, stats.size);
    goto file_read_all_exit;
  }
  buffer[stats.size] = '\0';
  if (buffer_size != NULL) { *buffer_size = bytes_read + 1; }
  success = true;
file_read_all_exit:
  DEBUG_ASSERT(FileHandleClose(&handle));
  return success;
}

B32 FileDump(U8* file_path, U8* buffer, U32 buffer_size) {
  B32 success = false;
  FileHandle handle;
  if (buffer[buffer_size - 1] == '\0') { buffer_size -= 1; }
  if (!FileHandleOpen(&handle, file_path, FileMode_Write | FileMode_Create | FileMode_Truncate)) { return false; }
  if (!FileHandleWrite(&handle, buffer, buffer_size, NULL)) { goto file_dump_exit; }
  success = true;
file_dump_exit:
  DEBUG_ASSERT(FileHandleClose(&handle));
  return success;
}

B32 FileAppend(U8* file_path, U8* buffer, U32 buffer_size) {
  B32 success = false;
  FileHandle handle;
  if (buffer[buffer_size - 1] == '\0') { buffer_size -= 1; }
  if (!FileHandleOpen(&handle, file_path, FileMode_Write | FileMode_Create)) { return false; }
  if (!FileHandleSeek(&handle, 0, FileSeekPos_End)) { goto file_append_exit; }
  if (!FileHandleWrite(&handle, buffer, buffer_size, NULL)) { goto file_append_exit; }
  success = true;
file_append_exit:
  DEBUG_ASSERT(FileHandleClose(&handle));
  return success;
}

B32 FileCopy(U8* src_path, U8* dest_path) {
  B32 success = false;
  Arena* arena = ArenaAllocate();
  U8* buffer;
  U32 buffer_size;
  if (!FileReadAll(arena, src_path, &buffer, &buffer_size)) { goto file_copy_exit; }
  if (!FileDump(dest_path, buffer, buffer_size)) { goto file_copy_exit; }
  success = true;
file_copy_exit:
  ArenaRelease(arena);
  return success;
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

B32 FileHandleRead(FileHandle* file, U8* buffer, U32 buffer_size, U32* bytes_read) {
  return CDEFAULT_IO_BACKEND_FN(FileHandleRead(file, buffer, buffer_size, bytes_read));
}

B32 FileHandleWrite(FileHandle* file, U8* buffer, U32 buffer_size, U32* bytes_written) {
  return CDEFAULT_IO_BACKEND_FN(FileHandleWrite(file, buffer, buffer_size, bytes_written));
}

#endif // CDEFAULT_IO_IMPLEMENTATION
