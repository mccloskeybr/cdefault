#ifndef CDEFAULT_VIDEO_H_
#define CDEFAULT_VIDEO_H_

#include "cdefault_std.h"
#include "cdefault_math.h"

// TODO:
// vsync
// shader abstraction?
// draw frames not just solid shapes?
// draw more shapes
// softer edges

// NOTE: A window must exist before any renderer / draw functions can be called.
// NOTE: Sensible defaults are chosen if field values are 0.
typedef struct WindowInitOpts WindowInitOpts;
struct WindowInitOpts {
  S32 x;
  S32 y;
  S32 width;
  S32 height;
  char* title;
  U32 clear_rgb;
};
B32  WindowInit(WindowInitOpts opts);
void WindowDeinit();
void WindowFlushEvents();
void WindowSwapBuffers();
void WindowGetDims(S32* x, S32* y, S32* width, S32* height);
void WindowSetTitle(char* title);
void WindowSetSize(S32 width, S32 height); // NOTE: Does not update resolution, may need to call RendererSetProjection separately.
void WindowShowCursor(B32 enable);
void WindowFullscreen(B32 enable);

void RendererSetProjection(M4 projection);
void RendererEnableScissorTest(S32 x, S32 y, S32 width, S32 height);
void RendererDisableScissorTest(void);
void RendererEnableDepthTest(void);
void RendererDisableDepthTest(void);
void RendererSetClearColor(F32 r, F32 g, F32 b, F32 a);
void RendererSetClearColorV(V4 color);
void RendererCastRay(F32 x, F32 y, V3* start, V3* dir);

void DrawLine(F32 start_x, F32 start_y, F32 end_x, F32 end_y, F32 thickness, F32 red, F32 green, F32 blue);
void DrawLineV(V2 start, V2 end, F32 thickness, V3 color);
void DrawRectangle(F32 center_x, F32 center_y, F32 width, F32 height, F32 red, F32 green, F32 blue);
void DrawRectangleV(V2 center, V2 size, V3 color);
void DrawRectangleRot(F32 center_x, F32 center_y, F32 width, F32 height, F32 andle_rad, F32 red, F32 green, F32 blue);
void DrawRectangleRotV(V2 center, V2 size, F32 angle_rad, V3 color);
void DrawRoundedRectangle(F32 center_x, F32 center_y, F32 width, F32 height, F32 radius, F32 red, F32 green, F32 blue);
void DrawRoundedRectangleV(V2 center, V2 size, F32 radius, V3 color);
void DrawRoundedRectangleRot(F32 center_x, F32 center_y, F32 width, F32 height, F32 radius, F32 angle_rad, F32 red, F32 green, F32 blue);
void DrawRoundedRectangleRotV(V2 center, V2 size, F32 radius, F32 angle_rad, V3 color);
void DrawCircle(F32 center_x, F32 center_y, F32 radius, F32 red, F32 green, F32 blue);
void DrawCircleV(V2 center, F32 radius, V3 color);
void DrawTriangle(F32 x1, F32 y1, F32 x2, F32 y2, F32 x3, F32 y3, F32 red, F32 green, F32 blue); // NOTE: must respect CCW winding order.
void DrawTriangleV(V2 p1, V2 p2, V2 p3, V3 color);

void RGBToF32s(U32 hex, F32* r, F32* g, F32* b);
void RGBToU32s(U32 hex, U32* r, U32* g, U32* b);
U32  RGBFromF32s(F32 r, F32 g, F32 b);
U32  RGBFromU32s(U32 r, U32 g, U32 b);

#endif // CDEFAULT_VIDEO_H_

