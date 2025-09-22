#ifndef CDEFAULT_RENDER_H_
#define CDEFAULT_RENDER_H_

#include "cdefault_std.h"
#include "cdefault_math.h"

// TODO:
// separate renderer from window? more intuitive to separate 2d vs 3d renderer then.
// vsync
// shader abstraction?
// draw more shapes
// softer edges?

// NOTE: A window must exist before any renderer / draw functions can be called.
// NOTE: Sensible defaults are chosen if field values are 0.
B32  WindowInit(S32 width, S32 height, char* title);
void WindowDeinit();
void WindowFlushEvents(); // NOTE: Must be called regularly, e.g. on some frame boundary.
B32  WindowShouldClose(); // NOTE: True when the window has been closed.
void WindowSwapBuffers();
void WindowGetDims(S32* x, S32* y, S32* width, S32* height); // NOTE: viewport dimensions; not necessarily the raw window size.
void WindowSetTitle(char* title);
void WindowSetSize(S32 width, S32 height); // NOTE: Does not update resolution, may need to call RendererSetProjection separately.
void WindowShowCursor(B32 enable);
void WindowFullscreen(B32 enable);

typedef enum KeyboardKey KeyboardKey;
typedef enum MouseButton MouseButton;
B32  WindowIsKeyPressed(KeyboardKey key);
B32  WindowIsKeyReleased(KeyboardKey key);
B32  WindowIsKeyJustPressed(KeyboardKey key);
B32  WindowIsKeyJustReleased(KeyboardKey key);
B32  WindowIsMouseButtonPressed(MouseButton button);
B32  WindowIsMouseButtonReleased(MouseButton button);
B32  WindowIsMouseButtonJustPressed(MouseButton button);
B32  WindowIsMouseButtonJustReleased(MouseButton button);
F32  WindowGetMouseScrollSign();
void WindowGetMousePosition(F32* x, F32* y); // NOTE: In screen coordinates. May need to project to renderer / viewport space separately (RendererCastRay).
void WindowGetMousePositionV(V2* pos);
void WindowGetMouseDeltaPosition(F32* x, F32* y);
void WindowGetMouseDeltaPositionV(V2* pos);

void RendererSetProjection(M4 projection);
void RendererRegisterImage(U32* image_handle, U8* image_bytes, U32 width, U32 height); // NOTE: Expects RGBA byte values (0 -> 255)
void RendererReleaseImage(U32 image_handle);
void RendererEnableScissorTest(S32 x, S32 y, S32 width, S32 height);
void RendererDisableScissorTest(void);
void RendererEnableDepthTest(void);
void RendererDisableDepthTest(void);
void RendererSetClearColor(F32 red, F32 green, F32 blue, F32 alpha);
void RendererSetClearColorV(V4 color);
void RendererCastRay(F32 x, F32 y, V2* intersect);
void RendererCastRayV(V2 pos, V2* intersect);
void RendererCastRay3(F32 x, F32 y, V3* start, V3* dir);
void RendererCastRay3V(V2 pos, V3* start, V3* dir);

void DrawLine(F32 start_x, F32 start_y, F32 end_x, F32 end_y, F32 thickness, F32 red, F32 green, F32 blue);
void DrawLineV(V2 start, V2 end, F32 thickness, V3 color);
void DrawRectangle(F32 center_x, F32 center_y, F32 width, F32 height, F32 red, F32 green, F32 blue);
void DrawRectangleV(V2 center, V2 size, V3 color);
void DrawRectangleFrame(F32 center_x, F32 center_y, F32 width, F32 height, F32 border, F32 red, F32 green, F32 blue);
void DrawRectangleFrameV(V2 center, V2 size, F32 border, V3 color);
void DrawRectangleRot(F32 center_x, F32 center_y, F32 width, F32 height, F32 andle_rad, F32 red, F32 green, F32 blue);
void DrawRectangleRotV(V2 center, V2 size, F32 angle_rad, V3 color);
void DrawRectangleFrameRot(F32 center_x, F32 center_y, F32 width, F32 height, F32 border, F32 angle_rad, F32 red, F32 green, F32 blue);
void DrawRectangleFrameRotV(V2 center, V2 size, F32 border, F32 angle_rad,  V3 color);
void DrawRoundedRectangle(F32 center_x, F32 center_y, F32 width, F32 height, F32 radius, F32 red, F32 green, F32 blue);
void DrawRoundedRectangleV(V2 center, V2 size, F32 radius, V3 color);
void DrawRoundedRectangleFrame(F32 center_x, F32 center_y, F32 width, F32 height, F32 radius, F32 border, F32 red, F32 green, F32 blue);
void DrawRoundedRectangleFrameV(V2 center, V2 size, F32 radius, F32 border, V3 color);
void DrawRoundedRectangleRot(F32 center_x, F32 center_y, F32 width, F32 height, F32 radius, F32 angle_rad, F32 red, F32 green, F32 blue);
void DrawRoundedRectangleRotV(V2 center, V2 size, F32 radius, F32 angle_rad, V3 color);
void DrawRoundedRectangleFrameRot(F32 center_x, F32 center_y, F32 width, F32 height, F32 radius, F32 border, F32 angle_rad, F32 red, F32 green, F32 blue);
void DrawRoundedRectangleFrameRotV(V2 center, V2 size, F32 radius, F32 angle_rad, F32 border, V3 color);
void DrawCircle(F32 center_x, F32 center_y, F32 radius, F32 red, F32 green, F32 blue);
void DrawCircleV(V2 center, F32 radius, V3 color);
void DrawRing(F32 center_x, F32 center_y, F32 radius, F32 border, F32 red, F32 green, F32 blue);
void DrawRingV(V2 center, F32 radius, F32 border, V3 color);
void DrawTriangle(F32 x1, F32 y1, F32 x2, F32 y2, F32 x3, F32 y3, F32 red, F32 green, F32 blue); // NOTE: must respect CCW winding order.
void DrawTriangleV(V2 p1, V2 p2, V2 p3, V3 color);
void DrawConvexHullV(V2* points, U32 points_len, V3 color);
void DrawImage(U32 image_handle, F32 center_x, F32 center_y, F32 width, F32 height);
void DrawImageV(U32 image_handle, V2 pos, V2 size);
void DrawImageRot(U32 image_handle, F32 center_x, F32 center_y, F32 width, F32 height, F32 angle_rad);
void DrawImageRotV(U32 image_handle, V2 pos, V2 size, F32 angle_rad);

enum KeyboardKey {
  Key_A,
  Key_B,
  Key_C,
  Key_D,
  Key_E,
  Key_F,
  Key_G,
  Key_H,
  Key_I,
  Key_J,
  Key_K,
  Key_L,
  Key_M,
  Key_N,
  Key_O,
  Key_P,
  Key_Q,
  Key_R,
  Key_S,
  Key_T,
  Key_U,
  Key_V,
  Key_W,
  Key_X,
  Key_Y,
  Key_Z,
  Key_0,
  Key_1,
  Key_2,
  Key_3,
  Key_4,
  Key_5,
  Key_6,
  Key_7,
  Key_8,
  Key_9,
  Key_F1,
  Key_F2,
  Key_F3,
  Key_F4,
  Key_F5,
  Key_F6,
  Key_F7,
  Key_F8,
  Key_F9,
  Key_F10,
  Key_F11,
  Key_F12,
  Key_Space,
  Key_Control,
  Key_LeftControl,
  Key_RightControl,
  Key_Shift,
  Key_LeftShift,
  Key_RightShift,
  Key_CapsLock,
  Key_Tab,
  Key_Alt,
  Key_Up,
  Key_Down,
  Key_Left,
  Key_Right,
  Key_Return,
  Key_Delete,
  Key_Escape,
  Key_Count,
};

enum MouseButton {
  MouseButton_Left,
  MouseButton_Right,
  MouseButton_Middle,
  MouseButton_X1,
  MouseButton_X2,
  MouseButton_Count,
};

#endif // CDEFAULT_RENDER_H_

#ifdef CDEFAULT_RENDER_IMPLEMENTATION
#undef CDEFAULT_RENDER_IMPLEMENTATION

#if defined(OS_WINDOWS) || defined(OS_LINUX)
#  if defined(OS_WINDOWS)
#    include <windows.h>
#  endif
#  include <GL/gl.h>
#  include <GL/glu.h>
#elif defined(OS_MAC)
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#endif

