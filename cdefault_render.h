#ifndef CDEFAULT_RENDER_H_
#define CDEFAULT_RENDER_H_

#include "cdefault_std.h"
#include "cdefault_math.h"
#include "cdefault_image.h"
#include "cdefault_font.h"

// TODO:
// move 2d camera
// 3d shape primative draw fns
// vsync
// shader abstraction?
// draw more shapes
// softer edges?
// open gl error detection

typedef struct Camera Camera;
struct Camera {
  V3 pos;
  V3 look_dir; // NOTE: Expected to be normalized.
  V3 up_dir;   // NOTE: Expected to be normalized and orthogonal to look_dir.
};

// NOTE: A window must exist before any renderer / draw functions can be called.
// NOTE: Sensible defaults are chosen if field values are 0.
B32  WindowInit(S32 width, S32 height, char* title);
void WindowDeinit();
void WindowFlushEvents(); // NOTE: Must be called regularly, e.g. on some frame boundary.
B32  WindowShouldClose(); // NOTE: True when the window has been closed.
void WindowSwapBuffers();
void WindowGetDims(S32* x, S32* y, S32* width, S32* height); // NOTE: viewport dimensions; not necessarily the raw window size.
void WindowSetTitle(char* title);
void WindowSetSize(S32 width, S32 height); // NOTE: Does not update resolution, may need to update the projection matrices as necessary.
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

void RendererSetProjection2D(M4 projection);
void RendererSetProjection3D(M4 projection);
Camera* RendererCamera3D();
void RendererRegisterImage(U32* image_handle, Image* image);
void RendererRegisterMesh(U32* mesh_handle, U32 image_handle, V3* points, V3* normals, V2* uvs, U32 vertices_size, U32* indices, U32 indices_size);
void RendererReleaseImage(U32 image_handle);
void RendererReleaseMesh(U32 mesh_handle);
void RendererEnableScissorTest(S32 x, S32 y, S32 width, S32 height);
void RendererDisableScissorTest(void);
void RendererEnableDepthTest(void);
void RendererDisableDepthTest(void);
void RendererEnableWireframe(void);
void RendererDisableWireframe(void);
void RendererSetClearColor(F32 red, F32 green, F32 blue, F32 alpha);
void RendererSetClearColorV(V4 color);
void RendererCastRay(F32 x, F32 y, V2* intersect);
void RendererCastRayV(V2 pos, V2* intersect);
void RendererCastRay3(F32 x, F32 y, V3* start, V3* dir);
void RendererCastRay3V(V2 pos, V3* start, V3* dir);

// NOTE: 2D API
void DrawLine2(F32 start_x, F32 start_y, F32 end_x, F32 end_y, F32 thickness, F32 red, F32 green, F32 blue);
void DrawLine2V(V2 start, V2 end, F32 thickness, V3 color);
void DrawQuadBezier(F32 start_x, F32 start_y, F32 control_x, F32 control_y, F32 end_x, F32 end_y, U32 resolution, F32 thickness, F32 red, F32 green, F32 blue);
void DrawQuadBezierV(V2 start, V2 control, V2 end, U32 resolution, F32 thickness, V3 color);
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
void DrawSubImage(U32 image_handle, F32 center_x, F32 center_y, F32 width, F32 height, F32 min_uv_x, F32 min_uv_y, F32 max_uv_x, F32 max_uv_y);
void DrawSubImageV(U32 image_handle, V2 center, V2 size, V2 min_uv, V2 max_uv);
void DrawSubImageRot(U32 image_handle, F32 center_x, F32 center_y, F32 width, F32 height, F32 angle_rad, F32 min_uv_x, F32 min_uv_y, F32 max_uv_x, F32 max_uv_y);
void DrawSubImageRotV(U32 image_handle, V2 center, V2 size, F32 angle_rad, V2 min_uv, V2 max_uv);
void DrawFontBmpCharacter(U32 image_handle, F32 center_x, F32 center_y, F32 width, F32 height, F32 min_uv_x, F32 min_uv_y, F32 max_uv_x, F32 max_uv_y, F32 red, F32 green, F32 blue);
void DrawFontBmpCharacterV(U32 image_handle, V2 center, V2 size, V2 min_uv, V2 max_uv, V3 color);
void DrawStringBmp(String8 str, FontAtlas* atlas, U32 atlas_handle, F32 font_height, F32 x, F32 y);
void DrawStringBmpV(String8 str, FontAtlas* atlas, U32 atlas_handle, F32 font_height, V2 pos);
void DrawFontSdfCharacter(U32 image_handle, F32 center_x, F32 center_y, F32 width, F32 height, F32 min_uv_x, F32 min_uv_y, F32 max_uv_x, F32 max_uv_y, F32 threshold, F32 smoothing, F32 red, F32 green, F32 blue);
void DrawFontSdfCharacterV(U32 image_handle, V2 center, V2 size, V2 min_uv, V2 max_uv, F32 threshold, F32 smoothing, V3 color);
void DrawStringSdf(String8 str, FontAtlas* atlas, U32 atlas_handle, F32 font_height, F32 x, F32 y);
void DrawStringSdfV(String8 str, FontAtlas* atlas, U32 atlas_handle, F32 font_height, V2 pos);
void DrawStringSdfEx(String8 str, FontAtlas* atlas, U32 atlas_handle, F32 font_height, F32 threshold, F32 smoothing, F32 x, F32 y);
void DrawStringSdfExV(String8 str, FontAtlas* atlas, U32 atlas_handle, F32 font_height, F32 threshold, F32 smoothing, V2 pos);

