#define CDEFAULT_STD_IMPLEMENTATION
#include "../cdefault_std.h"
#define CDEFAULT_VIDEO_IMPLEMENTATION
#include "../cdefault_video.h"

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

int main(void) {
  glShaderSource_Fn* UNUSED(test) = (glShaderSource_Fn*) wglGetProcAddress("glShaderSource");

  WindowInitOpts opts;
  MEMORY_ZERO_STRUCT(&opts);
  opts.title = "video example";
  ASSERT(WindowInit(opts));

  // TODO: move this back into cdefault_video.
  glShaderSource_Fn* glShaderSource = (glShaderSource_Fn*) wglGetProcAddress("glShaderSource");
  glCompileShader_Fn* glCompileShader = (glCompileShader_Fn*) wglGetProcAddress("glCompileShader");
  glCreateProgram_Fn* glCreateProgram = (glCreateProgram_Fn*) wglGetProcAddress("glCreateProgram");
  glCreateShader_Fn* glCreateShader = (glCreateShader_Fn*) wglGetProcAddress("glCreateShader");
  glGetShaderiv_Fn* glGetShaderiv = (glGetShaderiv_Fn*) wglGetProcAddress("glGetShaderiv");
  glAttachShader_Fn* glAttachShader = (glAttachShader_Fn*) wglGetProcAddress("glAttachShader");
  glLinkProgram_Fn* glLinkProgram = (glLinkProgram_Fn*) wglGetProcAddress("glLinkProgram");
  glGetProgramiv_Fn* glGetProgramiv = (glGetProgramiv_Fn*) wglGetProcAddress("glGetProgramiv");
  glUseProgram_Fn* glUseProgram = (glUseProgram_Fn*) wglGetProcAddress("glUseProgram");
  glDeleteShader_Fn* glDeleteShader = (glDeleteShader_Fn*) wglGetProcAddress("glDeleteShader");
  glVertexAttribPointer_Fn* glVertexAttribPointer = (glVertexAttribPointer_Fn*) wglGetProcAddress("glVertexAttribPointer");
  glEnableVertexAttribArray_Fn* glEnableVertexAttribArray = (glEnableVertexAttribArray_Fn*) wglGetProcAddress("glEnableVertexAttribArray");
  glGenVertexArrays_Fn* glGenVertexArrays = (glGenVertexArrays_Fn*) wglGetProcAddress("glGenVertexArrays");
  glGenBuffers_Fn* glGenBuffers = (glGenBuffers_Fn*) wglGetProcAddress("glGenBuffers");
  glBindBuffer_Fn* glBindBuffer = (glBindBuffer_Fn*) wglGetProcAddress("glBindBuffer");
  glBufferData_Fn* glBufferData = (glBufferData_Fn*) wglGetProcAddress("glBufferData");
  glBindVertexArray_Fn* glBindVertexArray = (glBindVertexArray_Fn*) wglGetProcAddress("glBindVertexArray");

  const char *vertexShaderSource = "#version 330 core\n"
      "layout (location = 0) in vec3 aPos;\n"
      "void main()\n"
      "{\n"
      "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
      "}\0";
  unsigned int vertexShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  const char* fragmentShaderSource = "#version 330 core\n"
      "out vec4 FragColor;\n"
      "void main()\n"
      "{\n"
      "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
      "}\0";
  unsigned int fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  unsigned int shaderProgram;
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  float vertices[] = {
      -0.5f, -0.5f, 0.0f,
      0.5f, -0.5f, 0.0f,
      0.0f,  0.5f, 0.0f
  };
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  while (true) {
    glUseProgram(shaderProgram);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    WindowSwapBuffers();
  }

  WindowDeinit();
  return 0;
}
