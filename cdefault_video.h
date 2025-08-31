#ifndef CDEFAULT_VIDEO_H_
#define CDEFAULT_VIDEO_H_

#include "cdefault_std.h"

typedef struct Window Window;
struct Window {
  U8 meme;
};

B32  VideoInit(void);
void VideoDeinit(void);

B32 VideoCreateWindow(Window* window, char* title);

#endif // CDEFAULT_VIDEO_H_

#ifdef CDEFAULT_VIDEO_IMPLEMENTATION
#undef CDEFAULT_VIDEO_IMPLEMENTATION

#if defined(OS_WINDOWS)
#define CDEFAULT_VIDEO_BACKEND_NAMESPACE WIN_

// NOTE: need to link with user32.lib
#include <windows.h>

static char* _win_WindowClass = "CDEFAULT_WINDOW_CLASS";

// TODO: anything interesting with this func?
static LRESULT CALLBACK WIN_WindowCallback(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam) {
  return DefWindowProc(hwnd, umsg, wparam, lparam);
}

B32 WIN_VideoInit() { return true; }

void WIN_VideoDeinit() {}

B32 WIN_VideoCreateWindow(Window* UNUSED(window), char* title) {
  HINSTANCE hinstance = GetModuleHandle(NULL);

  WNDCLASS window_class;
  MEMORY_ZERO_STRUCT(&window_class);
  window_class.lpfnWndProc = WIN_WindowCallback;
  window_class.hInstance = hinstance;
  window_class.lpszClassName = _win_WindowClass;
  RegisterClass(&window_class);

  HWND hwnd = CreateWindowEx(
      0, _win_WindowClass, title, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
      NULL, NULL, hinstance, NULL);
  ASSERT(hwnd != 0);

  ShowWindow(hwnd, SW_NORMAL);
  return true;
}

#else
#  error Unsupported OS for cdefault video.
#endif

#define CDEFAULT_VIDEO_BACKEND_FN_IMPL(ns, fn) GLUE(ns, fn)
#define CDEFAULT_VIDEO_BACKEND_FN(x) CDEFAULT_VIDEO_BACKEND_FN_IMPL(CDEFAULT_VIDEO_BACKEND_NAMESPACE, x)

B32 VideoInit(void) {
  return CDEFAULT_VIDEO_BACKEND_FN(VideoInit());
}

void VideoDeinit(void) {
  CDEFAULT_VIDEO_BACKEND_FN(VideoDeinit());
}

B32 VideoCreateWindow(Window* window, char* title) {
  return CDEFAULT_VIDEO_BACKEND_FN(VideoCreateWindow(window, title));
}

#endif // CDEFAULT_VIDEO_IMPLEMENTATION
