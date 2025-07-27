#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

int main(void) {
  U8 buffer[1024] = {0};
  FILE* log_file = fmemopen(buffer, sizeof(buffer), "w+");
  if (log_file == NULL) { LOG_ERROR("Unable to open test log buffer."); }
  LogInit(log_file);
  LOG_INFO("test");
  ASSERT(strcmp(buffer, "[INFO | log_test.c:10] test"));

  LogInit(stderr);
  LOG_INFO("[SUCCESS] LogTest");
}
