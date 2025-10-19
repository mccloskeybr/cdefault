#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_TEST_IMPLEMENTATION
#include "../cdefault_test.h"

void StopwatchTest(void) {
  Stopwatch stopwatch;
  StopwatchInit(&stopwatch);
  // NOTE: try a bunch of times in case we get a bad reading.
  B32 success = false;
  for (S32 i = 0; i < 50; i++) {
    StopwatchReset(&stopwatch);
    SleepMs(50);
    F32 time = StopwatchReadSeconds(&stopwatch);
    success = 0.025f < time && time < 0.075f;
    if (success) { break; }
  }
  EXPECT_TRUE(success);
}

int main(void) {
  TimeInit();
  RUN_TEST(StopwatchTest);
  LogTestReport();
  return 0;
}
