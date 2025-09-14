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
static V2 enemy_pos;
static F32 radar_theta;

static void DrawRadar() {
  V2 radar_center = { WINDOW_WIDTH / 2 , WINDOW_HEIGHT / 2};

  // NOTE: outer ring
  DrawRingV(radar_center, RADAR_RADIUS, 20, (V3) { 0, 0, 0 });

  // NOTE: sweep
  Line2 radar;
  radar.start = radar_center;
  radar.end.x = F32Cos(radar_theta);
  radar.end.y = F32Sin(radar_theta);
  V2MultF32(&radar.end, &radar.end, RADAR_RADIUS);
  V2AddV2(&radar.end, &radar.start, &radar.end);
  DrawLineV(radar.start, radar.end, 5, (V3) { 0, 0, 0 });
}

static void DrawBlip() {
  V2 radar_center = { WINDOW_WIDTH / 2 , WINDOW_HEIGHT / 2};

  Circle2 blip;
  blip.radius = 50;
  blip.center_point = enemy_pos;
  V2AddV2(&blip.center_point, &blip.center_point, &radar_center);
  DrawCircleV(blip.center_point, blip.radius, V3_RED);
}

int main(void) {
  TimeInit();
  ASSERT(WindowInit(WINDOW_WIDTH, WINDOW_HEIGHT, "space wars"));
  RendererSetClearColor(0.39f, 0.58f, 0.92f, 1);

  F32 dt_s = 0.0f;
  Stopwatch frame_stopwatch;
  StopwatchInit(&frame_stopwatch);

  self_pos.x = 0;
  self_pos.y = 0;
  enemy_pos.x = 0;
  enemy_pos.y = 100;

  while (!WindowShouldClose()) {
    WindowFlushEvents();
    if (WindowIsKeyPressed(Key_Control) && WindowIsKeyJustPressed(Key_C)) {
      LOG_INFO("SIGINT received");
      exit(0);
    }

    radar_theta -= 0.05f;
    DrawRadar();
    Line2 radar_scan;
    radar_scan.start = self_pos;
    radar_scan.end.x = F32Cos(radar_theta);
    radar_scan.end.y = F32Sin(radar_theta);
    V2MultF32(&radar_scan.end, &radar_scan.end, RADAR_RADIUS);
    Circle2 enemy;
    enemy.center_point = enemy_pos;
    enemy.radius = 50;
    if (Line2IntersectCircle2(&radar_scan, &enemy, NULL, NULL)) {
      DrawBlip();
    }

    do { dt_s = StopwatchReadSeconds(&frame_stopwatch); } while (dt_s < DT_S_TARGET);
    StopwatchReset(&frame_stopwatch);
    WindowSwapBuffers();
  }

  WindowDeinit();
  return 0;
}