// NOTE: 3D API
// TODO: draw flat shapes
void DrawLine3(F32 start_x, F32 start_y, F32 start_z, F32 end_x, F32 end_y, F32 end_z, F32 color_r, F32 color_g, F32 color_b);
void DrawLine3V(V3 start, V3 end, V3 color);
void DrawSphere(F32 center_x, F32 center_y, F32 center_z, F32 rot_x, F32 rot_y, F32 rot_z, F32 rot_w, F32 radius, F32 red, F32 green, F32 blue);
void DrawSphereV(V3 center, V4 rot, F32 radius, V3 color);
void DrawCube(F32 center_x, F32 center_y, F32 center_z, F32 rot_x, F32 rot_y, F32 rot_z, F32 rot_w, F32 size_x, F32 size_y, F32 size_z, F32 red, F32 green, F32 blue);
void DrawCubeV(V3 center, V4 rot, V3 size, V3 color);
void DrawTetrahedron(F32 x1, F32 y1, F32 z1, F32 x2, F32 y2, F32 z2, F32 x3, F32 y3, F32 z3, F32 x4, F32 y4, F32 z4, F32 color_r, F32 color_g, F32 color_b);
void DrawTetrahedronV(V3 p1, V3 p2, V3 p3, V3 p4, V3 color);
void DrawMesh(U32 mesh_handle, F32 pos_x, F32 pos_y, F32 pos_z, F32 quat_x, F32 quat_y, F32 quat_z, F32 quat_w, F32 scale_x, F32 scale_y, F32 scale_z);
void DrawMeshV(U32 mesh_handle, V3 pos, V4 rot, V3 scale);
void DrawMeshColor(U32 mesh_handle, F32 pos_x, F32 pos_y, F32 pos_z, F32 quat_x, F32 quat_y, F32 quat_z, F32 quat_w, F32 scale_x, F32 scale_y, F32 scale_z, F32 color_r, F32 color_g, F32 color_b);
void DrawMeshColorV(U32 mesh_handle, V3 pos, V4 rot, V3 scale, V3 color);
void DrawMeshEx(U32 mesh_handle, F32 pos_x, F32 pos_y, F32 pos_z, F32 quat_x, F32 quat_y, F32 quat_z, F32 quat_w, F32 scale_x, F32 scale_y, F32 scale_z, F32 color_r, F32 color_g, F32 color_b, F32 tex_color_ratio);
void DrawMeshExV(U32 mesh_handle, V3 pos, V4 rot, V3 scale, V3 color, F32 tex_color_ratio);

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
#define GL_TEXTURE1                 0x84C1

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
typedef void   glBufferSubData_Fn(GLenum, GLintptr, GLsizeiptr, const void*);
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
typedef void   glActiveTexture_Fn(GLenum);
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
typedef void   glDrawElements_Fn(GLenum, GLsizei, GLenum, const void*);
typedef void   glPixelStorei_Fn(GLenum, GLint);
typedef void   glPolygonMode_Fn(GLenum, GLenum);
typedef void   glLineWidth_Fn(GLfloat);

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
  glBufferSubData_Fn*           glBufferSubData;
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
  glActiveTexture_Fn*           glActiveTexture;
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
  glDrawElements_Fn*            glDrawElements;
  glPixelStorei_Fn*             glPixelStorei;
  glPolygonMode_Fn*             glPolygonMode;
  glLineWidth_Fn*               glLineWidth;
};
static OpenGLAPI _ogl;

// TODO: accessing through a linked list is prolly a little too slow for this.
typedef struct RenderMesh RenderMesh;
struct RenderMesh {
  U32 id;
  U32 vao;
  U32 vbo;
  U32 ibo;
  U32 indices_size;
  U32 image_handle;
  RenderMesh* prev;
  RenderMesh* next;
};

typedef struct Renderer Renderer;
struct Renderer {
  // NOTE: 2d data

  GLuint quad_vbo;
  GLuint quad_vao;

  GLuint rect_shader;
  GLint  rect_camera_uniform;
  GLint  rect_color_uniform;
  GLint  rect_size_uniform;
  GLint  rect_radius_uniform;

  GLuint frame_shader;
  GLint  frame_camera_uniform;
  GLint  frame_color_uniform;
  GLint  frame_size_uniform;
  GLint  frame_border_size_uniform;
  GLint  frame_radius_uniform;

  GLuint tri_shader;
  GLint  tri_camera_uniform;
  GLint  tri_color_uniform;

  GLuint image_shader;
  GLint  image_camera_uniform;
  GLint  image_min_uv_uniform;
  GLint  image_max_uv_uniform;

  GLuint font_shader;
  GLint  font_camera_uniform;
  GLint  font_min_uv_uniform;
  GLint  font_max_uv_uniform;
  GLint  font_color_uniform;

  GLuint font_sdf_shader;
  GLint  font_sdf_camera_uniform;
  GLint  font_sdf_threshold_uniform;
  GLint  font_sdf_smoothing_uniform;
  GLint  font_sdf_min_uv_uniform;
  GLint  font_sdf_max_uv_uniform;
  GLint  font_sdf_color_uniform;

  // NOTE: 3D data

  U32 next_mesh_id;
  Arena* mesh_pool;
  RenderMesh* meshes_head;
  RenderMesh* meshes_tail;
  RenderMesh* meshes_free_list;
  U32 icosphere_handle;
  U32 cube_handle;

  GLuint mesh_shader;
  GLint  mesh_camera_uniform;
  GLint  mesh_texture_uniform;
  GLint  mesh_color_uniform;
  GLint  mesh_tex_color_ratio_uniform;

  GLuint line_shader;
  GLint  line_camera_uniform;
  GLint  line_color_uniform;

  Camera camera_3d;
  M4 projection_3d;
  M4 world_to_camera_2d;
};
static Renderer _renderer;

