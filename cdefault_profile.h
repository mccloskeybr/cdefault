#ifndef CDEFAULT_PROFILE_H_
#define CDEFAULT_PROFILE_H_

#include "cdefault_std.h"

// NOTE: Metrics need to be manually registered in this enum.
typedef enum ProfileMetricType ProfileMetricType;
enum ProfileMetricType {
  // ...
  ProfileMetricType_Count,
};

// NOTE: Profiling is inherently stack-shaped. Therefore, when profiling a section, unless it
// is the root anchor, it is always being called inside another anchor's time window. Consider:
//
// A |-----------|
// B    |-----|
//
// elapsed_exclusive measures time that is unique *only* to that given metric / anchor --
// A's elapsed_exclusive does not contain any of the times B is measured. elapsed_inclusive conversely
// does include the overlap time. in the case of recursive calls, only the top-level time is maintained.
typedef struct ProfileAnchor ProfileAnchor;
struct ProfileAnchor {
  U64 num_hits;
  U64 elapsed_exclusive; // Does not include child-anchor times.
  U64 elapsed_inclusive; // Includes child-anchor times.
};

// NOTE: Surround critical blocks with PROFILE_START / PROFILE_END to time them.
#ifndef NDEBUG
#  define PROFILE_START(metric) _ProfileBlockStart(metric);
#  define PROFILE_END(metric) _ProfileBlockEnd(metric);
#else
#  define PROFILE_START(metric)
#  define PROFILE_END(metric)
#endif

// NOTE: GetAnchor and Reset are expected to be called in a centralized profiling / debug controller.
ProfileAnchor* ProfileGetAnchor(ProfileMetricType metric);
void ProfileReset(void);
void _ProfileBlockStart(ProfileMetricType metric);
void _ProfileBlockEnd(ProfileMetricType metric);

#endif // CDEFAULT_PROFILE_H_

#ifdef CDEFAULT_PROFILE_IMPLEMENTATION
#undef CDEFAULT_PROFILE_IMPLEMENTATION

typedef struct ProfileBlock ProfileBlock;
struct ProfileBlock {
  U32 parent_index;
  ProfileMetricType metric;
  U64 start;
  U64 elapsed_inclusive_snapshot;
};

typedef struct ProfileContext ProfileContext;
struct ProfileContext {
  ProfileAnchor profile_zones[ProfileMetricType_Count];
  ProfileBlock profile_block_stack[128];
  U32 next_profile_block;
};
static ProfileContext _profile_context;

static U64 ReadCpuTimer() {
  return __rdtsc();
}

ProfileAnchor* ProfileGetAnchor(ProfileMetricType metric) {
  ProfileContext* ctx = &_profile_context;
  return &ctx->profile_zones[metric];
}

void ProfileReset(void) {
  ProfileContext* ctx = &_profile_context;
  DEBUG_ASSERT(ctx->next_profile_block == 0);
  MEMORY_ZERO_STATIC_ARRAY(ctx->profile_zones);
}

void _ProfileBlockStart(ProfileMetricType metric) {
  ProfileContext* ctx = &_profile_context;
  ProfileAnchor* anchor = &ctx->profile_zones[metric];
  ProfileBlock* block = &ctx->profile_block_stack[ctx->next_profile_block];
  ctx->next_profile_block++;
  DEBUG_ASSERT(ctx->next_profile_block <= STATIC_ARRAY_SIZE(ctx->profile_block_stack));
  MEMORY_ZERO_STRUCT(block);
  block->metric = metric;
  block->elapsed_inclusive_snapshot = anchor->elapsed_inclusive;
  block->start = ReadCpuTimer();
}

void _ProfileBlockEnd(ProfileMetricType metric) {
  U64 end = ReadCpuTimer();
  ProfileContext* ctx = &_profile_context;
  ProfileBlock* block = &ctx->profile_block_stack[ctx->next_profile_block - 1];
  DEBUG_ASSERT(block->metric == metric);
  ctx->next_profile_block -= 1;
  ProfileAnchor* anchor = &ctx->profile_zones[block->metric];
  U64 elapsed = end - block->start;

  // parents have already have the child time subtracted, so just add the current time back.
  anchor->elapsed_exclusive += elapsed;
  // we clobber the inclusive time for recursive calls -- the topmost layer wins.
  anchor->elapsed_inclusive = block->elapsed_inclusive_snapshot + elapsed;
  anchor->num_hits++;

  if (ctx->next_profile_block > 0) {
    ProfileBlock* block_parent = &ctx->profile_block_stack[ctx->next_profile_block - 1];
    ProfileAnchor* anchor_parent = &ctx->profile_zones[block_parent->metric];
    anchor_parent->elapsed_exclusive -= elapsed;
  }
}

#endif // CDEFAULT_PROFILE_IMPLEMENTATION