// TODO: expect this to move around with multi platform support?
#define GL_FRAMEBUFFER_SRGB         0x8DB9
#define GL_CLAMP_TO_EDGE            0x812F
#define GL_COMPILE_STATUS           0x8B81
#define GL_LINK_STATUS              0x8B82
#define GL_FRAGMENT_SHADER          0x8B30
#define GL_VERTEX_SHADER            0x8B31
#define GL_ARRAY_BUFFER             0x8892
#define GL_ELEMENT_ARRAY_BUFFER     0x8893
#define GL_FRAMEBUFFER              0x8D40
#define GL_RENDERBUFFER             0x8D41
#define GL_DEPTH_STENCIL_ATTACHMENT 0x821A
#define GL_FRAMEBUFFER_COMPLETE     0x8CD5
#define GL_COLOR_ATTACHMENT0        0x8CE0
#define GL_DEPTH24_STENCIL8         0x88F0
#define GL_STATIC_DRAW              0x88E4
#define GL_FRAMEBUFFER_SRGB         0x8DB9

typedef char      GLchar;
typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;

typedef void   glShaderSource_Fn(GLuint, GLsizei, GLchar**, GLint*);
typedef void   glCompileShader_Fn(GLuint);
typedef GLuint glCreateProgram_Fn(void);
typedef GLuint glCreateShader_Fn(GLenum);
typedef void   glGetShaderiv_Fn(GLuint, GLenum, GLint*);
typedef void   glAttachShader_Fn(GLuint, GLuint);
typedef void   glLinkProgram_Fn(GLuint);
typedef void   glGetProgramiv_Fn(GLuint, GLenum, GLint*);
typedef void   glUseProgram_Fn(GLuint);
typedef void   glDeleteShader_Fn(GLuint);
typedef void   glEnableVertexAttribArray_Fn(GLuint);
typedef void   glVertexAttribPointer_Fn(GLuint, GLint, GLenum, GLboolean, GLsizei, const void*);
typedef void   glGenVertexArrays_Fn(GLsizei, GLuint*);
typedef void   glGenBuffers_Fn(GLsizei, GLuint*);
typedef void   glBindBuffer_Fn(GLenum, GLuint);
typedef void   glBufferData_Fn(GLenum, GLsizeiptr, const void*, GLenum);
typedef void   glBindVertexArray_Fn(GLuint);
typedef GLint  glGetUniformLocation_Fn(GLuint, const GLchar*);
typedef void   glUniformMatrix4fv_Fn(GLint, GLsizei, GLboolean, const GLfloat*);
typedef void   glUniform3fv_Fn(GLint, GLsizei, const GLfloat*);
typedef void   glUniform2fv_Fn(GLint, GLsizei, const GLfloat*);
typedef void   glUniform1f_Fn(GLint, GLfloat);
typedef void   glScissor_Fn(GLint, GLint, GLsizei, GLsizei);
typedef void   glViewport_Fn(GLint, GLint, GLsizei, GLsizei);
typedef void   glDeleteBuffers_Fn(GLsizei, const GLuint*);
typedef void   glDeleteVertexArrays_Fn(GLsizei, const GLuint*);
typedef void   glTexImage2D_Fn(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void*);
typedef void   glGenTextures_Fn(GLsizei, GLuint*);
typedef void   glBindTexture_Fn(GLenum, GLuint);
typedef void   glTexParameteri_Fn(GLenum, GLenum, GLint);
typedef void   glDeleteTextures_Fn(GLsizei, const GLuint*);
typedef void   glEnable_Fn(GLenum);
typedef void   glDisable_Fn(GLenum);
typedef void   glCullFace_Fn(GLenum);
typedef void   glFrontFace_Fn(GLenum);
typedef void   glClearColor_Fn(GLfloat, GLfloat, GLfloat, GLfloat);
typedef void   glBlendFunc_Fn(GLenum, GLenum);
typedef void   glClear_Fn(GLenum);
typedef void   glDrawArrays_Fn(GLenum, GLint, GLsizei);

// NOTE: platform agnostic open gl api.
typedef struct OpenGLAPI OpenGLAPI;
struct OpenGLAPI {
  glShaderSource_Fn*            glShaderSource;
  glCompileShader_Fn*           glCompileShader;
  glCreateProgram_Fn*           glCreateProgram;
  glCreateShader_Fn*            glCreateShader;
  glGetShaderiv_Fn*             glGetShaderiv;
  glAttachShader_Fn*            glAttachShader;
  glLinkProgram_Fn*             glLinkProgram;
  glGetProgramiv_Fn*            glGetProgramiv;
  glUseProgram_Fn*              glUseProgram;
  glDeleteShader_Fn*            glDeleteShader;
  glVertexAttribPointer_Fn*     glVertexAttribPointer;
  glEnableVertexAttribArray_Fn* glEnableVertexAttribArray;
  glGenVertexArrays_Fn*         glGenVertexArrays;
  glGenBuffers_Fn*              glGenBuffers;
  glBindBuffer_Fn*              glBindBuffer;
  glBufferData_Fn*              glBufferData;
  glBindVertexArray_Fn*         glBindVertexArray;
  glGetUniformLocation_Fn*      glGetUniformLocation;
  glUniformMatrix4fv_Fn*        glUniformMatrix4fv;
  glUniform3fv_Fn*              glUniform3fv;
  glUniform2fv_Fn*              glUniform2fv;
  glUniform1f_Fn*               glUniform1f;
  glScissor_Fn*                 glScissor;
  glViewport_Fn*                glViewport;
  glDeleteBuffers_Fn*           glDeleteBuffers;
  glDeleteVertexArrays_Fn*      glDeleteVertexArrays;
  glTexImage2D_Fn*              glTexImage2D;
  glGenTextures_Fn*             glGenTextures;
  glBindTexture_Fn*             glBindTexture;
  glTexParameteri_Fn*           glTexParameteri;
  glDeleteTextures_Fn*          glDeleteTextures;
  glEnable_Fn*                  glEnable;
  glDisable_Fn*                 glDisable;
  glCullFace_Fn*                glCullFace;
  glFrontFace_Fn*               glFrontFace;
  glClearColor_Fn*              glClearColor;
  glBlendFunc_Fn*               glBlendFunc;
  glClear_Fn*                   glClear;
  glDrawArrays_Fn*              glDrawArrays;
};
static OpenGLAPI _ogl;

typedef struct Renderer Renderer;
struct Renderer {
  GLuint quad_vbo;
  GLuint quad_vao;

  GLuint rect_shader;
  GLint rect_camera_uniform;
  GLint rect_color_uniform;
  GLint rect_size_uniform;
  GLint rect_radius_uniform;

  GLuint frame_shader;
  GLint frame_camera_uniform;
  GLint frame_color_uniform;
  GLint frame_size_uniform;
  GLint frame_border_size_uniform;
  GLint frame_radius_uniform;

  GLuint tri_shader;
  GLint tri_camera_uniform;
  GLint tri_color_uniform;

  GLuint image_shader;
  GLint image_camera_uniform;

  M4 world_to_camera;
};
static Renderer _renderer;

static B32 RendererCompileShader(GLuint* shader, char* vertex_shader_source, char* fragment_shader_source) {
  OpenGLAPI* g = &_ogl;
  GLuint vertex_shader = 0;
  GLuint fragment_shader = 0;
  GLint result;
  B32 success = false;

  vertex_shader = g->glCreateShader(GL_VERTEX_SHADER);
  g->glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
  g->glCompileShader(vertex_shader);
  g->glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &result);
  if (!result) {
    LOG_ERROR("[RENDER] Failed to compile vertex shader."); // TODO: print ogl error message.
    goto renderer_compile_shader_exit;
  }

  fragment_shader = g->glCreateShader(GL_FRAGMENT_SHADER);
  g->glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
  g->glCompileShader(fragment_shader);
  g->glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &result);
  if (!result) {
    LOG_ERROR("[RENDER] Failed to compile fragment shader.");
    goto renderer_compile_shader_exit;
  }

  *shader = g->glCreateProgram();
  g->glAttachShader(*shader, vertex_shader);
  g->glAttachShader(*shader, fragment_shader);
  g->glLinkProgram(*shader);
  g->glGetShaderiv(*shader, GL_LINK_STATUS, &result);
  if (!result) {
    LOG_ERROR("[RENDER] Failed to link shader program.");
    goto renderer_compile_shader_exit;
  }

  success = true;

renderer_compile_shader_exit:
  if (vertex_shader != 0)   { g->glDeleteShader(vertex_shader);   }
  if (fragment_shader != 0) { g->glDeleteShader(fragment_shader); }
  return success;
}