static RenderMesh* RendererFindMesh(U32 mesh_handle) {
  Renderer* r = &_renderer;
  for (RenderMesh* curr = r->meshes_head; curr != NULL; curr = curr->next) {
    if (curr->id == mesh_handle) { return curr; }
  }
  return NULL;
}

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
  MEMORY_ZERO_STRUCT(r);
  OpenGLAPI* g = &_ogl;
  B32 success = false; // TODO: error checking.

  S32 max_texture_size;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);

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
    "uniform vec2 min_uv;\n"
    "uniform vec2 max_uv;\n"
    "layout (location = 0) in vec2 in_pos;\n"
    "layout (location = 1) in vec2 in_tex_coord;\n"
    "out vec2 tex_coord;\n"
    "void main() {\n"
    "  tex_coord = mix(min_uv, max_uv, in_tex_coord);\n"
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
  r->image_min_uv_uniform = g->glGetUniformLocation(r->image_shader, "min_uv");
  r->image_max_uv_uniform = g->glGetUniformLocation(r->image_shader, "max_uv");

  char* font_vertex_shader_source =
    "#version 330 core\n"
    "uniform mat4 to_camera_transform;\n"
    "uniform vec2 min_uv;\n"
    "uniform vec2 max_uv;\n"
    "layout (location = 0) in vec2 in_pos;\n"
    "layout (location = 1) in vec2 in_tex_coord;\n"
    "out vec2 tex_coord;\n"
    "void main() {\n"
    "  tex_coord = mix(min_uv, max_uv, in_tex_coord);\n"
    "  gl_Position = to_camera_transform * vec4(in_pos, 0.0, 1.0);\n"
    "}\0";
  char* font_fragment_shader_source =
    "#version 330 core\n"
    "uniform sampler2D texture_image;\n"
    "uniform vec3 color;\n"
    "in vec2 tex_coord;\n"
    "out vec4 frag_color;\n"
    "void main() { \n"
    "  float alpha = texture(texture_image, tex_coord).r;\n"
    "  frag_color = vec4(color, alpha);\n"
    "}\0";
  DEBUG_ASSERT(RendererCompileShader(&r->font_shader, font_vertex_shader_source, font_fragment_shader_source));
  r->font_camera_uniform = g->glGetUniformLocation(r->font_shader, "to_camera_transform");
  r->font_min_uv_uniform = g->glGetUniformLocation(r->font_shader, "min_uv");
  r->font_max_uv_uniform = g->glGetUniformLocation(r->font_shader, "max_uv");
  r->font_color_uniform  = g->glGetUniformLocation(r->font_shader, "color");

  char* font_sdf_vertex_shader_source =
    "#version 330 core\n"
    "uniform mat4 to_camera_transform;\n"
    "uniform vec2 min_uv;\n"
    "uniform vec2 max_uv;\n"
    "layout (location = 0) in vec2 in_pos;\n"
    "layout (location = 1) in vec2 in_tex_coord;\n"
    "out vec2 tex_coord;\n"
    "void main() {\n"
    "  tex_coord = mix(min_uv, max_uv, in_tex_coord);\n"
    "  gl_Position = to_camera_transform * vec4(in_pos, 0.0, 1.0);\n"
    "}\0";
  char* font_sdf_fragment_shader_source =
    "#version 330 core\n"
    "uniform sampler2D texture_image;\n"
    "uniform vec3 color;\n"
    "uniform float threshold;\n"
    "uniform float smoothing;\n"
    "in vec2 tex_coord;\n"
    "out vec4 frag_color;\n"
    "void main() { \n"
    "  float distance = texture(texture_image, tex_coord).r;\n"
    "  float alpha    = smoothstep(threshold - smoothing, threshold + smoothing, distance);\n"
    "  frag_color     = vec4(color, alpha);\n"
    "}\0";
  DEBUG_ASSERT(RendererCompileShader(&r->font_sdf_shader, font_sdf_vertex_shader_source, font_sdf_fragment_shader_source));
  r->font_sdf_camera_uniform = g->glGetUniformLocation(r->font_sdf_shader, "to_camera_transform");
  r->font_sdf_threshold_uniform = g->glGetUniformLocation(r->font_sdf_shader, "threshold");
  r->font_sdf_smoothing_uniform = g->glGetUniformLocation(r->font_sdf_shader, "smoothing");
  r->font_sdf_min_uv_uniform = g->glGetUniformLocation(r->font_sdf_shader, "min_uv");
  r->font_sdf_max_uv_uniform = g->glGetUniformLocation(r->font_sdf_shader, "max_uv");
  r->font_sdf_color_uniform  = g->glGetUniformLocation(r->font_sdf_shader, "color");

  char* mesh_vertex_shader_source =
    "#version 330 core\n"
    "uniform mat4 to_camera_transform;\n"
    "layout (location = 0) in vec3 in_pos;\n"
    "layout (location = 1) in vec3 in_normal;\n"
    "layout (location = 2) in vec2 in_tex_coord;\n"
    "out vec3 normal;\n"
    "out vec2 tex_coord;\n"
    "void main() {\n"
    "  gl_Position = to_camera_transform * vec4(in_pos, 1.0);\n"
    "  normal = in_normal;\n"
    "  tex_coord = in_tex_coord;\n"
    "}\0";
  char* mesh_fragment_shader_source =
    "#version 330 core\n"
    "uniform sampler2D texture_image;\n"
    "uniform vec3 color;\n"
    "uniform float tex_color_ratio;\n"
    "in vec3 normal;\n"
    "in vec2 tex_coord;\n"
    "out vec4 frag_color;\n"
    "void main() {\n"
    "  vec4 tex_color = texture(texture_image, tex_coord);\n"
    "  vec4 in_color  = vec4(color, 1.0);\n"
    "  frag_color = mix(tex_color, in_color, tex_color_ratio);\n"
    "}\0";
  DEBUG_ASSERT(RendererCompileShader(&r->mesh_shader, mesh_vertex_shader_source, mesh_fragment_shader_source));
  r->mesh_camera_uniform = g->glGetUniformLocation(r->mesh_shader, "to_camera_transform");
  r->mesh_texture_uniform = g->glGetUniformLocation(r->mesh_shader, "texture_image");
  r->mesh_color_uniform = g->glGetUniformLocation(r->mesh_shader, "color");
  r->mesh_tex_color_ratio_uniform = g->glGetUniformLocation(r->mesh_shader, "tex_color_ratio");

  char* line_vertex_shader_source =
    "#version 330 core\n"
    "uniform mat4 to_camera_transform;\n"
    "layout (location = 0) in vec3 in_pos;\n"
    "void main() {\n"
    "  gl_Position = to_camera_transform * vec4(in_pos, 1.0);\n"
    "}\0";
  char* line_fragment_shader_source =
    "#version 330 core\n"
    "uniform vec3 color;\n"
    "out vec4 frag_color;\n"
    "void main() {\n"
    "  frag_color = vec4(color, 1.0);\n"
    "}\0";
  DEBUG_ASSERT(RendererCompileShader(&r->line_shader, line_vertex_shader_source, line_fragment_shader_source));
  r->line_camera_uniform = g->glGetUniformLocation(r->line_shader, "to_camera_transform");
  r->line_color_uniform = g->glGetUniformLocation(r->line_shader, "color");

  F32 quad_vertices[6][4] = {
    {-0.5f, +0.5f, 0, 1 },
    {-0.5f, -0.5f, 0, 0 },
    {+0.5f, -0.5f, 1, 0 },
    {-0.5f, +0.5f, 0, 1 },
    {+0.5f, -0.5f, 1, 0 },
    {+0.5f, +0.5f, 1, 1 },
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

  S32 width, height;
  WindowGetDims(NULL, NULL, &width, &height);
  M4 projection_2d = M4Orthographic(0, width, 0, height, 0.01f, 100.0f);
  RendererSetProjection2D(projection_2d);

  M4 projection_3d = M4Perspective(F32_PI / 4.0f, 16.0f / 9.0f, 0.01f, 100.0f);
  RendererSetProjection3D(projection_3d);
  r->camera_3d.pos      = (V3) {0, 0, 0 };
  r->camera_3d.look_dir = V3_Z_NEG;
  r->camera_3d.up_dir   = V3_Y_POS;

  r->mesh_pool = ArenaAllocate();
  V3 icosphere_points[12] = {
    (V3){-0.5257311f,  0.8506508f,  0.0000000f},
    (V3){ 0.5257311f,  0.8506508f,  0.0000000f},
    (V3){-0.5257311f, -0.8506508f,  0.0000000f},
    (V3){ 0.5257311f, -0.8506508f,  0.0000000f},
    (V3){ 0.0000000f, -0.5257311f,  0.8506508f},
    (V3){ 0.0000000f,  0.5257311f,  0.8506508f},
    (V3){ 0.0000000f, -0.5257311f, -0.8506508f},
    (V3){ 0.0000000f,  0.5257311f, -0.8506508f},
    (V3){ 0.8506508f,  0.0000000f, -0.5257311f},
    (V3){ 0.8506508f,  0.0000000f,  0.5257311f},
    (V3){-0.8506508f,  0.0000000f, -0.5257311f},
    (V3){-0.8506508f,  0.0000000f,  0.5257311f},
  };
  V3 icosphere_norms[12] = {
    (V3){-0.5257311f,  0.8506508f,  0.0000000f},
    (V3){ 0.5257311f,  0.8506508f,  0.0000000f},
    (V3){-0.5257311f, -0.8506508f,  0.0000000f},
    (V3){ 0.5257311f, -0.8506508f,  0.0000000f},
    (V3){ 0.0000000f, -0.5257311f,  0.8506508f},
    (V3){ 0.0000000f,  0.5257311f,  0.8506508f},
    (V3){ 0.0000000f, -0.5257311f, -0.8506508f},
    (V3){ 0.0000000f,  0.5257311f, -0.8506508f},
    (V3){ 0.8506508f,  0.0000000f, -0.5257311f},
    (V3){ 0.8506508f,  0.0000000f,  0.5257311f},
    (V3){-0.8506508f,  0.0000000f, -0.5257311f},
    (V3){-0.8506508f,  0.0000000f,  0.5257311f},
  };
  V2 icosphere_uvs[12] = {
    (V2){1.0000f, 0.8230f},
    (V2){0.5000f, 0.8230f},
    (V2){1.0000f, 0.1770f},
    (V2){0.5000f, 0.1770f},
    (V2){0.7500f, 0.3237f},
    (V2){0.7500f, 0.6763f},
    (V2){0.2500f, 0.3237f},
    (V2){0.2500f, 0.6763f},
    (V2){0.4115f, 0.5000f},
    (V2){0.5885f, 0.5000f},
    (V2){0.0885f, 0.5000f},
    (V2){0.9115f, 0.5000f},
  };
  U32 icosphere_indices[60] = {
    0, 11, 5, 0, 5, 1,  0, 1, 7, 0, 7, 10, 0, 10, 11,
    1, 5, 9, 5, 11, 4, 11, 10, 2, 10, 7, 6, 7, 1, 8,
    3, 9, 4, 3, 4, 2,  3, 2, 6, 3, 6, 8,  3, 8, 9,
    4, 9, 5, 2, 4, 11, 6, 2, 10, 8, 6, 7,  9, 8, 1,
  };
  RendererRegisterMesh(&r->icosphere_handle, 0, (V3*) &icosphere_points, (V3*) &icosphere_norms, (V2*) &icosphere_uvs, 12, (U32*) &icosphere_indices, 60);

  V3 cube_points[24] = {
    (V3){ 0.5f, -0.5f, -0.5f },
    (V3){ 0.5f,  0.5f, -0.5f },
    (V3){ 0.5f,  0.5f,  0.5f },
    (V3){ 0.5f, -0.5f,  0.5f },
    (V3){ -0.5f, -0.5f,  0.5f },
    (V3){ -0.5f,  0.5f,  0.5f },
    (V3){ -0.5f,  0.5f, -0.5f },
    (V3){ -0.5f, -0.5f, -0.5f },
    (V3){ -0.5f, 0.5f, -0.5f },
    (V3){ -0.5f, 0.5f,  0.5f },
    (V3){  0.5f, 0.5f,  0.5f },
    (V3){  0.5f, 0.5f, -0.5f },
    (V3){ -0.5f, -0.5f,  0.5f },
    (V3){ -0.5f, -0.5f, -0.5f },
    (V3){  0.5f, -0.5f, -0.5f },
    (V3){  0.5f, -0.5f,  0.5f },
    (V3){ -0.5f, -0.5f, 0.5f },
    (V3){  0.5f, -0.5f, 0.5f },
    (V3){  0.5f,  0.5f, 0.5f },
    (V3){ -0.5f,  0.5f, 0.5f },
    (V3){  0.5f, -0.5f, -0.5f },
    (V3){ -0.5f, -0.5f, -0.5f },
    (V3){ -0.5f,  0.5f, -0.5f },
    (V3){  0.5f,  0.5f, -0.5f },
  };
  V3 cube_norms[24] = {
    (V3){ 1,0,0 }, (V3){ 1,0,0 }, (V3){ 1,0,0 }, (V3){ 1,0,0 },
    (V3){ -1,0,0 }, (V3){ -1,0,0 }, (V3){ -1,0,0 }, (V3){ -1,0,0 },
    (V3){ 0,1,0 }, (V3){ 0,1,0 }, (V3){ 0,1,0 }, (V3){ 0,1,0 },
    (V3){ 0,-1,0 }, (V3){ 0,-1,0 }, (V3){ 0,-1,0 }, (V3){ 0,-1,0 },
    (V3){ 0,0,1 }, (V3){ 0,0,1 }, (V3){ 0,0,1 }, (V3){ 0,0,1 },
    (V3){ 0,0,-1 }, (V3){ 0,0,-1 }, (V3){ 0,0,-1 }, (V3){ 0,0,-1 },
  };
  V2 cube_uvs[24] = {
    (V2){0,0}, (V2){1,0}, (V2){1,1}, (V2){0,1},
    (V2){0,0}, (V2){1,0}, (V2){1,1}, (V2){0,1},
    (V2){0,0}, (V2){1,0}, (V2){1,1}, (V2){0,1},
    (V2){0,0}, (V2){1,0}, (V2){1,1}, (V2){0,1},
    (V2){0,0}, (V2){1,0}, (V2){1,1}, (V2){0,1},
    (V2){0,0}, (V2){1,0}, (V2){1,1}, (V2){0,1},
  };
  U32 cube_indices[36] = {
    0,1,2,     0,2,3,
    4,5,6,     4,6,7,
    8,9,10,    8,10,11,
    12,13,14,  12,14,15,
    16,17,18,  16,18,19,
    20,21,22,  20,22,23
  };
  RendererRegisterMesh(&r->cube_handle, 0, (V3*) &cube_points, (V3*) &cube_norms, (V2*) &cube_uvs, 24, (U32*) &cube_indices, 36);

  RendererEnableDepthTest();
  g->glLineWidth(3);

  LOG_INFO("[RENDER] OpenGL renderer initialized.");
  success = true;

  return success;
}