#ifdef CDEFAULT_VIDEO_IMPLEMENTATION
#undef CDEFAULT_VIDEO_IMPLEMENTATION

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

  GLuint tri_shader;
  GLint tri_camera_uniform;
  GLint tri_color_uniform;

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
    LOG_ERROR("[VIDEO] Failed to compile vertex shader."); // TODO: print ogl error message.
    goto renderer_compile_shader_exit;
  }

  fragment_shader = g->glCreateShader(GL_FRAGMENT_SHADER);
  g->glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
  g->glCompileShader(fragment_shader);
  g->glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &result);
  if (!result) {
    LOG_ERROR("[VIDEO] Failed to compile fragment shader.");
    goto renderer_compile_shader_exit;
  }

  *shader = g->glCreateProgram();
  g->glAttachShader(*shader, vertex_shader);
  g->glAttachShader(*shader, fragment_shader);
  g->glLinkProgram(*shader);
  g->glGetShaderiv(*shader, GL_LINK_STATUS, &result);
  if (!result) {
    LOG_ERROR("[VIDEO] Failed to link shader program.");
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

  LOG_INFO("[VIDEO] Initializing OpenGL renderer.");

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
    "  vec2 q = abs(centered_pos) - (size / 2.0) + radius;\n"
    "  float dist = length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - radius;\n"
    "  float alpha = 1.0 - smoothstep(0.0, 1.0, dist);\n"
    "  frag_color = vec4(color, alpha);\n"
    "}\0";
  DEBUG_ASSERT(RendererCompileShader(&r->rect_shader, rect_vertex_shader_source, rect_fragment_shader_source));
  r->rect_camera_uniform = g->glGetUniformLocation(r->rect_shader, "to_camera_transform");
  r->rect_color_uniform = g->glGetUniformLocation(r->rect_shader, "color");
  r->rect_size_uniform = g->glGetUniformLocation(r->rect_shader, "size");
  r->rect_radius_uniform = g->glGetUniformLocation(r->rect_shader, "radius");

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
  M4Orthographic(0, width, 0, height, 0.01f, 100.0f, &projection);
  RendererSetProjection(projection);

  LOG_INFO("[VIDEO] OpenGL renderer initialized.");
  success = true;

  return success;
}

void RendererSetProjection(M4 projection) {
  Renderer* r = &_renderer;
  V3 pos    = { 0, 0, 1 }; // NOTE: seat camera Z back so items can exist at z = 0.
  V3 target = V3_Z_NEG;
  V3 up     = V3_Y_POS;
  M4 camera;
  M4LookAt(&pos, &target, &up, &camera);
  M4MultM4(&projection, &camera, &r->world_to_camera);
}

void RendererEnableScissorTest(S32 x, S32 y, S32 width, S32 height) {
  OpenGLAPI* g = &_ogl;
  glEnable(GL_SCISSOR_TEST);
  g->glScissor(x, y, width, height);
}

void RendererDisableScissorTest(void) {
  glDisable(GL_SCISSOR_TEST);
}

void RendererEnableDepthTest(void) {
  glEnable(GL_DEPTH_TEST);
}

void RendererDisableDepthTest(void) {
  glDisable(GL_DEPTH_TEST);
}

void RendererSetClearColor(F32 r, F32 g, F32 b, F32 a) {
  glClearColor(r, g, b, a);
}

void RendererSetClearColorV(V4 color) {
  RendererSetClearColor(color.r, color.g, color.b, color.a);
}

static V3 UnprojectPoint(V3 p, M4* inv_vp) {
  V4 v = { p.x, p.y, p.z, 1.0f };
  V4 v2;
  M4MultV4(inv_vp, &v, &v2);
  V4DivF32(&v2, v2.w, &v2);
  return (V3) { v2.x, v2.y, v2.z };
}

void RendererCastRay(F32 x, F32 y, V3* start, V3* dir) {
  Renderer* r = &_renderer;

  S32 width, height;
  WindowGetDims(NULL, NULL, &width, &height);
  F32 x2 = (x / (((F32) width) / 2.0f)) - 1.0f;
  F32 y2 = (y / (((F32) height) / 2.0f)) - 1.0f;
  x2 = CLAMP(-1, x2, 1);
  y2 = CLAMP(-1, y2, 1);

  M4 world_to_camera_inv;
  M4Invert(&r->world_to_camera, &world_to_camera_inv);
  V3 n = UnprojectPoint((V3) { x2, y2, -1}, &world_to_camera_inv);
  V3 f = UnprojectPoint((V3) { x2, y2, +1}, &world_to_camera_inv);
  V3 delta;
  V3SubV3(&f, &n, &delta);
  V3Normalize(&delta, &delta);

  if (start != NULL) { *start = n; }
  if (dir != NULL) { *dir = delta; }
}

