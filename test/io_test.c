#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_IO_IMPLEMENTATION
#include "../cdefault_io.h"

int main(void) {
  Arena* arena = ArenaAllocate();
  U8* buffer;
  U32 bytes_read;
  DEBUG_ASSERT(FileReadAll(arena, (U8*) "apple.txt", &buffer, &bytes_read));
  LOG_INFO("hi: %d %s", bytes_read, buffer);

  U8 test[3] = "hi";
  DEBUG_ASSERT(FileAppend((U8*) "bingus.txt", (U8*) test, 3));
  DEBUG_ASSERT(FileCopy((U8*) "bingus.txt", (U8*) "bingus_copy.txt"));

  return 0;
}