static void RendererDeinit() {
  Renderer* r = &_renderer;
  ArenaRelease(r->mesh_pool);
}

void RendererSetProjection2D(M4 projection_2d) {
  Renderer* r = &_renderer;
  // NOTE: 2d camera is fixed, so we can just store the final world to camera matrix instead of both and recalculating per-frame.
  V3 pos    = { 0, 0, 1 }; // NOTE: seat camera Z back so items can exist at z = 0.
  V3 target = V3_Z_NEG;
  V3 up     = V3_Y_POS;
  M4 camera = M4LookAt(&pos, &target, &up);
  M4MultM4(&r->world_to_camera_2d, &projection_2d, &camera);
}

void RendererSetProjection3D(M4 projection_3d) {
  Renderer* r = &_renderer;
  r->projection_3d = projection_3d;
}

Camera* RendererCamera3D() {
  Renderer* r = &_renderer;
  return &r->camera_3d;
}

void RendererRegisterImage(U32* image_handle, Image* image) {
  OpenGLAPI* g = &_ogl;
  g->glGenTextures(1, image_handle);
  g->glBindTexture(GL_TEXTURE_2D, *image_handle);
  switch (image->format) {
    case ImageFormat_RGBA: {
      g->glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
      g->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->data);
    } break;
    case ImageFormat_RGB: {
      g->glPixelStorei(GL_UNPACK_ALIGNMENT, 3);
      g->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data);
    } break;
    case ImageFormat_R: {
      g->glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      g->glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, image->width, image->height, 0, GL_RED, GL_UNSIGNED_BYTE, image->data);
    } break;
    default: {
      UNIMPLEMENTED();
    } break;
  }
  g->glBindTexture(GL_TEXTURE_2D, 0);
}