// NOTE: Expected to be called immediately after the plat window is initialized.
static B32 RendererInit(void) {
  Renderer* r = &_renderer;
  OpenGLAPI* g = &_ogl;
  B32 success = false; // TODO: error checking.

  LOG_INFO("[RENDER] Initializing OpenGL renderer.");

  // TODO: verify existence.
  g->glEnable(GL_FRAMEBUFFER_SRGB);
  g->glEnable(GL_CULL_FACE);
  g->glEnable(GL_TEXTURE_2D);
  g->glEnable(GL_BLEND);
  g->glCullFace(GL_BACK);
  g->glFrontFace(GL_CCW);
  g->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  char* rect_vertex_shader_source =
    "#version 330 core\n"
    "uniform mat4 to_camera_transform;\n"
    "layout (location = 0) in vec2 in_pos;\n"
    "layout (location = 1) in vec2 in_tex_coord;\n"
    "out vec2 tex_coord;\n"
    "void main() {\n"
    "  tex_coord = in_tex_coord;\n"
    "  gl_Position = to_camera_transform * vec4(in_pos, 0.0, 1.0);\n"
    "}\0";
  char* rect_fragment_shader_source =
    "#version 330 core\n"
    "uniform vec3 color;\n"
    "uniform vec2 size;\n"
    "uniform float radius;\n"
    "in vec2 tex_coord;\n"
    "out vec4 frag_color;\n"
    "void main() { \n"
    "  vec2 centered_pos = (tex_coord - 0.5) * size;\n"
    "  vec2 half_size = size * 0.5;\n"
    "  float dist = length(max(abs(centered_pos) - half_size + vec2(radius), 0.0)) - radius;\n"
    "  float aa = 1.0;\n"
    "  float alpha = smoothstep(aa, 0.0, dist);\n"
    "  frag_color = vec4(color, alpha);\n"
    "}\0";
  DEBUG_ASSERT(RendererCompileShader(&r->rect_shader, rect_vertex_shader_source, rect_fragment_shader_source));
  r->rect_camera_uniform = g->glGetUniformLocation(r->rect_shader, "to_camera_transform");
  r->rect_color_uniform = g->glGetUniformLocation(r->rect_shader, "color");
  r->rect_size_uniform = g->glGetUniformLocation(r->rect_shader, "size");
  r->rect_radius_uniform = g->glGetUniformLocation(r->rect_shader, "radius");

  char* frame_vertex_shader_source =
    "#version 330 core\n"
    "uniform mat4 to_camera_transform;\n"
    "layout (location = 0) in vec2 in_pos;\n"
    "layout (location = 1) in vec2 in_tex_coord;\n"
    "out vec2 tex_coord;\n"
    "void main() {\n"
    "  tex_coord = in_tex_coord;\n"
    "  gl_Position = to_camera_transform * vec4(in_pos, 0.0, 1.0);\n"
    "}\0";
  char* frame_fragment_shader_source =
    "#version 330 core\n"
    "uniform vec3 color;\n"
    "uniform vec2 size;\n"
    "uniform float border_size;\n"
    "uniform float radius;\n"
    "in vec2 tex_coord;\n"
    "out vec4 frag_color;\n"
    "void main() { \n"
    "  vec2 centered_pos = (tex_coord - 0.5) * size;\n"
    "  vec2 half_size_outer = size * 0.5;\n"
    "  vec2 half_size_inner = max(half_size_outer - vec2(border_size), vec2(0.0));\n"
    "  float r_outer = clamp(radius, 0.0, min(half_size_outer.x, half_size_outer.y));\n"
    "  float r_inner = r_outer * min(half_size_inner.x / half_size_outer.x, half_size_inner.y / half_size_outer.y);\n"
    "  float dist_outer = length(max(abs(centered_pos) - half_size_outer + vec2(r_outer), 0.0)) - r_outer;\n"
    "  float dist_inner = length(max(abs(centered_pos) - half_size_inner + vec2(r_inner), 0.0)) - r_inner;\n"
    "  float aa = 1.0;\n"
    "  float alpha_outer = smoothstep(aa, 0.0, dist_outer);\n"
    "  float alpha_inner = smoothstep(aa, 0.0, dist_inner);\n"
    "  float alpha = clamp(alpha_outer - alpha_inner, 0.0, 1.0);\n"
    "  frag_color = vec4(color, alpha);\n"
    "}\0";
  DEBUG_ASSERT(RendererCompileShader(&r->frame_shader, frame_vertex_shader_source, frame_fragment_shader_source));
  r->frame_camera_uniform = g->glGetUniformLocation(r->frame_shader, "to_camera_transform");
  r->frame_color_uniform = g->glGetUniformLocation(r->frame_shader, "color");
  r->frame_size_uniform = g->glGetUniformLocation(r->frame_shader, "size");
  r->frame_border_size_uniform = g->glGetUniformLocation(r->frame_shader, "border_size");
  r->frame_radius_uniform = g->glGetUniformLocation(r->frame_shader, "radius");

  char* tri_vertex_shader_source =
    "#version 330 core\n"
    "uniform mat4 to_camera_transform;\n"
    "layout (location = 0) in vec2 in_pos;\n"
    "void main() {\n"
    "  gl_Position = to_camera_transform * vec4(in_pos, 0.0, 1.0);\n"
    "}\0";
  char* tri_fragment_shader_source =
    "#version 330 core\n"
    "uniform vec3 color;\n"
    "out vec4 frag_color;\n"
    "void main() { \n"
    "  frag_color = vec4(color, 1);\n"
    "}\0";
  DEBUG_ASSERT(RendererCompileShader(&r->tri_shader, tri_vertex_shader_source, tri_fragment_shader_source));
  r->tri_camera_uniform = g->glGetUniformLocation(r->tri_shader, "to_camera_transform");
  r->tri_color_uniform = g->glGetUniformLocation(r->tri_shader, "color");

  char* image_vertex_shader_source =
    "#version 330 core\n"
    "uniform mat4 to_camera_transform;\n"
    "layout (location = 0) in vec2 in_pos;\n"
    "layout (location = 1) in vec2 in_tex_coord;\n"
    "out vec2 tex_coord;\n"
    "void main() {\n"
    "  tex_coord = in_tex_coord;\n"
    "  gl_Position = to_camera_transform * vec4(in_pos, 0.0, 1.0);\n"
    "}\0";
  char* image_fragment_shader_source =
    "#version 330 core\n"
    "uniform sampler2D texture_image;\n"
    "in vec2 tex_coord;\n"
    "out vec4 frag_color;\n"
    "void main() { \n"
    "  frag_color = texture(texture_image, tex_coord);\n"
    "}\0";
  DEBUG_ASSERT(RendererCompileShader(&r->image_shader, image_vertex_shader_source, image_fragment_shader_source));
  r->image_camera_uniform = g->glGetUniformLocation(r->image_shader, "to_camera_transform");

  F32 quad_vertices[6][4] = {
    {-0.5f, +0.5f, 0, 0 },
    {-0.5f, -0.5f, 0, 1 },
    {+0.5f, -0.5f, 1, 1 },
    {-0.5f, +0.5f, 0, 0 },
    {+0.5f, -0.5f, 1, 1 },
    {+0.5f, +0.5f, 1, 0 },
  };
  g->glGenBuffers(1, &r->quad_vbo);
  g->glBindBuffer(GL_ARRAY_BUFFER, r->quad_vbo);
  g->glBufferData(GL_ARRAY_BUFFER, sizeof(F32) * 6 * 4, quad_vertices, GL_STATIC_DRAW);
  g->glGenVertexArrays(1, &r->quad_vao);
  g->glBindVertexArray(r->quad_vao);
  g->glEnableVertexAttribArray(0);
  g->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(F32) * 4, (void*)(sizeof(F32) * 0)); // points
  g->glEnableVertexAttribArray(1);
  g->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(F32) * 4, (void*)(sizeof(F32) * 2)); // uvs
  g->glBindVertexArray(0);
  g->glBindBuffer(GL_ARRAY_BUFFER, 0);

  M4 projection;
  S32 width, height;
  WindowGetDims(NULL, NULL, &width, &height);
  M4Orthographic(&projection, 0, width, 0, height, 0.01f, 100.0f);
  RendererSetProjection(projection);

  LOG_INFO("[RENDER] OpenGL renderer initialized.");
  success = true;

  return success;
}

void RendererSetProjection(M4 projection) {
  Renderer* r = &_renderer;
  V3 pos    = { 0, 0, 1 }; // NOTE: seat camera Z back so items can exist at z = 0.
  V3 target = V3_Z_NEG;
  V3 up     = V3_Y_POS;
  M4 camera;
  M4LookAt(&camera, &pos, &target, &up);
  M4MultM4(&r->world_to_camera, &projection, &camera);
}

void RendererRegisterImage(U32* image_handle, U8* image_bytes, U32 width, U32 height) {
  OpenGLAPI* g = &_ogl;
  g->glGenTextures(1, image_handle);
  g->glBindTexture(GL_TEXTURE_2D, *image_handle);
  g->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_bytes);
  g->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  g->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  g->glBindTexture(GL_TEXTURE_2D, 0);
}

void RendererReleaseImage(U32 image_handle) {
  OpenGLAPI* g = &_ogl;
  g->glDeleteTextures(1, &image_handle);
}

