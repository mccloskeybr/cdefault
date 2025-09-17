#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_IO_IMPLEMENTATION
#include "../cdefault_io.h"

int main(void) {
  DEBUG_ASSERT(DirSetCurrentToExeDir());

  Arena* arena = ArenaAllocate();
  U8* path;
  U32 path_size;
  DEBUG_ASSERT(DirGetCurrent(arena, &path, &path_size));
  LOG_INFO("%s", path);

  return 0;
}
