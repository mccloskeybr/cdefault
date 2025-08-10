#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"

B32 LogCustomFileTest(void) {
#ifdef OS_LINUX
  U8 buffer[KB(1)] = {0};
  FILE* log_file = fmemopen(buffer, sizeof(buffer), "w+");
  ASSERT(log_file != NULL);
  LogInit(log_file);
  LOG_INFO("test_1");
  LOG_INFO("test_2");
  fseek(log_file, 0, SEEK_SET);
  TEST_EXPECT(strcmp(buffer, "[INFO | log_test.c:14]: test_1\n[INFO | log_test.c:15]: test_2\n") == 0);
  LogInit(stderr);
  return true;
#else
  LOG_WARN("LogCustomFileTest not supported on this OS.");
  return false;
#endif
}

int main(void) {
  TEST(LogCustomFileTest());
  return 0;
}