void RendererEnableScissorTest(S32 x, S32 y, S32 width, S32 height) {
  OpenGLAPI* g = &_ogl;
  g->glEnable(GL_SCISSOR_TEST);
  g->glScissor(x, y, width, height);
}

void RendererDisableScissorTest(void) {
  OpenGLAPI* g = &_ogl;
  g->glDisable(GL_SCISSOR_TEST);
}

void RendererEnableDepthTest(void) {
  OpenGLAPI* g = &_ogl;
  g->glEnable(GL_DEPTH_TEST);
}

void RendererDisableDepthTest(void) {
  OpenGLAPI* g = &_ogl;
  g->glDisable(GL_DEPTH_TEST);
}

void RendererSetClearColor(F32 red, F32 green, F32 blue, F32 alpha) {
  OpenGLAPI* g = &_ogl;
  g->glClearColor(red, green, blue, alpha);
}

void RendererSetClearColorV(V4 color) {
  RendererSetClearColor(color.r, color.g, color.b, color.a);
}

static V3 UnprojectPoint(V3 p, M4* inv_vp) {
  V4 v = { p.x, p.y, p.z, 1.0f };
  V4 v2;
  M4MultV4(&v2, inv_vp, &v);
  V4DivF32(&v2, &v2, v2.w);
  return (V3) { v2.x, v2.y, v2.z };
}

void RendererCastRay(F32 x, F32 y, V2* intersect) {
  V3 start;
  RendererCastRay3(x, y, &start, NULL);
  intersect->x = start.x;
  intersect->y = start.y;
}

void RendererCastRayV(V2 pos, V2* intersect) {
  RendererCastRay(pos.x, pos.y, intersect);
}

void RendererCastRay3(F32 x, F32 y, V3* start, V3* dir) {
  Renderer* r = &_renderer;

  S32 width, height;
  WindowGetDims(NULL, NULL, &width, &height);
  F32 x2 = (x / (((F32) width) / 2.0f)) - 1.0f;
  F32 y2 = (y / (((F32) height) / 2.0f)) - 1.0f;
  x2 = CLAMP(-1, x2, 1);
  y2 = CLAMP(-1, y2, 1);

  M4 world_to_camera_inv;
  M4Invert(&world_to_camera_inv, &r->world_to_camera);
  V3 n = UnprojectPoint((V3) { x2, y2, -1}, &world_to_camera_inv);
  V3 f = UnprojectPoint((V3) { x2, y2, +1}, &world_to_camera_inv);
  V3 delta;
  V3SubV3(&delta, &f, &n);
  V3Normalize(&delta, &delta);

  if (start != NULL) { *start = n; }
  if (dir != NULL) { *dir = delta; }
}

void RendererCastRay3V(V2 pos, V3* start, V3* dir) {
  RendererCastRay3(pos.x, pos.y, start, dir);
}

void DrawLine(F32 start_x, F32 start_y, F32 end_x, F32 end_y, F32 thickness, F32 red, F32 green, F32 blue) {
  V2 start = { start_x, start_y };
  V2 end   = { end_x, end_y };
  V2 delta;
  V2SubV2(&delta, &end, &start);
  F32 theta = F32ArcTan2(delta.y, delta.x);
  F32 width = V2Length(&delta);
  DrawRoundedRectangleRot(start.x + delta.x / 2, start.y + delta.y / 2, width, thickness, thickness / 2.0f, theta, red, green, blue);
}

void DrawLineV(V2 start, V2 end, F32 thickness, V3 color) {
  DrawLine(start.x, start.y, end.x, end.y, thickness, color.r, color.g, color.b);
}

void DrawRectangle(F32 center_x, F32 center_y, F32 width, F32 height, F32 red, F32 green, F32 blue) {
  DrawRoundedRectangle(center_x, center_y, width, height, 0.0f, red, green, blue);
}

void DrawRectangleV(V2 center, V2 size, V3 color) {
  DrawRectangle(center.x, center.y, size.x, size.y, color.r, color.g, color.b);
}

void DrawRectangleFrame(F32 center_x, F32 center_y, F32 width, F32 height, F32 border, F32 red, F32 green, F32 blue) {
  DrawRectangleFrameRot(center_x, center_y, width, height, border, 0, red, green, blue);
}

void DrawRectangleFrameV(V2 center, V2 size, F32 border, V3 color) {
  DrawRectangleFrame(center.x, center.y, size.x, size.y, border, color.r, color.g, color.b);
}

void DrawRectangleRot(F32 center_x, F32 center_y, F32 width, F32 height, F32 angle_rad, F32 red, F32 green, F32 blue) {
  DrawRoundedRectangleRot(center_x, center_y, width, height, 0.0f, angle_rad, red, green, blue);
}

void DrawRectangleRotV(V2 center, V2 size, F32 angle_rad, V3 color) {
  DrawRectangleRot(center.x, center.y, size.x, size.y, angle_rad, color.r, color.g, color.b);
}

void DrawRoundedRectangle(F32 center_x, F32 center_y, F32 width, F32 height, F32 radius, F32 red, F32 green, F32 blue) {
  DrawRoundedRectangleRot(center_x, center_y, width, height, radius, 0.0f, red, green, blue);
}

void DrawRoundedRectangleV(V2 center, V2 size, F32 radius, V3 color) {
  DrawRoundedRectangle(center.x, center.y, size.x, size.y, radius, color.r, color.g, color.b);
}

void DrawRoundedRectangleFrame(F32 center_x, F32 center_y, F32 width, F32 height, F32 radius, F32 border, F32 red, F32 green, F32 blue) {
  DrawRoundedRectangleFrameRot(center_x, center_y, width, height, radius, border, 0, red, green, blue);
}

void DrawRoundedRectangleFrameV(V2 center, V2 size, F32 radius, F32 border, V3 color) {
  DrawRoundedRectangleFrame(center.x, center.y, size.x, size.y, radius, border, color.r, color.g, color.b);
}

void DrawRoundedRectangleRot(F32 center_x, F32 center_y, F32 width, F32 height, F32 radius, F32 angle_rad, F32 red, F32 green, F32 blue) {
  Renderer* r = &_renderer;
  OpenGLAPI* g = &_ogl;

  V3 color = { red, green, blue };
  V3 pos   = { center_x, center_y, 0 };
  V3 scale = { width, height, 1 };
  V4 rot;
  V4RotateAroundAxis(&rot, &V3_Z_POS, angle_rad);
  M4 rect_to_world, rect_to_camera, rect_to_camera_t;
  M4FromTransform(&rect_to_world, &pos, &rot, &scale);
  M4MultM4(&rect_to_camera, &r->world_to_camera, &rect_to_world);
  M4Transpose(&rect_to_camera_t, &rect_to_camera);

  g->glUseProgram(r->rect_shader);
  g->glUniformMatrix4fv(r->rect_camera_uniform, 1, GL_FALSE, (GLfloat*) &rect_to_camera_t);
  g->glUniform3fv(r->rect_color_uniform, 1, (GLfloat*) &color);
  g->glUniform2fv(r->rect_size_uniform, 1, (GLfloat*) &scale);
  g->glUniform1f(r->rect_radius_uniform, radius);
  g->glBindVertexArray(r->quad_vao);
  g->glDrawArrays(GL_TRIANGLES, 0, 6);
  g->glBindVertexArray(0);
  g->glUseProgram(0);
}

void DrawRoundedRectangleRotV(V2 center, V2 size, F32 radius, F32 angle, V3 color) {
  DrawRoundedRectangleRot(center.x, center.y, size.x, size.y, radius, angle, color.r, color.g, color.b);
}

void DrawRectangleFrameRot(F32 center_x, F32 center_y, F32 width, F32 height, F32 border, F32 angle_rad, F32 red, F32 green, F32 blue) {
  DrawRoundedRectangleFrameRot(center_x, center_y, width, height, 0, border, angle_rad, red, green, blue);
}

void DrawRectangleFrameRotV(V2 center, V2 size, F32 border, F32 angle_rad,  V3 color) {
  DrawRectangleFrameRot(center.x, center.y, size.x, size.y, border, angle_rad, color.r, color.g, color.b);
}

void DrawRoundedRectangleFrameRot(F32 center_x, F32 center_y, F32 width, F32 height, F32 radius, F32 border, F32 angle_rad, F32 red, F32 green, F32 blue) {
  Renderer* r = &_renderer;
  OpenGLAPI* g = &_ogl;

  V3 color = { red, green, blue };
  V3 pos   = { center_x, center_y, 0 };
  V3 scale = { width, height, 1 };
  V4 rot;
  V4RotateAroundAxis(&rot, &V3_Z_POS, angle_rad);
  M4 frame_to_world, frame_to_camera, frame_to_camera_t;
  M4FromTransform(&frame_to_world, &pos, &rot, &scale);
  M4MultM4(&frame_to_camera, &r->world_to_camera, &frame_to_world);
  M4Transpose(&frame_to_camera_t, &frame_to_camera);

  g->glUseProgram(r->frame_shader);
  g->glUniformMatrix4fv(r->frame_camera_uniform, 1, GL_FALSE, (GLfloat*) &frame_to_camera_t);
  g->glUniform3fv(r->frame_color_uniform, 1, (GLfloat*) &color);
  g->glUniform2fv(r->frame_size_uniform, 1, (GLfloat*) &scale);
  g->glUniform1f(r->frame_border_size_uniform, border);
  g->glUniform1f(r->frame_radius_uniform, radius);
  g->glBindVertexArray(r->quad_vao);
  g->glDrawArrays(GL_TRIANGLES, 0, 6);
  g->glBindVertexArray(0);
  g->glUseProgram(0);
}

