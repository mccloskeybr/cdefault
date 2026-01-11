#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

#define WINDOW_WIDTH  1920
#define WINDOW_HEIGHT 1080

static V2 camera_pos;
static V2 pos;
static V2 velocity;
static V2 enemies[128];
static V2 bullets_pos[128];
static V2 bullets_vel[128];

static F32 CamX(F32 x) { return x - camera_pos.x + WINDOW_WIDTH / 2; }
static F32 CamY(F32 y) { return y - camera_pos.y + WINDOW_HEIGHT / 2; }

int main(void) {
  DEBUG_ASSERT(LogInitStdOut());
  TimeInit();
  RandSeed(NULL, 12345);
  DEBUG_ASSERT(WindowInit(WINDOW_WIDTH, WINDOW_HEIGHT, "monstro"));
  RendererSetClearColor(0.39f, 0.58f, 0.92f, 1);

  F32 dt_s = 0.0f;
  Stopwatch frame_stopwatch, input_stopwatch, enemy_spawn_stopwatch;
  StopwatchInit(&frame_stopwatch);
  StopwatchInit(&input_stopwatch);
  StopwatchInit(&enemy_spawn_stopwatch);

  camera_pos = (V2) { 0, 0 };
  pos = (V2) { 0, 0 };

  while (!WindowShouldClose()) {
    if (WindowIsKeyPressed(Key_Control) && WindowIsKeyJustPressed(Key_C)) {
      LOG_INFO("SIG_INT received, closing.");
      exit(0);
    }
    camera_pos = V2Lerp(camera_pos, pos, 0.005f);

    if (WindowIsMouseButtonJustPressed(MouseButton_Left)) {
      V2 mouse_pos;
      WindowGetMousePositionV(&mouse_pos);
      mouse_pos.x -= WINDOW_WIDTH / 2;
      mouse_pos.x += camera_pos.x;
      mouse_pos.y -= WINDOW_HEIGHT / 2;
      mouse_pos.y += camera_pos.y;
      V2 dir = V2SubV2(mouse_pos, pos);
      if (V2LengthSq(dir) > 0) {
        dir = V2MultF32(V2Normalize(dir), 10);
        for (U32 i = 0; i < STATIC_ARRAY_SIZE(bullets_pos); i++) {
          if (V2LengthSq(bullets_pos[i]) == 0) {
            bullets_pos[i] = pos;
            bullets_vel[i] = dir;
            break;
          }
        }
      }
    }

    if (StopwatchReadSeconds(&input_stopwatch) > 0.75f) {
      StopwatchReset(&input_stopwatch);
      V2 dir = (V2) { RandF32(NULL, -1000, 1000), RandF32(NULL, -1000, 1000) };
      U32 i = 0;
      for (; i < STATIC_ARRAY_SIZE(enemies); i++) {
        if (V2LengthSq(enemies[i]) == 0) { continue; }
        V2 test = V2SubV2(enemies[i], pos);
        if (V2LengthSq(test) < V2LengthSq(dir)) {
          dir = test;
        }
      }
      if (V2LengthSq(dir) > 0) { dir = V2Normalize(dir); }
      velocity = V2AddV2(velocity, V2MultF32(dir, 10.0f));
    }
    pos = V2AddV2(pos, velocity);
    velocity = V2MultF32(velocity, 0.9f);

    if (StopwatchReadSeconds(&enemy_spawn_stopwatch) > 5.0f) {
      StopwatchReset(&enemy_spawn_stopwatch);
      for (U32 j = 0; j < 5; j++) {
        U32 i = 0;
        for (; i < STATIC_ARRAY_SIZE(enemies); i++) {
          if (V2LengthSq(enemies[i]) == 0) { break; }
        }
        enemies[i].x = RandF32(NULL, pos.x - 1000, pos.x + 1000);
        enemies[i].y = RandF32(NULL, pos.y - 1000, pos.y + 1000);
      }
    }

    for (U32 i = 0; i < STATIC_ARRAY_SIZE(bullets_pos); i++) {
      if (V2LengthSq(bullets_pos[i]) == 0) { continue; }
      V2 dist = V2SubV2(bullets_pos[i], pos);
      if (V2Length(dist) > 1000) {
        bullets_pos[i] = (V2) { 0, 0 };
        continue;
      }
      bullets_pos[i] = V2AddV2(bullets_pos[i], bullets_vel[i]);
      DrawCircle(CamX(bullets_pos[i].x), CamY(bullets_pos[i].y), 10, 0, 0, 0);
    }

    for (U32 i = 0; i < STATIC_ARRAY_SIZE(enemies); i++) {
      if (V2LengthSq(enemies[i]) == 0) { continue; }
      if (Circle2IntersectCircle2(pos, 50, enemies[i], 50, NULL)) {
        enemies[i] = (V2) { 0, 0 };
        continue;
      }

      B32 deleted = false;
      for (U32 j = 0; j < STATIC_ARRAY_SIZE(bullets_pos); j++) {
        if (V2LengthSq(bullets_pos[j]) == 0) { continue; }
        if (Circle2IntersectCircle2(enemies[i], 50, bullets_pos[j], 10, NULL)) {
          bullets_pos[j] = (V2) { 0, 0 };
          enemies[i] = (V2) { 0, 0 };
          deleted = true;
          break;
        }
      }
      if (deleted) { continue; }

      V2 dir = V2SubV2(pos, enemies[i]);
      if (V2LengthSq(dir) > 0) { dir = V2Normalize(dir); }
      dir = V2MultF32(dir, 2.0f);
      enemies[i] = V2AddV2(enemies[i], dir);
      DrawCircle(CamX(enemies[i].x), CamY(enemies[i].y), 50, 0, 0, 1);
    }

    DrawCircle(CamX(pos.x), CamY(pos.y), 50, 1, 0, 0);

#if 0
    do { dt_s = StopwatchReadSeconds(&frame_stopwatch); } while (dt_s < 0.016);
#else
    dt_s = StopwatchReadSeconds(&frame_stopwatch);
#endif
    StopwatchReset(&frame_stopwatch);
    // LOG_INFO("%.05f", dt_s);
    WindowSwapBuffers();
    WindowFlushEvents();
  }

  WindowDeinit();
  return 0;
}
