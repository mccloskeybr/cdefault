#define CDEFAULT_IMPLEMENTATION
#include "../cdefault.h"

int main(void) {
  TimeInit();
  ASSERT(WindowInit(1920, 1080, "render shapes example"));
  RendererSetClearColor(0.39f, 0.58f, 0.92f, 1);

  V3 red = { 1, 0, 0 };
  V3 blue = { 0, 0, 1 };
  F32 theta = 0.0f;
  F32 scale = 0.0f;
  while (!WindowShouldClose()) {
    if (WindowIsKeyPressed(Key_Control) && WindowIsKeyJustPressed(Key_C)) {
      LOG_INFO("SIGINT received");
      exit(0);
    }

    V2 mouse_pos;
    WindowGetMousePositionV(&mouse_pos);
    RendererCastRayV(mouse_pos, &mouse_pos);
    theta += 0.005f;
    scale = F32Abs(F32Cos(TimeSecondsSinceStart()));

    DrawRectangleRotV(mouse_pos, (V2) { 50, 50 }, 1 - theta, blue);
    DrawTriangle(200, 200, 400, 200, 300, 300, red.r, red.g, red.b);
    DrawRectangle(600, 200, 100 * scale, 100, 1, 0, 0);
    DrawRectangle(600, 200, 80, 80 * scale, 0, 1, 0);
    DrawRoundedRectangleFrame(800, 350, 100, 100, 10, 20, 0, 0, 1);
    DrawRoundedRectangleFrameRot(800, 200, 100, 100, 20, 1, theta, 0, 0, 1);
    DrawRing(1000, 200, 50, 5, 1, 0, 0);
    DrawRing(1000, 200, 45 * (1 - scale), 5, 0, 1, 0);
    DrawLine(200, 400, 1000, 500, 25, 1, 1, 1);

    WindowSwapBuffers();
    WindowFlushEvents();
  }

  WindowDeinit();
  return 0;
}
