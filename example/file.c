#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_IO_IMPLEMENTATION
#include "../cdefault_io.h"

int main(void) {
  DEBUG_ASSERT(DirSetCurrentToExeDir());
  Arena* arena = ArenaAllocate();

  String8List paths;
  DEBUG_ASSERT(DirListFiles(arena, Str8Lit("../"), &paths));
  for (String8ListNode* curr = paths.head; curr != NULL; curr = curr->next) {
    LOG_INFO("%.*s", curr->string.size, curr->string.str);
  }

  LOG_INFO("done");
  return 0;
}