void RendererReleaseImage(U32 image_handle) {
  OpenGLAPI* g = &_ogl;
  g->glDeleteTextures(1, &image_handle);
}

void RendererRegisterMesh(U32* mesh_handle, U32 image_handle, V3* points, V3* normals, V2* uvs, U32 vertices_size, U32* indices, U32 indices_size) {
  Renderer* r = &_renderer;
  OpenGLAPI* g = &_ogl;

  DEBUG_ASSERT(points != NULL);
  U32 points_size  = vertices_size;
  U32 normals_size = vertices_size * (normals != NULL);
  U32 uvs_size     = vertices_size * (uvs != NULL);

  GLuint vbo;
  g->glGenBuffers(1, &vbo);
  g->glBindBuffer(GL_ARRAY_BUFFER, vbo);
  g->glBufferData(GL_ARRAY_BUFFER, (points_size * sizeof(V3)) + (normals_size * sizeof(V3)) + (uvs_size * sizeof(V2)), NULL, GL_STATIC_DRAW);

  GLuint vao;
  g->glGenVertexArrays(1, &vao);
  g->glBindVertexArray(vao);
  U64 offset = 0;

  g->glEnableVertexAttribArray(0);
  g->glBufferSubData(GL_ARRAY_BUFFER, offset, points_size * sizeof(V3), points);
  g->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(V3), (void*) offset);
  offset += points_size * sizeof(V3);

  g->glEnableVertexAttribArray(1);
  g->glBufferSubData(GL_ARRAY_BUFFER, offset, normals_size * sizeof(V3), normals);
  g->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(V3), (void*) offset);
  offset += normals_size * sizeof(V3);

  g->glEnableVertexAttribArray(2);
  g->glBufferSubData(GL_ARRAY_BUFFER, offset, uvs_size * sizeof(V2), uvs);
  g->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(V2), (void*) offset);
  offset += uvs_size * sizeof(V2);

  GLuint ibo;
  g->glGenBuffers(1, &ibo);
  g->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  g->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size * sizeof(U32), indices, GL_STATIC_DRAW);

  g->glBindBuffer(GL_ARRAY_BUFFER, 0);
  g->glBindVertexArray(0);
  g->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  RenderMesh* mesh;
  if (r->meshes_free_list != NULL) {
    mesh = r->meshes_free_list;
    SLL_STACK_POP(r->meshes_free_list, next);
  } else {
    mesh = ARENA_PUSH_STRUCT(r->mesh_pool, RenderMesh);
  }
  MEMORY_ZERO_STRUCT(mesh);
  DLL_PUSH_BACK(r->meshes_head, r->meshes_tail, mesh, prev, next);
  *mesh_handle = r->next_mesh_id++;
  mesh->id = *mesh_handle;
  mesh->vao = vao;
  mesh->vbo = vbo;
  mesh->ibo = ibo;
  mesh->indices_size = indices_size;
  mesh->image_handle = image_handle;
}

