#define PROFILE_REGISTRY(PROFILE_METRIC) \
  PROFILE_METRIC(JSON_LOAD)

#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

#define WARMUP_RUNS    10
#define BENCHMARK_RUNS 500

int main(void) {
  DirSetCurrentToExeDir();
  String8 file_buffer;
  Arena* file_arena = ArenaAllocate();
  DEBUG_ASSERT(FileReadAll(file_arena, Str8Lit("../data/test_json.json"), &file_buffer.str, &file_buffer.size));

  Arena* json_arena = ArenaAllocate();
  U64 base = ArenaPos(json_arena);

  for (S32 i = 0; i < WARMUP_RUNS; i++) {
    JsonObject json;
    DEBUG_ASSERT(JsonParse(json_arena, &json, file_buffer));
    ArenaPopTo(json_arena, base);
  }

  U64 min = U64_MAX;
  U64 max = U64_MIN;
  U64 avg = 0;
  for (S32 i = 0; i < BENCHMARK_RUNS; i++) {
    JsonObject json;

    PROFILE_START(JSON_LOAD);
    JsonParse(json_arena, &json, file_buffer);
    PROFILE_END(JSON_LOAD);

    ProfileAnchor anchor = ProfileGetAnchor(JSON_LOAD);

    avg += anchor.elapsed_inclusive;
    max = MAX(max, anchor.elapsed_inclusive);
    min = MIN(min, anchor.elapsed_inclusive);

    ArenaPopTo(json_arena, base);
    ProfileReset();
  }
  avg /= BENCHMARK_RUNS;
  LOG_INFO("MIN: %lu, MAX %lu, AVG: %lu", min, max, avg);

  return 0;
}