void DrawLine(F32 start_x, F32 start_y, F32 end_x, F32 end_y, F32 thickness, F32 red, F32 green, F32 blue) {
  V2 start = { start_x, start_y };
  V2 end   = { end_x, end_y };
  V2 delta;
  V2SubV2(&end, &start, &delta);
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

void DrawRoundedRectangleRot(F32 center_x, F32 center_y, F32 width, F32 height, F32 radius, F32 angle_rad, F32 red, F32 green, F32 blue) {
  Renderer* r = &_renderer;
  OpenGLAPI* g = &_ogl;

  V3 color = { red, green, blue };
  V3 pos   = { center_x, center_y, 0 };
  V3 scale = { width, height, 1 };
  V4 rot;
  V4RotateAroundAxis(&V3_Z_POS, angle_rad, &rot);
  M4 rect_to_world, rect_to_camera, rect_to_camera_t;
  M4FromTransform(&pos, &rot, &scale, &rect_to_world);
  M4MultM4(&r->world_to_camera, &rect_to_world, &rect_to_camera);
  M4Transpose(&rect_to_camera, &rect_to_camera_t);

  g->glUseProgram(r->rect_shader);
  g->glUniformMatrix4fv(r->rect_camera_uniform, 1, GL_FALSE, (GLfloat*) &rect_to_camera_t);
  g->glUniform3fv(r->rect_color_uniform, 1, (GLfloat*) &color);
  g->glUniform2fv(r->rect_size_uniform, 1, (GLfloat*) &scale);
  g->glUniform1f(r->rect_radius_uniform, radius);
  g->glBindVertexArray(r->quad_vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  g->glBindVertexArray(0);
  g->glUseProgram(0);
}

void DrawRoundedRectangleRotV(V2 center, V2 size, F32 radius, F32 angle, V3 color) {
  DrawRoundedRectangleRot(center.x, center.y, size.x, size.y, radius, angle, color.r, color.g, color.b);
}

void DrawCircle(F32 center_x, F32 center_y, F32 radius, F32 red, F32 green, F32 blue) {
  DrawRoundedRectangle(center_x, center_y, radius * 2, radius * 2, radius, red, green, blue);
}

void DrawCircleV(V2 center, F32 radius, V3 color) {
  DrawCircle(center.x, center.y, radius, color.r, color.g, color.b);
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
  M4FromTransform(&pos, &rot, &scale, &tri_to_world);
  M4MultM4(&r->world_to_camera, &tri_to_world, &tri_to_camera);
  M4Transpose(&tri_to_camera, &tri_to_camera_t);

  g->glUseProgram(r->tri_shader);
  g->glUniformMatrix4fv(r->tri_camera_uniform, 1, GL_FALSE, (GLfloat*) &tri_to_camera_t);
  g->glUniform3fv(r->tri_color_uniform, 1, (GLfloat*) &color);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  g->glBindVertexArray(0);
  g->glUseProgram(0);

  g->glDeleteBuffers(1, &tri_vbo);
  g->glDeleteVertexArrays(1, &tri_vao);
}

void DrawTriangleV(V2 p1, V2 p2, V2 p3, V3 color) {
  DrawTriangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, color.r, color.g, color.b);
}

void RGBToF32s(U32 hex, F32* r, F32* g, F32* b) {
  U32 r2, g2, b2;
  RGBToU32s(hex, &r2, &g2, &b2);
  *r = r2 / 255.0f;
  *g = g2 / 255.0f;
  *b = b2 / 255.0f;
}

void RGBToU32s(U32 hex, U32* r, U32* g, U32* b) {
  *r = (U32) ((hex & 0xff0000) >> 16);
  *g = (U32) ((hex & 0x00ff00) >> 8);
  *b = (U32) ((hex & 0x0000ff) >> 0);
}

U32 RGBFromF32s(F32 r, F32 g, F32 b) {
  U32 r2, g2, b2;
  r2 = (U32) (r * 255);
  g2 = (U32) (g * 255);
  b2 = (U32) (b * 255);
  return RGBFromU32s(r2, g2, b2);
}

U32 RGBFromU32s(U32 r, U32 g, U32 b) {
  return (r << 16) | (g << 8) | (b << 0);
}

#if defined(OS_WINDOWS)
#define CDEFAULT_VIDEO_BACKEND_NAMESPACE WIN_
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

typedef struct WIN_Window WIN_Window;
struct WIN_Window {
  B32 initialized;
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
  OpenGLAPI* g = &_ogl;
  switch (umsg) {
    case WM_WINDOWPOSCHANGED: {
      if (g->glViewport != NULL) {
        S32 width, height;
        WindowGetDims(NULL, NULL, &width, &height);
        g->glViewport(0, 0, width, height);
      }
      return DefWindowProc(hwnd, umsg, wparam, lparam);
    } break;
    default: {
      return DefWindowProc(hwnd, umsg, wparam, lparam);
    } break;
  }
}

B32 WIN_WindowInit(WindowInitOpts opts) {
  WIN_Window* window = &_win_window;
  DEBUG_ASSERT(!window->initialized);

  LOG_INFO("[VIDEO] Initializing window.");

  if (opts.x == 0)          { opts.x = CW_USEDEFAULT;      }
  if (opts.y == 0)          { opts.y = CW_USEDEFAULT;      }
  if (opts.width == 0)      { opts.width = CW_USEDEFAULT;  }
  if (opts.height == 0)     { opts.height = CW_USEDEFAULT; }
  if (opts.title == NULL)   { opts.title = "title";        }

  B32 success = false;

  HINSTANCE hinstance = GetModuleHandle(NULL);

  WNDCLASS window_class;
  MEMORY_ZERO_STRUCT(&window_class);
  window_class.lpfnWndProc = WIN_WindowCallback;
  window_class.hInstance = hinstance;
  window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
  window_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  window_class.lpszMenuName = "CDEFAULT_MENU";
  window_class.lpszClassName = "CDEFAULT_WINDOW_CLASS";
  RegisterClass(&window_class);

  DWORD create_window_flags = 0;
  create_window_flags |= WINDOW_STYLE;
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
    WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB, // WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
    0
  };
  ogl_context = wglCreateContextAttribsARB(device_context, 0, ogl_attr);
  if (!wglMakeCurrent(device_context, ogl_context)) {
    LOG_ERROR("Failed to create OpenGL context: %d", GetLastError());
    goto win_window_create_exit;
  }

  // TODO: verify availability.
  // TODO: move to renderer init.
  glEnable(GL_FRAMEBUFFER_SRGB);
  glEnable(GL_CULL_FACE);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  F32 clear_r, clear_g, clear_b;
  RGBToF32s(opts.clear_rgb, &clear_r, &clear_g, &clear_b);
  glClearColor(clear_r, clear_g, clear_b, 0);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // TODO: cover all ogl functions.
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
  _ogl.glScissor = glScissor;
  _ogl.glViewport = glViewport;

#undef WIN_LINK_GL

  WINDOWPLACEMENT window_placement;
  MEMORY_ZERO_STRUCT(&window_placement);
  window_placement.length = sizeof(window_placement);
  GetWindowPlacement(window->handle, &window_placement);
  window->width = window_placement.rcNormalPosition.right - window_placement.rcNormalPosition.left;
  window->height = window_placement.rcNormalPosition.bottom - window_placement.rcNormalPosition.top;
  window->handle = handle;
  window->device_context = device_context;
  window->ogl_context = ogl_context;
  window->initialized = true;

  if (!RendererInit()) {
    LOG_ERROR("[VIDEO] Failed to initialize renderer.");
    window->initialized = false;
    goto win_window_create_exit;
  }

  LOG_INFO("[VIDEO] Window initialized successfully.");

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

void WIN_WindowFlushEvents() {
  WIN_Window* window = &_win_window;
  DEBUG_ASSERT(window->initialized);
  MSG msg;
  while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
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

void WindowFlushEvents() {
  CDEFAULT_VIDEO_BACKEND_FN(WindowFlushEvents());
}

void WindowSwapBuffers() {
  CDEFAULT_VIDEO_BACKEND_FN(WindowSwapBuffers());
}

void WindowGetDims(S32* x, S32* y, S32* width, S32* height) {
  CDEFAULT_VIDEO_BACKEND_FN(WindowGetDims(x, y, width, height));
}

void WindowSetTitle(char* title) {
  CDEFAULT_VIDEO_BACKEND_FN(WindowSetTitle(title));
}

void WindowSetSize(S32 width, S32 height) {
  CDEFAULT_VIDEO_BACKEND_FN(WindowSetSize(width, height));
}

void WindowShowCursor(B32 enable) {
  CDEFAULT_VIDEO_BACKEND_FN(WindowShowCursor(enable));
}

void WindowFullscreen(B32 enable) {
  CDEFAULT_VIDEO_BACKEND_FN(WindowFullscreen(enable));
}

#endif // CDEFAULT_VIDEO_IMPLEMENTATION
