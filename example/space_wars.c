#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_MATH_IMPLEMENTATION
#include "../cdefault_math.h"
#define CDEFAULT_GEOMETRY_IMPLEMENTATION
#include "../cdefault_geometry.h"
#define CDEFAULT_RENDER_IMPLEMENTATION
#include "../cdefault_render.h"

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 720
#define DT_S_TARGET   0.016
#define RADAR_RADIUS  300

static V2 self_pos;
static V2 target_pos;

static F32 radar_sweep_theta = 0;
static void DrawRadar(F32 dt_s) {
  V2 center = { WINDOW_WIDTH / 2 , WINDOW_HEIGHT / 2};

  // NOTE: outer ring
  DrawRingV(center, RADAR_RADIUS, 20, (V3) { 0, 0, 0 });

  // NOTE: sweep
  V2 sweep_dir = { F32Cos(radar_sweep_theta), F32Sin(radar_sweep_theta) };
  V2 sweep_end;
  V2MultF32(&sweep_end, &sweep_dir, RADAR_RADIUS);
  V2AddV2(&sweep_end, &center, &sweep_end);
  DrawLineV(center, sweep_end, 5, (V3) { 0, 0, 0 });
  radar_sweep_theta -= dt_s;

  // NOTE: blip
  Aabb2 enemy;
  enemy.center_point = target_pos;
  enemy.size = (V2) { 10, 10 };
  Ray2 beam;
  beam.start = self_pos;
  beam.dir = sweep_dir;
  if (IntersectionRay2Aabb2(&beam, &enemy, NULL, NULL)) {
    DrawRectangleV(enemy.center_point, enemy.size, (V3) { 1, 0, 0 });
  }
}

int main(void) {
  TimeInit();
  ASSERT(WindowInit(WINDOW_WIDTH, WINDOW_HEIGHT, "space wars"));
  RendererSetClearColor(0.39f, 0.58f, 0.92f, 1);

  F32 dt_s = 0.0f;
  Stopwatch frame_stopwatch;
  StopwatchInit(&frame_stopwatch);

  self_pos = (V2) { 0, 0 };
  target_pos = (V2) { 0, 10 };

  while (!WindowShouldClose()) {
    WindowFlushEvents();

    if (WindowIsKeyPressed(Key_Control) && WindowIsKeyJustPressed(Key_C)) {
      LOG_INFO("SIGINT received");
      exit(0);
    }

    DrawRadar(dt_s);

    do { dt_s = StopwatchReadSeconds(&frame_stopwatch); } while (dt_s < DT_S_TARGET);
    StopwatchReset(&frame_stopwatch);

    WindowSwapBuffers();
  }

  WindowDeinit();
  return 0;
}