void DrawRoundedRectangleFrameRotV(V2 center, V2 size, F32 radius, F32 border, F32 angle_rad, V3 color) {
  DrawRoundedRectangleFrameRot(center.x, center.y, size.x, size.y, radius, border, angle_rad, color.r, color.g, color.b);
}

void DrawCircle(F32 center_x, F32 center_y, F32 radius, F32 red, F32 green, F32 blue) {
  DrawRoundedRectangle(center_x, center_y, radius * 2, radius * 2, radius, red, green, blue);
}

void DrawCircleV(V2 center, F32 radius, V3 color) {
  DrawCircle(center.x, center.y, radius, color.r, color.g, color.b);
}

void DrawRing(F32 center_x, F32 center_y, F32 radius, F32 border, F32 red, F32 green, F32 blue) {
  DrawRoundedRectangleFrame(center_x, center_y, radius * 2, radius * 2, radius, border, red, green, blue);
}

void DrawRingV(V2 center, F32 radius, F32 border, V3 color) {
  DrawRing(center.x, center.y, radius, border, color.r, color.g, color.b);
}

void DrawTriangle(F32 x1, F32 y1, F32 x2, F32 y2, F32 x3, F32 y3, F32 red, F32 green, F32 blue) {
  // TODO: i assume there's a better way to do this without resorting to compatibility mode?
  Renderer* r = &_renderer;
  OpenGLAPI* g = &_ogl;

  F32 x_min = MIN(x1, MIN(x2, x3));
  F32 x_max = MAX(x1, MAX(x2, x3));
  F32 y_min = MIN(y1, MIN(y2, y3));
  F32 y_max = MAX(y1, MAX(y2, y3));
  F32 x_scale = x_max - x_min;
  F32 y_scale = y_max - y_min;
  F32 tri_vertices[3][2] = {
    { ((x1 - x_min) / x_scale) - 0.5f, ((y1 - y_min) / y_scale) - 0.5f },
    { ((x2 - x_min) / x_scale) - 0.5f, ((y2 - y_min) / y_scale) - 0.5f },
    { ((x3 - x_min) / x_scale) - 0.5f, ((y3 - y_min) / y_scale) - 0.5f },
  };
  GLuint tri_vbo, tri_vao;
  g->glGenBuffers(1, &tri_vbo);
  g->glBindBuffer(GL_ARRAY_BUFFER, tri_vbo);
  g->glBufferData(GL_ARRAY_BUFFER, sizeof(F32) * 3 * 2, tri_vertices, GL_STATIC_DRAW);
  g->glGenVertexArrays(1, &tri_vao);
  g->glBindVertexArray(tri_vao);
  g->glEnableVertexAttribArray(0);
  g->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(F32) * 2, (void*)(sizeof(F32) * 0));

  V3 color = { red, green, blue };
  V3 pos = { (x_min + x_max) / 2, (y_min + y_max) / 2.0f, 0 };
  V3 scale = { x_scale, y_scale, 1 };
  V4 rot = V4_QUAT_IDENT;
  M4 tri_to_world, tri_to_camera, tri_to_camera_t;
  M4FromTransform(&tri_to_world, &pos, &rot, &scale);
  M4MultM4(&tri_to_camera, &r->world_to_camera, &tri_to_world);
  M4Transpose(&tri_to_camera_t, &tri_to_camera);

  g->glUseProgram(r->tri_shader);
  g->glUniformMatrix4fv(r->tri_camera_uniform, 1, GL_FALSE, (GLfloat*) &tri_to_camera_t);
  g->glUniform3fv(r->tri_color_uniform, 1, (GLfloat*) &color);
  g->glDrawArrays(GL_TRIANGLES, 0, 3);
  g->glBindVertexArray(0);
  g->glUseProgram(0);

  g->glDeleteBuffers(1, &tri_vbo);
  g->glDeleteVertexArrays(1, &tri_vao);
}

void DrawTriangleV(V2 p1, V2 p2, V2 p3, V3 color) {
  DrawTriangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, color.r, color.g, color.b);
}

// SPEEDUP: this is slow because drawing triangles is slow.
void DrawConvexHullV(V2* points, U32 points_len, V3 color) {
  V2 p0 = points[0];
  for (U32 i = 1; i < points_len; i++) {
    V2 p1 = points[i];
    V2 p2 = points[(i + 1) % points_len];
    DrawTriangleV(p0, p1, p2, color);
  }
}

void DrawImage(U32 image_handle, F32 center_x, F32 center_y, F32 width, F32 height) {
  DrawImageRot(image_handle, center_x, center_y, width, height, 0.0f);
}

void DrawImageV(U32 image_handle, V2 pos, V2 size) {
  DrawImage(image_handle, pos.x, pos.y, size.x, size.y);
}

void DrawImageRot(U32 image_handle, F32 center_x, F32 center_y, F32 width, F32 height, F32 angle_rad) {
  Renderer* r = &_renderer;
  OpenGLAPI* g = &_ogl;

  V3 pos   = { center_x, center_y, 0 };
  V3 scale = { width, height, 1 };
  V4 rot;
  V4RotateAroundAxis(&rot, &V3_Z_POS, angle_rad);
  M4 image_to_world, image_to_camera, image_to_camera_t;
  M4FromTransform(&image_to_world, &pos, &rot, &scale);
  M4MultM4(&image_to_camera, &r->world_to_camera, &image_to_world);
  M4Transpose(&image_to_camera_t, &image_to_camera);

  g->glUseProgram(r->image_shader);
  g->glUniformMatrix4fv(r->image_camera_uniform, 1, GL_FALSE, (GLfloat*) &image_to_camera_t);
  g->glBindTexture(GL_TEXTURE_2D, image_handle);
  g->glBindVertexArray(r->quad_vao);
  g->glDrawArrays(GL_TRIANGLES, 0, 6);
  g->glBindTexture(GL_TEXTURE_2D, 0);
  g->glBindVertexArray(0);
  g->glUseProgram(0);
}

void DrawImageRotV(U32 image_handle, V2 pos, V2 size, F32 angle_rad) {
  DrawImageRot(image_handle, pos.x, pos.y, size.x, size.y, angle_rad);
}

#if defined(OS_WINDOWS)
#define CDEFAULT_RENDER_BACKEND_NAMESPACE WIN_
// NOTE: need to link with user32.lib opengl32.lib gdi32.lib

#define WGL_CONTEXT_MAJOR_VERSION_ARB             0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB             0x2092
#define WGL_CONTEXT_FLAGS_ARB                     0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB              0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

// NOTE: don't allow window resizing.
#define WINDOW_STYLE WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME

typedef HGLRC WINAPI wglCreateContextAttribsARB_Fn(HDC, HGLRC, const int*);

#define WIN_KEY_JUST_PRESSED  0
#define WIN_KEY_JUST_RELEASED 1
#define WIN_KEY_PRESSED       2

typedef struct WIN_Window WIN_Window;
struct WIN_Window {
  B8 initialized;
  B8 key_states[Key_Count];
  B8 mouse_states[MouseButton_Count];
  F32 m_scroll_sign;
  V2 mouse_pos;
  V2 mouse_pos_delta;
  B8 should_close;
  HWND handle;
  HDC device_context;
  HGLRC ogl_context;
  S32 width;
  S32 height;
};
static WIN_Window _win_window;

// TODO: instead of printing GetLastError(), do some formatting using FormatMessage?

// TODO: custom on close callback?
static LRESULT CALLBACK WIN_WindowCallback(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam) {
  WIN_Window* window = &_win_window;
  OpenGLAPI* g = &_ogl;
  switch (umsg) {
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP:
    case WM_MOUSEWHEEL: {
      // NOTE: should be handled by the event loop instead.
      UNREACHABLE();
      return 0;
    } break;
    case WM_SIZE: {
      S32 width  = (S32) LOWORD(lparam);
      S32 height = (S32) HIWORD(lparam);
      if (width != window->width || height != window->height) {
        if (g->glViewport != NULL) {
          // NOTE: only update the cached width / height on explicit resizes.
          // this may be called e.g. when toggling fullscreen, where we want to
          // maintain the existing viewport size.
          //
          // the current implementation basically assumes that the screen size
          // will be some multiple of the resolution, tracked through the renderer's
          // perspective matrix. maybe we will want to support divergence (e.g.
          // adding vertical black bars kind of thing) at some point.
          g->glViewport(0, 0, width, height);
        }
      }
      return DefWindowProc(hwnd, umsg, wparam, lparam);
    } break;
    case WM_CLOSE: {
      window->should_close = true;
      return DefWindowProc(hwnd, umsg, wparam, lparam);
    } break;
    default: {
      return DefWindowProc(hwnd, umsg, wparam, lparam);
    } break;
  }
}

