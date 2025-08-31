#ifndef CDEFAULT_VIDEO_H_
#define CDEFAULT_VIDEO_H_

#include "cdefault_std.h"

// NOTE: Sensible defaults are chosen if field values are 0.
typedef struct WindowInitOpts WindowInitOpts;
struct WindowInitOpts {
  S32 x;
  S32 y;
  S32 width;
  S32 height;
  char* title;
};
B32  WindowInit(WindowInitOpts opts);
void WindowDeinit();
void WindowSwapBuffers();
void WindowGetDims(S32* x, S32* y, S32* width, S32* height);

#endif // CDEFAULT_VIDEO_H_

#ifdef CDEFAULT_VIDEO_IMPLEMENTATION
#undef CDEFAULT_VIDEO_IMPLEMENTATION

#if defined(OS_WINDOWS)
#define CDEFAULT_VIDEO_BACKEND_NAMESPACE WIN_

// NOTE: need to link with user32.lib opengl32.lib gdi32.lib
#include <windows.h>
#include <GL/GL.h>

#define WGL_CONTEXT_MAJOR_VERSION_ARB             0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB             0x2092
#define WGL_CONTEXT_FLAGS_ARB                     0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB              0x9126
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#define GL_FRAMEBUFFER_SRGB                       0x8DB9

typedef HGLRC WINAPI wglCreateContextAttribsARB_Fn(HDC, HGLRC, const int*);

typedef struct WIN_Window WIN_Window;
struct WIN_Window {
  B32 initialized;
  HWND handle;
  HDC device_context;
  HGLRC ogl_context;
};
static WIN_Window _win_window;

// TODO: instead of printing GetLastError(), do some formatting using FormatMessage?

// TODO: custom on close callback?
static LRESULT CALLBACK WIN_WindowCallback(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam) {
  return DefWindowProc(hwnd, umsg, wparam, lparam);
}

B32 WIN_WindowInit(WindowInitOpts opts) {
  WIN_Window* window = &_win_window;
  DEBUG_ASSERT(!window->initialized);

  if (opts.x == 0)        { opts.x = CW_USEDEFAULT;      }
  if (opts.y == 0)        { opts.y = CW_USEDEFAULT;      }
  if (opts.width == 0)    { opts.width = CW_USEDEFAULT;  }
  if (opts.height == 0)   { opts.height = CW_USEDEFAULT; }
  if (opts.title == NULL) { opts.title = "title";        }

  B32 success = false;

  HINSTANCE hinstance = GetModuleHandle(NULL);

  WNDCLASS window_class;
  MEMORY_ZERO_STRUCT(&window_class);
  window_class.lpfnWndProc = WIN_WindowCallback;
  window_class.hInstance = hinstance;
  window_class.lpszClassName = "CDEFAULT_WINDOW_CLASS";
  RegisterClass(&window_class);

  DWORD create_window_flags = 0;
  create_window_flags |= WS_OVERLAPPEDWINDOW;
  create_window_flags |= WS_VISIBLE;

  HWND handle = CreateWindowExA(
      0, window_class.lpszClassName, opts.title, create_window_flags,
      opts.x, opts.y, opts.width, opts.height, NULL, NULL, hinstance, NULL);
  if (handle == NULL) {
    LOG_ERROR("[VIDEO] Failed to create window: %d", GetLastError());
    goto win_window_create_exit;
  }
  HDC device_context = GetDC(handle);

  PIXELFORMATDESCRIPTOR desired_pixel_format;
  MEMORY_ZERO_STRUCT(&desired_pixel_format);
  desired_pixel_format.nSize = sizeof(desired_pixel_format);
  desired_pixel_format.nVersion = 1;
  desired_pixel_format.iPixelType = PFD_TYPE_RGBA;
  desired_pixel_format.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
  desired_pixel_format.cColorBits = 32;
  desired_pixel_format.cAlphaBits = 8;
  desired_pixel_format.iLayerType = PFD_MAIN_PLANE;
  S32 suggested_pixel_format_idx = ChoosePixelFormat(device_context, &desired_pixel_format);
  if (suggested_pixel_format_idx == 0) {
    LOG_ERROR("[VIDEO] Failed to choose a pixel format: %d", GetLastError());
    goto win_window_create_exit;
  }
  PIXELFORMATDESCRIPTOR suggested_pixel_format_desc;
  if (!DescribePixelFormat(device_context, suggested_pixel_format_idx, sizeof(suggested_pixel_format_desc), &suggested_pixel_format_desc)) {
    LOG_ERROR("[VIDEO] Failed to describe pixel format: %d", GetLastError());
    goto win_window_create_exit;
  }
  if (!SetPixelFormat(device_context, suggested_pixel_format_idx, &suggested_pixel_format_desc)) {
    LOG_ERROR("[VIDEO] Failed to set pixel format: %d", GetLastError());
    goto win_window_create_exit;
  }

  // NOTE: make fake ogl context first.
  HGLRC ogl_context = wglCreateContext(device_context);
  if (ogl_context == NULL) {
    LOG_ERROR("[VIDEO] Failed to create OpenGL context: %d", GetLastError());
    goto win_window_create_exit;
  }
  if (wglMakeCurrent(device_context, ogl_context) == FALSE) {
    LOG_ERROR("[VIDEO] Failed to attach the OpenGL context: %d", GetLastError());
    wglDeleteContext(ogl_context);
    goto win_window_create_exit;
  }
  wglCreateContextAttribsARB_Fn* wglCreateContextAttribsARB =
    (wglCreateContextAttribsARB_Fn*) wglGetProcAddress("wglCreateContextAttribsARB");
  if (wglCreateContextAttribsARB == NULL) {
    LOG_ERROR("[VIDEO] Failed to find wglCreateContextAttribsARB function pointer.");
    wglDeleteContext(ogl_context);
    goto win_window_create_exit;
  }
  wglDeleteContext(ogl_context);

  // NOTE: make real ogl context.
  const S32 ogl_attr[] = {
    WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
    WGL_CONTEXT_MINOR_VERSION_ARB, 0,
    WGL_CONTEXT_FLAGS_ARB, 0,
    WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
    0
  };
  ogl_context = wglCreateContextAttribsARB(device_context, 0, ogl_attr);
  if (!wglMakeCurrent(device_context, ogl_context)) {
    LOG_ERROR("Failed to create OpenGL context: %d", GetLastError());
    goto win_window_create_exit;
  }

  // TODO: verify availability.
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_FRAMEBUFFER_SRGB);
  glEnable(GL_CULL_FACE);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  glClearColor(1.0f, 0.35f, 0.35f, 0);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  window->handle = handle;
  window->device_context = device_context;
  window->ogl_context = ogl_context;
  window->initialized = true;

  success = true;