void RendererReleaseMesh(U32 mesh_handle) {
  Renderer* r = &_renderer;
  OpenGLAPI* g = &_ogl;
  RenderMesh* mesh = RendererFindMesh(mesh_handle);
  DEBUG_ASSERT(mesh != NULL);
  // if (mesh == NULL) { return; }
  g->glDeleteVertexArrays(1, &mesh->vao);
  g->glDeleteBuffers(1, &mesh->vbo);
  g->glDeleteBuffers(1, &mesh->ibo);
  DLL_REMOVE(r->meshes_head, r->meshes_tail, mesh, prev, next);
  SLL_STACK_PUSH(r->meshes_free_list, mesh, next);
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

void RendererEnableWireframe(void) {
  OpenGLAPI* g = &_ogl;
  g->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void RendererDisableWireframe(void) {
  OpenGLAPI* g = &_ogl;
  g->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
  M4Invert(&world_to_camera_inv, &r->world_to_camera_2d);
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

void DrawLine2(F32 start_x, F32 start_y, F32 end_x, F32 end_y, F32 thickness, F32 red, F32 green, F32 blue) {
  V2 start = { start_x, start_y };
  V2 end   = { end_x, end_y };
  V2 delta;
  V2SubV2(&delta, &end, &start);
  F32 theta = F32ArcTan2(delta.y, delta.x);
  F32 width = V2Length(&delta);
  DrawRoundedRectangleRot(start.x + delta.x / 2, start.y + delta.y / 2, width, thickness, thickness / 2.0f, theta, red, green, blue);
}

void DrawLineV(V2 start, V2 end, F32 thickness, V3 color) {
  DrawLine2(start.x, start.y, end.x, end.y, thickness, color.r, color.g, color.b);
}

void DrawQuadBezier(F32 start_x, F32 start_y, F32 control_x, F32 control_y, F32 end_x, F32 end_y, U32 resolution, F32 thickness, F32 red, F32 green, F32 blue) {
  F32 x_prev = start_x;
  F32 y_prev = start_y;
  for (U32 i = 0; i <= resolution; i++) {
    F32 t = ((F32) i) / ((F32) resolution);
    F32 x_1 = F32Lerp(start_x, control_x, t);
    F32 y_1 = F32Lerp(start_y, control_y, t);
    F32 x_2 = F32Lerp(control_x, end_x, t);
    F32 y_2 = F32Lerp(control_y, end_y, t);
    F32 x_3 = F32Lerp(x_1, x_2, t);
    F32 y_3 = F32Lerp(y_1, y_2, t);
    DrawLine2(x_prev, y_prev, x_3, y_3, thickness, red, green, blue);
    x_prev = x_3;
    y_prev = y_3;
  }
}

void DrawQuadBezierV(V2 start, V2 control, V2 end, U32 resolution, F32 thickness, V3 color) {
  DrawQuadBezier(start.x, start.y, control.x, control.y, end.x, end.y, resolution, thickness, color.r, color.g, color.b);
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
  rect_to_world = M4FromTransform(&pos, &rot, &scale);
  M4MultM4(&rect_to_camera, &r->world_to_camera_2d, &rect_to_world);
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
  frame_to_world = M4FromTransform(&pos, &rot, &scale);
  M4MultM4(&frame_to_camera, &r->world_to_camera_2d, &frame_to_world);
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
  tri_to_world = M4FromTransform(&pos, &rot, &scale);
  M4MultM4(&tri_to_camera, &r->world_to_camera_2d, &tri_to_world);
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
  DrawSubImageRot(image_handle, center_x, center_y, width, height, angle_rad, 0, 0, 1, 1);
}

void DrawImageRotV(U32 image_handle, V2 pos, V2 size, F32 angle_rad) {
  DrawImageRot(image_handle, pos.x, pos.y, size.x, size.y, angle_rad);
}

void DrawSubImage(U32 image_handle, F32 center_x, F32 center_y, F32 width, F32 height, F32 min_uv_x, F32 min_uv_y, F32 max_uv_x, F32 max_uv_y) {
  DrawSubImageRot(image_handle, center_x, center_y, width, height, 0, min_uv_x, min_uv_y, max_uv_x, max_uv_y);
}

void DrawSubImageV(U32 image_handle, V2 center, V2 size, V2 min_uv, V2 max_uv) {
  DrawSubImage(image_handle, center.x, center.y, size.x, size.y, min_uv.x, min_uv.y, max_uv.x, max_uv.y);
}

void DrawSubImageRot(U32 image_handle, F32 center_x, F32 center_y, F32 width, F32 height, F32 angle_rad, F32 min_uv_x, F32 min_uv_y, F32 max_uv_x, F32 max_uv_y) {
  Renderer* r = &_renderer;
  OpenGLAPI* g = &_ogl;

  V2 min_uv = { min_uv_x, min_uv_y };
  V2 max_uv = { max_uv_x, max_uv_y };
  V3 pos    = { center_x, center_y, 0 };
  V3 scale  = { width, height, 1 };
  V4 rot;
  V4RotateAroundAxis(&rot, &V3_Z_POS, angle_rad);
  M4 image_to_world, image_to_camera, image_to_camera_t;
  image_to_world = M4FromTransform(&pos, &rot, &scale);
  M4MultM4(&image_to_camera, &r->world_to_camera_2d, &image_to_world);
  M4Transpose(&image_to_camera_t, &image_to_camera);

  g->glUseProgram(r->image_shader);
  g->glUniformMatrix4fv(r->image_camera_uniform, 1, GL_FALSE, (GLfloat*) &image_to_camera_t);
  g->glUniform2fv(r->image_min_uv_uniform, 1, (GLfloat*) &min_uv);
  g->glUniform2fv(r->image_max_uv_uniform, 1, (GLfloat*) &max_uv);
  g->glBindTexture(GL_TEXTURE_2D, image_handle);
  g->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  g->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  g->glBindVertexArray(r->quad_vao);
  g->glDrawArrays(GL_TRIANGLES, 0, 6);
  g->glBindTexture(GL_TEXTURE_2D, 0);
  g->glBindVertexArray(0);
  g->glUseProgram(0);
}

void DrawSubImageRotV(U32 image_handle, V2 center, V2 size, F32 angle_rad, V2 min_uv, V2 max_uv) {
  DrawSubImageRot(image_handle, center.x, center.y, size.x, size.y, angle_rad, min_uv.x, min_uv.y, max_uv.x, max_uv.y);
}

void DrawFontBmpCharacter(U32 image_handle, F32 center_x, F32 center_y, F32 width, F32 height, F32 min_uv_x, F32 min_uv_y, F32 max_uv_x, F32 max_uv_y, F32 red, F32 green, F32 blue) {
  Renderer* r = &_renderer;
  OpenGLAPI* g = &_ogl;

  V3 color  = { red, green, blue };
  V2 min_uv = { min_uv_x, min_uv_y };
  V2 max_uv = { max_uv_x, max_uv_y };
  V3 pos    = { center_x, center_y, 0 };
  V3 scale  = { width, height, 1 };
  V4 rot;
  V4RotateAroundAxis(&rot, &V3_Z_POS, 0);
  M4 image_to_world, image_to_camera, image_to_camera_t;
  image_to_world = M4FromTransform(&pos, &rot, &scale);
  M4MultM4(&image_to_camera, &r->world_to_camera_2d, &image_to_world);
  M4Transpose(&image_to_camera_t, &image_to_camera);

  g->glUseProgram(r->font_shader);
  g->glUniformMatrix4fv(r->font_camera_uniform, 1, GL_FALSE, (GLfloat*) &image_to_camera_t);
  g->glUniform2fv(r->font_min_uv_uniform, 1, (GLfloat*) &min_uv);
  g->glUniform2fv(r->font_max_uv_uniform, 1, (GLfloat*) &max_uv);
  g->glUniform3fv(r->font_color_uniform, 1, (GLfloat*) &color);
  g->glBindTexture(GL_TEXTURE_2D, image_handle);
  g->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  g->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  g->glBindVertexArray(r->quad_vao);
  g->glDrawArrays(GL_TRIANGLES, 0, 6);
  g->glBindTexture(GL_TEXTURE_2D, 0);
  g->glBindVertexArray(0);
  g->glUseProgram(0);
}

void DrawFontBmpCharacterV(U32 image_handle, V2 center, V2 size, V2 min_uv, V2 max_uv, V3 color) {
  DrawFontBmpCharacter(image_handle, center.x, center.y, size.x, size.y, min_uv.x, min_uv.y, max_uv.x, max_uv.y, color.r, color.g, color.b);
}

void DrawFontSdfCharacter(U32 image_handle, F32 center_x, F32 center_y, F32 width, F32 height, F32 min_uv_x, F32 min_uv_y, F32 max_uv_x, F32 max_uv_y, F32 threshold, F32 smoothing, F32 red, F32 green, F32 blue) {
  Renderer* r = &_renderer;
  OpenGLAPI* g = &_ogl;

  threshold = CLAMP(0.0f, threshold, 1.0f);
  smoothing = CLAMP(0.0f, smoothing, 1.0f);
  if (threshold == 0) { threshold = 0.5f; }
  if (smoothing == 0) { smoothing = 0.15f; }

  V3 color  = { red, green, blue };
  V2 min_uv = { min_uv_x, min_uv_y };
  V2 max_uv = { max_uv_x, max_uv_y };
  V3 pos    = { center_x, center_y, 0 };
  V3 scale  = { width, height, 1 };
  V4 rot;
  V4RotateAroundAxis(&rot, &V3_Z_POS, 0);
  M4 image_to_world, image_to_camera, image_to_camera_t;
  image_to_world = M4FromTransform(&pos, &rot, &scale);
  M4MultM4(&image_to_camera, &r->world_to_camera_2d, &image_to_world);
  M4Transpose(&image_to_camera_t, &image_to_camera);

  g->glUseProgram(r->font_sdf_shader);
  g->glUniformMatrix4fv(r->font_sdf_camera_uniform, 1, GL_FALSE, (GLfloat*) &image_to_camera_t);
  g->glUniform1f(r->font_sdf_threshold_uniform, threshold);
  g->glUniform1f(r->font_sdf_smoothing_uniform, smoothing);
  g->glUniform2fv(r->font_sdf_min_uv_uniform, 1, (GLfloat*) &min_uv);
  g->glUniform2fv(r->font_sdf_max_uv_uniform, 1, (GLfloat*) &max_uv);
  g->glUniform3fv(r->font_sdf_color_uniform, 1, (GLfloat*) &color);
  g->glBindTexture(GL_TEXTURE_2D, image_handle);
  g->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  g->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  g->glBindVertexArray(r->quad_vao);
  g->glDrawArrays(GL_TRIANGLES, 0, 6);
  g->glBindTexture(GL_TEXTURE_2D, 0);
  g->glBindVertexArray(0);
  g->glUseProgram(0);
}

void DrawFontSdfCharacterV(U32 image_handle, V2 center, V2 size, V2 min_uv, V2 max_uv, F32 threshold, F32 smoothing, V3 color) {
  DrawFontSdfCharacter(image_handle, center.x, center.y, size.x, size.y, min_uv.x, min_uv.y, max_uv.x, max_uv.y, threshold, smoothing, color.r, color.g, color.b);
}

void DrawStringBmp(String8 str, FontAtlas* atlas, U32 atlas_handle, F32 font_height, F32 x, F32 y) {
  V2 pos = V2Assign(x, y);
  DrawStringBmpV(str, atlas, atlas_handle, font_height, pos);
}

void DrawStringBmpV(String8 str, FontAtlas* atlas, U32 atlas_handle, F32 font_height, V2 pos) {
  for (S32 i = 0; i < str.size; i++) {
    U8 curr = str.str[i];
    U8 next = i < str.size - 1 ? str.str[i + 1] : 0;
    V2 center, size, min_uv, max_uv;
    DEBUG_ASSERT(FontAtlasPlace(atlas, curr, next, font_height, &pos, &center, &size, &min_uv, &max_uv));
    DrawFontBmpCharacterV(atlas_handle, center, size, min_uv, max_uv, V3_BLACK);
  }
}

void DrawStringSdf(String8 str, FontAtlas* atlas, U32 atlas_handle, F32 font_height, F32 x, F32 y) {
  DrawStringSdfEx(str, atlas, atlas_handle, font_height, 0, 0, x, y);
}

void DrawStringSdfV(String8 str, FontAtlas* atlas, U32 atlas_handle, F32 font_height, V2 pos) {
  DrawStringSdfExV(str, atlas, atlas_handle, font_height, 0, 0, pos);
}

void DrawStringSdfEx(String8 str, FontAtlas* atlas, U32 atlas_handle, F32 font_height, F32 threshold, F32 smoothing, F32 x, F32 y) {
  V2 pos = V2Assign(x, y);
  DrawStringSdfExV(str, atlas, atlas_handle, font_height, threshold, smoothing, pos);
}

void DrawStringSdfExV(String8 str, FontAtlas* atlas, U32 atlas_handle, F32 font_height, F32 threshold, F32 smoothing, V2 pos) {
  for (S32 i = 0; i < str.size; i++) {
    U8 curr = str.str[i];
    U8 next = i < str.size - 1 ? str.str[i + 1] : 0;
    V2 center, size, min_uv, max_uv;
    DEBUG_ASSERT(FontAtlasPlace(atlas, curr, next, font_height, &pos, &center, &size, &min_uv, &max_uv));
    DrawFontSdfCharacterV(atlas_handle, center, size, min_uv, max_uv, threshold, smoothing, V3_BLACK);
  }
}

void DrawLine3(F32 start_x, F32 start_y, F32 start_z, F32 end_x, F32 end_y, F32 end_z, F32 color_r, F32 color_g, F32 color_b) {
  DrawLine3V(V3Assign(start_x, start_y, start_z), V3Assign(end_x, end_y, end_z), V3Assign(color_r, color_g, color_b));
}

// TODO: add thickness? sdf w/ 3d rect, similar to 2d api. remove reliance on glLineWidth.
void DrawLine3V(V3 start, V3 end, V3 color) {
  Renderer* r = &_renderer;
  OpenGLAPI* g = &_ogl;

  V3 vertices[2] = { start, end };
  GLuint line_vao, line_vbo;
  g->glGenVertexArrays(1, &line_vao);
  g->glGenBuffers(1, &line_vbo);
  g->glBindVertexArray(line_vao);
  g->glBindBuffer(GL_ARRAY_BUFFER, line_vbo);
  g->glBufferData(GL_ARRAY_BUFFER, sizeof(F32) * 6, vertices, GL_STATIC_DRAW);
  g->glEnableVertexAttribArray(0);
  g->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(F32), (void*)(sizeof(F32) * 0));

  V3 camera_target;
  V3AddV3(&camera_target, &r->camera_3d.pos, &r->camera_3d.look_dir);
  M4 camera, world_to_camera, mesh_to_world, mesh_to_camera, mesh_to_camera_t;
  camera = M4LookAt(&r->camera_3d.pos, &camera_target, &r->camera_3d.up_dir);
  M4MultM4(&world_to_camera, &r->projection_3d, &camera);
  mesh_to_world = M4FromTransform(&V3_ZEROES, &V4_QUAT_IDENT, &V3_ONES);
  M4MultM4(&mesh_to_camera, &world_to_camera, &mesh_to_world);
  M4Transpose(&mesh_to_camera_t, &mesh_to_camera);

  g->glUseProgram(r->line_shader);
  g->glUniformMatrix4fv(r->line_camera_uniform, 1, GL_FALSE, (GLfloat*) &mesh_to_camera_t);
  g->glUniform3fv(r->line_color_uniform, 1, (GLfloat*) &color);
  g->glBindVertexArray(line_vao);
  g->glDrawArrays(GL_LINES, 0, 2);

  g->glBindVertexArray(0);
  g->glUseProgram(0);
  g->glDeleteVertexArrays(1, &line_vao);
  g->glDeleteBuffers(1, &line_vbo);
}

void DrawSphere(F32 center_x, F32 center_y, F32 center_z, F32 rot_x, F32 rot_y, F32 rot_z, F32 rot_w, F32 radius, F32 red, F32 green, F32 blue) {
  Renderer* r = &_renderer;
  DrawMeshColor(r->icosphere_handle, center_x, center_y, center_z, rot_x, rot_y, rot_z, rot_w, radius, radius, radius, red, green, blue);
}

void DrawSphereV(V3 center, V4 rot, F32 radius, V3 color) {
  DrawSphere(center.x, center.y, center.z, rot.x, rot.y, rot.z, rot.w, radius, color.r, color.g, color.b);
}

void DrawCube(F32 center_x, F32 center_y, F32 center_z, F32 rot_x, F32 rot_y, F32 rot_z, F32 rot_w, F32 size_x, F32 size_y, F32 size_z, F32 red, F32 green, F32 blue) {
  Renderer* r = &_renderer;
  DrawMeshColor(r->cube_handle, center_x, center_y, center_z, rot_x, rot_y, rot_z, rot_w, size_x, size_y, size_z, red, green, blue);
}

void DrawCubeV(V3 center, V4 rot, V3 size, V3 color) {
  DrawCube(center.x, center.y, center.z, rot.x, rot.y, rot.z, rot.w, size.x, size.y, size.z, color.r, color.g, color.b);
}

void DrawTetrahedron(F32 x1, F32 y1, F32 z1, F32 x2, F32 y2, F32 z2, F32 x3, F32 y3, F32 z3, F32 x4, F32 y4, F32 z4, F32 color_r, F32 color_g, F32 color_b) {
  DrawTetrahedronV(V3Assign(x1, y1, z1), V3Assign(x2, y2, z2), V3Assign(x3, y3, z3), V3Assign(x4, y4, z4), V3Assign(color_r, color_g, color_b));
}

void DrawTetrahedronV(V3 p1, V3 p2, V3 p3, V3 p4, V3 color) {
  V3 v[4] = {p1, p2, p3, p4};
  // NOTE: 4th vertex is explicitly *not* on the face, required for determining face winding order.
  U32 faces[4][4] = {
    {0, 1, 2, 3},
    {0, 2, 3, 1},
    {0, 3, 1, 2},
    {1, 3, 2, 0},
  };
  V3  points[12];
  V3  norms[12];
  V2  uvs[12];
  U32 indices[12];
  for (U32 i = 0; i < 4; i++) {
    U32 a = faces[i][0];
    U32 b = faces[i][1];
    U32 c = faces[i][2];
    U32 d = faces[i][3];

    V3 ab, ac, ad, cross;
    V3SubV3(&ab, &v[b], &v[a]);
    V3SubV3(&ac, &v[c], &v[a]);
    V3SubV3(&ad, &v[d], &v[a]);
    V3CrossV3(&cross, &ab, &ac);
    // NOTE: if norm is pointing at 4th vertex, winding order is flipped
    if (V3DotV3(&cross, &ad) > 0) {
      SWAP(U32, b, c);
      V3SubV3(&ab, &v[b], &v[a]);
      V3SubV3(&ac, &v[c], &v[a]);
      V3CrossV3(&cross, &ab, &ac);
    }
    if (V3LengthSq(&cross) == 0) { return; }
    V3Normalize(&cross, &cross);

    U32 idx = i * 3;
    points[idx + 0] = v[a];
    points[idx + 1] = v[b];
    points[idx + 2] = v[c];
    norms[idx + 0] = cross;
    norms[idx + 1] = cross;
    norms[idx + 2] = cross;
    uvs[idx + 0] = V2Assign(0, 0);
    uvs[idx + 1] = V2Assign(1, 0);
    uvs[idx + 2] = V2Assign(0.5f, 1);
    indices[idx + 0] = idx + 0;
    indices[idx + 1] = idx + 1;
    indices[idx + 2] = idx + 2;
  }

  U32 tetrahedron_handle;
  RendererRegisterMesh(&tetrahedron_handle, 0, (V3*) &points, (V3*) &norms, (V2*) &uvs, 12, indices, 12);
  // NOTE: verts already in world space, so don't need to do additional transformation.
  DrawMeshExV(tetrahedron_handle, V3_ZEROES, V4_QUAT_IDENT, V3_ONES, color, 1);
  RendererReleaseMesh(tetrahedron_handle);
}

void DrawMesh(U32 mesh_handle, F32 pos_x, F32 pos_y, F32 pos_z, F32 rot_x, F32 rot_y, F32 rot_z, F32 rot_w, F32 scale_x, F32 scale_y, F32 scale_z) {
  DrawMeshEx(mesh_handle, pos_x, pos_y, pos_z, rot_x, rot_y, rot_z, rot_w, scale_x, scale_y, scale_z, 0, 0, 0, 0);
}

void DrawMeshV(U32 mesh_handle, V3 pos, V4 rot, V3 scale) {
  DrawMeshExV(mesh_handle, pos, rot, scale, V3Assign(0, 0, 0), 0);
}

void DrawMeshColor(U32 mesh_handle, F32 pos_x, F32 pos_y, F32 pos_z, F32 rot_x, F32 rot_y, F32 rot_z, F32 rot_w, F32 scale_x, F32 scale_y, F32 scale_z, F32 color_r, F32 color_g, F32 color_b) {
  DrawMeshEx(mesh_handle, pos_x, pos_y, pos_z, rot_x, rot_y, rot_z, rot_w, scale_x, scale_y, scale_z, color_r, color_g, color_b, 1);
}

void DrawMeshColorV(U32 mesh_handle, V3 pos, V4 rot, V3 scale, V3 color) {
  DrawMeshExV(mesh_handle, pos, rot, scale, color, 1);
}

void DrawMeshEx(U32 mesh_handle, F32 pos_x, F32 pos_y, F32 pos_z, F32 rot_x, F32 rot_y, F32 rot_z, F32 rot_w, F32 scale_x, F32 scale_y, F32 scale_z, F32 color_r, F32 color_g, F32 color_b, F32 tex_color_ratio) {
  DrawMeshExV(mesh_handle, V3Assign(pos_x, pos_y, pos_z), V4Assign(rot_x, rot_y, rot_z, rot_w), V3Assign(scale_x, scale_y, scale_z), V3Assign(color_r, color_g, color_b), tex_color_ratio);
}

void DrawMeshExV(U32 mesh_handle, V3 pos, V4 rot, V3 scale, V3 color, F32 tex_color_ratio) {
  Renderer* r = &_renderer;
  OpenGLAPI* g = &_ogl;
  RenderMesh* mesh = RendererFindMesh(mesh_handle);
  DEBUG_ASSERT(mesh != NULL);

  V3 camera_target;
  V3AddV3(&camera_target, &r->camera_3d.pos, &r->camera_3d.look_dir);
  M4 camera, world_to_camera, mesh_to_world, mesh_to_camera, mesh_to_camera_t;
  camera = M4LookAt(&r->camera_3d.pos, &camera_target, &r->camera_3d.up_dir);
  M4MultM4(&world_to_camera, &r->projection_3d, &camera);
  mesh_to_world = M4FromTransform(&pos, &rot, &scale);
  M4MultM4(&mesh_to_camera, &world_to_camera, &mesh_to_world);
  M4Transpose(&mesh_to_camera_t, &mesh_to_camera);

  g->glUseProgram(r->mesh_shader);
  g->glUniformMatrix4fv(r->mesh_camera_uniform, 1, GL_FALSE, (GLfloat*) &mesh_to_camera_t);
  g->glUniform3fv(r->mesh_color_uniform, 1, (GLfloat*) &color);
  g->glUniform1f(r->mesh_tex_color_ratio_uniform, tex_color_ratio);
  g->glBindTexture(GL_TEXTURE_2D, mesh->image_handle);
  g->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  g->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  g->glBindVertexArray(mesh->vao);
  g->glDrawElements(GL_TRIANGLES, mesh->indices_size, GL_UNSIGNED_INT, 0);

  g->glBindVertexArray(0);
  g->glBindTexture(GL_TEXTURE_2D, 0);
  g->glUseProgram(0);
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
  WIN_LINK_GL(glBufferSubData);
  WIN_LINK_GL(glBindVertexArray);
  WIN_LINK_GL(glGetUniformLocation);
  WIN_LINK_GL(glUniformMatrix4fv);
  WIN_LINK_GL(glUniform3fv);
  WIN_LINK_GL(glUniform2fv);
  WIN_LINK_GL(glUniform1f);
  WIN_LINK_GL(glDeleteBuffers);
  WIN_LINK_GL(glDeleteVertexArrays);
  WIN_LINK_GL(glActiveTexture);
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
  _ogl.glDrawElements = glDrawElements;
  _ogl.glPixelStorei = glPixelStorei;
  _ogl.glPolygonMode = glPolygonMode;
  _ogl.glLineWidth = glLineWidth;

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
  RendererDeinit();
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
