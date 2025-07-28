#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

B32 LogCustomFileTest(void) {
  U8 buffer[KB(1)] = {0};
  FILE* log_file = fmemopen(buffer, sizeof(buffer), "w+");
  ASSERT(log_file != NULL);
  LogInit(log_file);
  LOG_INFO("test_1");
  LOG_INFO("test_2");
  fseek(log_file, 0, SEEK_SET);
  TEST_EXPECT(strcmp(buffer, "[INFO | log_test.c:13]: test_1\n[INFO | log_test.c:14]: test_2\n") == 0);
  LogInit(stderr);
  return true;
}

int main(void) {
  TEST(LogCustomFileTest);
  return 0;
}