win_window_create_exit:
  if (!success) { if (handle != NULL) { CloseWindow(handle); } }
  return success;
}

void WIN_WindowDeinit() {
  WIN_Window* window = &_win_window;
  DEBUG_ASSERT(window->initialized);
  wglMakeCurrent(window->device_context, NULL);
  wglDeleteContext(window->ogl_context);
  CloseWindow(window->handle);
}

void WIN_WindowSwapBuffers() {
  WIN_Window* window = &_win_window;
  DEBUG_ASSERT(window->initialized);
  SwapBuffers(window->device_context);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void WIN_WindowGetDims(S32* x, S32* y, S32* width, S32* height) {
  WIN_Window* window = &_win_window;
  DEBUG_ASSERT(window->initialized);
  RECT dims;
  GetClientRect(window->handle, &dims);
  *x = dims.left;
  *y = dims.top;
  *width = dims.right - dims.left;
  *height = dims.bottom - dims.top;
}

#else
#  error Unsupported OS for cdefault video.
#endif

#define CDEFAULT_VIDEO_BACKEND_FN_IMPL(ns, fn) GLUE(ns, fn)
#define CDEFAULT_VIDEO_BACKEND_FN(x) CDEFAULT_VIDEO_BACKEND_FN_IMPL(CDEFAULT_VIDEO_BACKEND_NAMESPACE, x)

B32 WindowInit(WindowInitOpts opts) {
  return CDEFAULT_VIDEO_BACKEND_FN(WindowInit(opts));
}

void WindowDeinit() {
  CDEFAULT_VIDEO_BACKEND_FN(WindowDeinit());
}

void WindowSwapBuffers() {
  CDEFAULT_VIDEO_BACKEND_FN(WindowSwapBuffers());
}

void WindowGetDims(S32* x, S32* y, S32* width, S32* height) {
  CDEFAULT_VIDEO_BACKEND_FN(WindowGetDims(x, y, width, height));
}

#endif // CDEFAULT_VIDEO_IMPLEMENTATION