B32 WIN_WindowInit(S32 width, S32 height, char* title) {
  WIN_Window* window = &_win_window;
  DEBUG_ASSERT(!window->initialized);

  LOG_INFO("[RENDER] Initializing window.");

  if (width == 0)    { width = CW_USEDEFAULT;  }
  if (height == 0)   { height = CW_USEDEFAULT; }
  if (title == NULL) { title = "";             }

  B32 success = false;

  HINSTANCE hinstance = GetModuleHandle(NULL);

  WNDCLASS window_class;
  MEMORY_ZERO_STRUCT(&window_class);
  window_class.lpfnWndProc = WIN_WindowCallback;
  window_class.hInstance = hinstance;
  window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
  window_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  window_class.lpszClassName = "CDEFAULT_WINDOW_CLASS";
  RegisterClass(&window_class);

  DWORD create_window_flags = 0;
  create_window_flags |= WINDOW_STYLE;
  create_window_flags |= WS_VISIBLE;

  HWND handle = CreateWindowExA(
      0, window_class.lpszClassName, title, create_window_flags,
      CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, hinstance, NULL);
  if (handle == NULL) {
    LOG_ERROR("[RENDER] Failed to create window: %d", GetLastError());
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
    LOG_ERROR("[RENDER] Failed to choose a pixel format: %d", GetLastError());
    goto win_window_create_exit;
  }
  PIXELFORMATDESCRIPTOR suggested_pixel_format_desc;
  if (!DescribePixelFormat(device_context, suggested_pixel_format_idx, sizeof(suggested_pixel_format_desc), &suggested_pixel_format_desc)) {
    LOG_ERROR("[RENDER] Failed to describe pixel format: %d", GetLastError());
    goto win_window_create_exit;
  }
  if (!SetPixelFormat(device_context, suggested_pixel_format_idx, &suggested_pixel_format_desc)) {
    LOG_ERROR("[RENDER] Failed to set pixel format: %d", GetLastError());
    goto win_window_create_exit;
  }

  // NOTE: make fake ogl context first.
  HGLRC ogl_context = wglCreateContext(device_context);
  if (ogl_context == NULL) {
    LOG_ERROR("[RENDER] Failed to create OpenGL context: %d", GetLastError());
    goto win_window_create_exit;
  }
  if (wglMakeCurrent(device_context, ogl_context) == FALSE) {
    LOG_ERROR("[RENDER] Failed to attach the OpenGL context: %d", GetLastError());
    wglDeleteContext(ogl_context);
    goto win_window_create_exit;
  }
  wglCreateContextAttribsARB_Fn* wglCreateContextAttribsARB =
    (wglCreateContextAttribsARB_Fn*) wglGetProcAddress("wglCreateContextAttribsARB");
  if (wglCreateContextAttribsARB == NULL) {
    LOG_ERROR("[RENDER] Failed to find wglCreateContextAttribsARB function pointer.");
    wglDeleteContext(ogl_context);
    goto win_window_create_exit;
  }
  wglDeleteContext(ogl_context);

  // NOTE: make real ogl context.
  const S32 ogl_attr[] = {
    WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
    WGL_CONTEXT_MINOR_VERSION_ARB, 0,
    WGL_CONTEXT_FLAGS_ARB, 0,
    WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB, // WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
    0
  };
  ogl_context = wglCreateContextAttribsARB(device_context, 0, ogl_attr);
  if (!wglMakeCurrent(device_context, ogl_context)) {
    LOG_ERROR("Failed to create OpenGL context: %d", GetLastError());
    goto win_window_create_exit;
  }

#define WIN_LINK_GL(fn) \
    _ogl.fn = (fn##_Fn*) wglGetProcAddress(#fn); \
    DEBUG_ASSERT(_ogl.fn != NULL)

  WIN_LINK_GL(glShaderSource);
  WIN_LINK_GL(glCompileShader);
  WIN_LINK_GL(glCreateProgram);
  WIN_LINK_GL(glCreateShader);
  WIN_LINK_GL(glGetShaderiv);
  WIN_LINK_GL(glAttachShader);
  WIN_LINK_GL(glLinkProgram);
  WIN_LINK_GL(glGetProgramiv);
  WIN_LINK_GL(glUseProgram);
  WIN_LINK_GL(glDeleteShader);
  WIN_LINK_GL(glVertexAttribPointer);
  WIN_LINK_GL(glEnableVertexAttribArray);
  WIN_LINK_GL(glGenVertexArrays);
  WIN_LINK_GL(glGenBuffers);
  WIN_LINK_GL(glBindBuffer);
  WIN_LINK_GL(glBufferData);
  WIN_LINK_GL(glBindVertexArray);
  WIN_LINK_GL(glGetUniformLocation);
  WIN_LINK_GL(glUniformMatrix4fv);
  WIN_LINK_GL(glUniform3fv);
  WIN_LINK_GL(glUniform2fv);
  WIN_LINK_GL(glUniform1f);
  WIN_LINK_GL(glDeleteBuffers);
  WIN_LINK_GL(glDeleteVertexArrays);
  _ogl.glDeleteTextures = glDeleteTextures;
  _ogl.glGenTextures = glGenTextures;
  _ogl.glBindTexture = glBindTexture;
  _ogl.glScissor = glScissor;
  _ogl.glViewport = glViewport;
  _ogl.glTexImage2D = glTexImage2D;
  _ogl.glTexParameteri = glTexParameteri;
  _ogl.glEnable = glEnable;
  _ogl.glDisable = glDisable;
  _ogl.glCullFace = glCullFace;
  _ogl.glFrontFace = glFrontFace;
  _ogl.glClearColor = glClearColor;
  _ogl.glBlendFunc = glBlendFunc;
  _ogl.glClear = glClear;
  _ogl.glDrawArrays = glDrawArrays;

#undef WIN_LINK_GL

  window->width = width;
  window->height = height;
  window->handle = handle;
  window->device_context = device_context;
  window->ogl_context = ogl_context;
  window->initialized = true;

  if (!RendererInit()) {
    LOG_ERROR("[RENDER] Failed to initialize renderer.");
    window->initialized = false;
    goto win_window_create_exit;
  }

  LOG_INFO("[RENDER] Window initialized successfully.");

  success = true;

win_window_create_exit:
  // TODO: close ogl context on failure.
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

static void WIN_KeyUpdate(B8* state, B32 is_pressed) {
  B8 new_state = 0;
  new_state |= (is_pressed && !(*state & BIT(WIN_KEY_PRESSED))) << WIN_KEY_JUST_PRESSED;
  new_state |= (!is_pressed && (*state & BIT(WIN_KEY_PRESSED))) << WIN_KEY_JUST_RELEASED;
  new_state |= is_pressed << WIN_KEY_PRESSED;
  *state = new_state;
}

void WIN_WindowFlushEvents() {
  WIN_Window* window = &_win_window;
  DEBUG_ASSERT(window->initialized);

  // NOTE: update single frame key events.
  for (S32 i = 0; i < Key_Count; i++) {
    window->key_states[i] &= ~(BIT(WIN_KEY_JUST_PRESSED) | BIT(WIN_KEY_JUST_RELEASED));
  }
  for (S32 i = 0; i < MouseButton_Count; i++) {
    window->mouse_states[i] &= ~(BIT(WIN_KEY_JUST_PRESSED) | BIT(WIN_KEY_JUST_RELEASED));
  }

  POINT mouse_point = { 0, 0 };
  if (GetCursorPos(&mouse_point)) {
    if (ScreenToClient(window->handle, &mouse_point)) {
      // TODO: can't use regular height here because that takes the menu bar into account, but this may be slow?
      S32 viewport_height;
      WindowGetDims(NULL, NULL, NULL, &viewport_height);
      mouse_point.y = viewport_height - mouse_point.y;
      window->mouse_pos_delta.x = mouse_point.x - window->mouse_pos.x;
      window->mouse_pos_delta.y = mouse_point.y - window->mouse_pos.y;
      window->mouse_pos.x = mouse_point.x;
      window->mouse_pos.y = mouse_point.y;
    } else {
      LOG_ERROR("Failed to find mouse point relative to the screen.");
    }
  }

  MSG msg;
  while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
    switch (msg.message) {

#define WIN_KEY_UPDATE(x) WIN_KeyUpdate(&window->key_states[x], is_down);

      case WM_KEYDOWN:
      case WM_KEYUP: {
        U32 key_code = (U32) msg.wParam;
        U32 key_state = (U32) msg.lParam;
        B32 is_down = (key_state & (1UL << 31)) == 0;
        switch (key_code) {
          case 'A': { WIN_KEY_UPDATE(Key_A); } break;
          case 'B': { WIN_KEY_UPDATE(Key_B); } break;
          case 'C': { WIN_KEY_UPDATE(Key_C); } break;
          case 'D': { WIN_KEY_UPDATE(Key_D); } break;
          case 'E': { WIN_KEY_UPDATE(Key_E); } break;
          case 'F': { WIN_KEY_UPDATE(Key_F); } break;
          case 'G': { WIN_KEY_UPDATE(Key_G); } break;
          case 'H': { WIN_KEY_UPDATE(Key_H); } break;
          case 'I': { WIN_KEY_UPDATE(Key_I); } break;
          case 'J': { WIN_KEY_UPDATE(Key_J); } break;
          case 'K': { WIN_KEY_UPDATE(Key_K); } break;
          case 'L': { WIN_KEY_UPDATE(Key_L); } break;
          case 'M': { WIN_KEY_UPDATE(Key_M); } break;
          case 'N': { WIN_KEY_UPDATE(Key_N); } break;
          case 'O': { WIN_KEY_UPDATE(Key_O); } break;
          case 'P': { WIN_KEY_UPDATE(Key_P); } break;
          case 'Q': { WIN_KEY_UPDATE(Key_Q); } break;
          case 'R': { WIN_KEY_UPDATE(Key_R); } break;
          case 'S': { WIN_KEY_UPDATE(Key_S); } break;
          case 'T': { WIN_KEY_UPDATE(Key_T); } break;
          case 'U': { WIN_KEY_UPDATE(Key_U); } break;
          case 'V': { WIN_KEY_UPDATE(Key_V); } break;
          case 'W': { WIN_KEY_UPDATE(Key_W); } break;
          case 'X': { WIN_KEY_UPDATE(Key_X); } break;
          case 'Y': { WIN_KEY_UPDATE(Key_Y); } break;
          case 'Z': { WIN_KEY_UPDATE(Key_Z); } break;
          case '0': { WIN_KEY_UPDATE(Key_0); } break;
          case '1': { WIN_KEY_UPDATE(Key_1); } break;
          case '2': { WIN_KEY_UPDATE(Key_2); } break;
          case '3': { WIN_KEY_UPDATE(Key_3); } break;
          case '4': { WIN_KEY_UPDATE(Key_4); } break;
          case '5': { WIN_KEY_UPDATE(Key_5); } break;
          case '6': { WIN_KEY_UPDATE(Key_6); } break;
          case '7': { WIN_KEY_UPDATE(Key_7); } break;
          case '8': { WIN_KEY_UPDATE(Key_8); } break;
          case '9': { WIN_KEY_UPDATE(Key_9); } break;
          case VK_F1:       { WIN_KEY_UPDATE(Key_F1);           } break;
          case VK_F2:       { WIN_KEY_UPDATE(Key_F2);           } break;
          case VK_F3:       { WIN_KEY_UPDATE(Key_F3);           } break;
          case VK_F4:       { WIN_KEY_UPDATE(Key_F4);           } break;
          case VK_F5:       { WIN_KEY_UPDATE(Key_F5);           } break;
          case VK_F6:       { WIN_KEY_UPDATE(Key_F6);           } break;
          case VK_F7:       { WIN_KEY_UPDATE(Key_F7);           } break;
          case VK_F8:       { WIN_KEY_UPDATE(Key_F8);           } break;
          case VK_F9:       { WIN_KEY_UPDATE(Key_F9);           } break;
          case VK_F10:      { WIN_KEY_UPDATE(Key_F10);          } break;
          case VK_F11:      { WIN_KEY_UPDATE(Key_F11);          } break;
          case VK_F12:      { WIN_KEY_UPDATE(Key_F12);          } break;
          case VK_SPACE:    { WIN_KEY_UPDATE(Key_Space);        } break;
          case VK_CONTROL:  { WIN_KEY_UPDATE(Key_Control);      } break;
          case VK_LCONTROL: { WIN_KEY_UPDATE(Key_LeftControl);  } break;
          case VK_RCONTROL: { WIN_KEY_UPDATE(Key_RightControl); } break;
          case VK_SHIFT:    { WIN_KEY_UPDATE(Key_Shift);        } break;
          case VK_LSHIFT:   { WIN_KEY_UPDATE(Key_LeftShift);    } break;
          case VK_RSHIFT:   { WIN_KEY_UPDATE(Key_RightShift);   } break;
          case VK_CAPITAL:  { WIN_KEY_UPDATE(Key_CapsLock);     } break;
          case VK_TAB:      { WIN_KEY_UPDATE(Key_Tab);          } break;
          case VK_MENU:     { WIN_KEY_UPDATE(Key_Alt);          } break;
          case VK_UP:       { WIN_KEY_UPDATE(Key_Up);           } break;
          case VK_DOWN:     { WIN_KEY_UPDATE(Key_Down);         } break;
          case VK_LEFT:     { WIN_KEY_UPDATE(Key_Left);         } break;
          case VK_RIGHT:    { WIN_KEY_UPDATE(Key_Right);        } break;
          case VK_RETURN:   { WIN_KEY_UPDATE(Key_Return);       } break;
          case VK_DELETE:   { WIN_KEY_UPDATE(Key_Delete);       } break;
          case VK_ESCAPE:   { WIN_KEY_UPDATE(Key_Escape);       } break;
        }
      } break;

#undef WIN_KEY_UPDATE
#define WIN_MOUSE_UPDATE(x, y) WIN_KeyUpdate(&window->mouse_states[x], y);

      case WM_LBUTTONDOWN: { WIN_MOUSE_UPDATE(MouseButton_Left, true); } break;
      case WM_LBUTTONUP:   { WIN_MOUSE_UPDATE(MouseButton_Left, false); } break;
      case WM_RBUTTONDOWN: { WIN_MOUSE_UPDATE(MouseButton_Right, true); } break;
      case WM_RBUTTONUP:   { WIN_MOUSE_UPDATE(MouseButton_Right, false); } break;
      case WM_MBUTTONDOWN: { WIN_MOUSE_UPDATE(MouseButton_Middle, true); } break;
      case WM_MBUTTONUP:   { WIN_MOUSE_UPDATE(MouseButton_Middle, false); } break;
      case WM_XBUTTONDOWN: {
        if      ((msg.wParam & (1 << 16)) != 0) { WIN_MOUSE_UPDATE(MouseButton_X1, true); }
        else if ((msg.wParam & (1 << 17)) != 0) { WIN_MOUSE_UPDATE(MouseButton_X1, true); }
      } break;
      case WM_XBUTTONUP: {
        if      ((msg.wParam & (1 << 16)) != 0) { WIN_MOUSE_UPDATE(MouseButton_X1, false); }
        else if ((msg.wParam & (1 << 17)) != 0) { WIN_MOUSE_UPDATE(MouseButton_X1, false); }
      } break;
      case WM_MOUSEWHEEL: {
        F32 scroll_delta = (F32) GET_WHEEL_DELTA_WPARAM(msg.wParam) / WHEEL_DELTA;
        window->m_scroll_sign = SIGN(scroll_delta);
      } break;

#undef WIN_MOUSE_UPDATE

      default: {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
      } break;
    }
  }
}
//

B32 WIN_WindowShouldClose() {
  WIN_Window* window = &_win_window;
  DEBUG_ASSERT(window->initialized);
  return window->should_close;
}

void WIN_WindowSwapBuffers() {
  OpenGLAPI* g = &_ogl;
  WIN_Window* window = &_win_window;
  DEBUG_ASSERT(window->initialized);
  SwapBuffers(window->device_context);
  g->glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void WIN_WindowGetDims(S32* x, S32* y, S32* width, S32* height) {
  WIN_Window* window = &_win_window;
  DEBUG_ASSERT(window->initialized);
  RECT dims;
  GetClientRect(window->handle, &dims);
  if (x != NULL)      { *x = dims.left;                   }
  if (y != NULL)      { *y = dims.top;                    }
  if (width != NULL)  { *width = dims.right - dims.left;  }
  if (height != NULL) { *height = dims.bottom - dims.top; }
}

void WIN_WindowSetTitle(char* title) {
  WIN_Window* window = &_win_window;
  DEBUG_ASSERT(window->initialized);
  SetWindowTextA(window->handle, title);
}

void WIN_WindowSetSize(S32 width, S32 height) {
  WIN_Window* window = &_win_window;
  DEBUG_ASSERT(window->initialized);

  window->width = width;
  window->height = height;

  WINDOWPLACEMENT window_placement;
  MEMORY_ZERO_STRUCT(&window_placement);
  window_placement.length = sizeof(window_placement);
  GetWindowPlacement(window->handle, &window_placement);
  window_placement.rcNormalPosition.right = window_placement.rcNormalPosition.left + window->width;
  window_placement.rcNormalPosition.bottom = window_placement.rcNormalPosition.top + window->height;
  SetWindowPlacement(window->handle, &window_placement);
}

void WIN_WindowShowCursor(B32 enable) {
  ShowCursor(enable);
}

// https://devblogs.microsoft.com/oldnewthing/20100412-00/?p=14353
void WIN_WindowFullscreen(B32 enable) {
  WIN_Window* window = &_win_window;
  DEBUG_ASSERT(window->initialized);

  DWORD style = GetWindowLong(window->handle, GWL_STYLE);
  if (enable) {
    MONITORINFO monitor_info;
    MEMORY_ZERO_STRUCT(&monitor_info);
    monitor_info.cbSize = sizeof(monitor_info);
    if (GetMonitorInfo(MonitorFromWindow(window->handle, MONITOR_DEFAULTTOPRIMARY), &monitor_info)) {
      SetWindowLong(window->handle, GWL_STYLE, style & ~WINDOW_STYLE);
      SetWindowPos(window->handle, HWND_TOP,
                   monitor_info.rcMonitor.left, monitor_info.rcMonitor.top,
                   monitor_info.rcMonitor.right - monitor_info.rcMonitor.left,
                   monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top,
                   SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
  } else {
    WIN_WindowSetSize(window->width, window->height);
    SetWindowLong(window->handle, GWL_STYLE, style | WINDOW_STYLE);
    SetWindowPos(window->handle, NULL, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
  }
}

B32 WIN_WindowIsKeyPressed(KeyboardKey key) {
  WIN_Window* window = &_win_window;
  DEBUG_ASSERT(window->initialized);
  return window->key_states[key] & BIT(WIN_KEY_PRESSED);
}

B32 WIN_WindowIsKeyReleased(KeyboardKey key) {
  return !WIN_WindowIsKeyPressed(key);
}

B32 WIN_WindowIsKeyJustPressed(KeyboardKey key) {
  WIN_Window* window = &_win_window;
  DEBUG_ASSERT(window->initialized);
  return window->key_states[key] & BIT(WIN_KEY_JUST_PRESSED);
}

B32 WIN_WindowIsKeyJustReleased(KeyboardKey key) {
  WIN_Window* window = &_win_window;
  DEBUG_ASSERT(window->initialized);
  return window->key_states[key] & BIT(WIN_KEY_JUST_RELEASED);
}

B32 WIN_WindowIsMouseButtonPressed(MouseButton button) {
  WIN_Window* window = &_win_window;
  DEBUG_ASSERT(window->initialized);
  return window->mouse_states[button] & BIT(WIN_KEY_PRESSED);
}

B32 WIN_WindowIsMouseButtonReleased(MouseButton button) {
  return !WIN_WindowIsMouseButtonPressed(button);
}

B32 WIN_WindowIsMouseButtonJustPressed(MouseButton button) {
  WIN_Window* window = &_win_window;
  DEBUG_ASSERT(window->initialized);
  return window->mouse_states[button] & BIT(WIN_KEY_JUST_PRESSED);
}

B32 WIN_WindowIsMouseButtonJustReleased(MouseButton button) {
  WIN_Window* window = &_win_window;
  DEBUG_ASSERT(window->initialized);
  return window->mouse_states[button] & BIT(WIN_KEY_JUST_RELEASED);
}

F32 WIN_WindowGetMouseScrollSign() {
  WIN_Window* window = &_win_window;
  DEBUG_ASSERT(window->initialized);
  return window->m_scroll_sign;
}

void WIN_WindowGetMousePosition(F32* x, F32* y) {
  WIN_Window* window = &_win_window;
  DEBUG_ASSERT(window->initialized);
  *x = window->mouse_pos.x;
  *y = window->mouse_pos.y;
}

void WIN_WindowGetMousePositionV(V2* pos) {
  WIN_WindowGetMousePosition(&pos->x, &pos->y);
}

void WIN_WindowGetMouseDeltaPosition(F32* x, F32* y) {
  WIN_Window* window = &_win_window;
  DEBUG_ASSERT(window->initialized);
  *x = window->mouse_pos_delta.x;
  *y = window->mouse_pos_delta.y;
}

void WIN_WindowGetMouseDeltaPositionV(V2* pos) {
  WIN_WindowGetMouseDeltaPosition(&pos->x, &pos->y);
}

#else
#  error Unsupported OS for cdefault render.
#endif

#define CDEFAULT_RENDER_BACKEND_FN_IMPL(ns, fn) GLUE(ns, fn)
#define CDEFAULT_RENDER_BACKEND_FN(x) CDEFAULT_RENDER_BACKEND_FN_IMPL(CDEFAULT_RENDER_BACKEND_NAMESPACE, x)

B32 WindowInit(S32 width, S32 height, char* title) {
  return CDEFAULT_RENDER_BACKEND_FN(WindowInit(width, height, title));
}

void WindowDeinit() {
  CDEFAULT_RENDER_BACKEND_FN(WindowDeinit());
}

void WindowFlushEvents() {
  CDEFAULT_RENDER_BACKEND_FN(WindowFlushEvents());
}

B32 WindowShouldClose() {
  return CDEFAULT_RENDER_BACKEND_FN(WindowShouldClose());
}

void WindowSwapBuffers() {
  CDEFAULT_RENDER_BACKEND_FN(WindowSwapBuffers());
}

void WindowGetDims(S32* x, S32* y, S32* width, S32* height) {
  CDEFAULT_RENDER_BACKEND_FN(WindowGetDims(x, y, width, height));
}

void WindowSetTitle(char* title) {
  CDEFAULT_RENDER_BACKEND_FN(WindowSetTitle(title));
}

void WindowSetSize(S32 width, S32 height) {
  CDEFAULT_RENDER_BACKEND_FN(WindowSetSize(width, height));
}

void WindowShowCursor(B32 enable) {
  CDEFAULT_RENDER_BACKEND_FN(WindowShowCursor(enable));
}

void WindowFullscreen(B32 enable) {
  CDEFAULT_RENDER_BACKEND_FN(WindowFullscreen(enable));
}

B32 WindowIsKeyJustPressed(KeyboardKey key) {
  return CDEFAULT_RENDER_BACKEND_FN(WindowIsKeyJustPressed(key));
}

B32 WindowIsKeyPressed(KeyboardKey key) {
  return CDEFAULT_RENDER_BACKEND_FN(WindowIsKeyPressed(key));
}

B32 WindowIsKeyJustReleased(KeyboardKey key) {
  return CDEFAULT_RENDER_BACKEND_FN(WindowIsKeyJustReleased(key));
}

B32 WindowIsKeyReleased(KeyboardKey key) {
  return CDEFAULT_RENDER_BACKEND_FN(WindowIsKeyReleased(key));
}

B32 WindowIsMouseButtonPressed(MouseButton button) {
  return CDEFAULT_RENDER_BACKEND_FN(WindowIsMouseButtonPressed(button));
}

B32 WindowIsMouseButtonReleased(MouseButton button) {
  return CDEFAULT_RENDER_BACKEND_FN(WindowIsMouseButtonReleased(button));
}

B32 WindowIsMouseButtonJustPressed(MouseButton button) {
  return CDEFAULT_RENDER_BACKEND_FN(WindowIsMouseButtonJustPressed(button));
}

B32 WindowIsMouseButtonJustReleased(MouseButton button) {
  return CDEFAULT_RENDER_BACKEND_FN(WindowIsMouseButtonJustReleased(button));
}

F32 WindowGetMouseScrollSign() {
  return CDEFAULT_RENDER_BACKEND_FN(WindowGetMouseScrollSign());
}

void WindowGetMousePosition(F32* x, F32* y) {
  CDEFAULT_RENDER_BACKEND_FN(WindowGetMousePosition(x, y));
}

void WindowGetMousePositionV(V2* pos) {
  CDEFAULT_RENDER_BACKEND_FN(WindowGetMousePositionV(pos));
}

void WindowGetMouseDeltaPosition(F32* x, F32* y) {
  CDEFAULT_RENDER_BACKEND_FN(WindowGetMouseDeltaPosition(x, y));
}

void WindowGetMouseDeltaPositionV(V2* pos) {
  CDEFAULT_RENDER_BACKEND_FN(WindowGetMouseDeltaPositionV(pos));
}

#endif // CDEFAULT_RENDER_IMPLEMENTATION
