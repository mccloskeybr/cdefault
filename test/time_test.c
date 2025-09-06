#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"

B32 StopwatchTest() {
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
  TEST_EXPECT(success);
  return true;
}

int main(void) {
  TimeInit();
  TEST(StopwatchTest());
  return 0;
}
