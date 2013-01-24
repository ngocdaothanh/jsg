#include "gl.h"

#include <jni.h>

#include <stdio.h>
#include <stdlib.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include "log.h"
#include "matrix.h"

//------------------------------------------------------------------------------

#define STRINGIFY(A) #A

static const char* gVertexShader = STRINGIFY(
  uniform   mat4 u_projection;
  attribute vec4 a_position;
  attribute vec2 a_texCoord;
  varying   vec2 v_texCoord;
  void main()
  {
     gl_Position = u_projection * a_position;
     v_texCoord = a_texCoord;
  }
);

static const char* gFragmentShader = STRINGIFY(
  precision mediump   float;
  varying   vec2      v_texCoord;
  uniform   sampler2D s_texture;
  void main()
  {
    gl_FragColor = texture2D(s_texture, v_texCoord);
  }
);

//------------------------------------------------------------------------------

static GLuint loadShader(GLenum shaderType, const char* pSource) {
  GLuint shader = glCreateShader(shaderType);
  if (shader) {
    glShaderSource(shader, 1, &pSource, NULL);
    glCompileShader(shader);
    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
      GLint infoLen = 0;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
      if (infoLen) {
        char* buf = (char*) malloc(infoLen);
        if (buf) {
          glGetShaderInfoLog(shader, infoLen, NULL, buf);
          LOGE("Could not compile shader %d:\n%s\n", shaderType, buf);
          free(buf);
        }
        glDeleteShader(shader);
        shader = 0;
      }
    }
  }
  return shader;
}

static GLuint createProgram(const char* pVertexSource, const char* pFragmentSource) {
  GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
  if (!vertexShader) {
    return 0;
  }

  GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
  if (!pixelShader) {
    return 0;
  }

  GLuint program = glCreateProgram();
  if (program) {
    glAttachShader(program, vertexShader);
    glAttachShader(program, pixelShader);
    glLinkProgram(program);
    GLint linkStatus = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE) {
      GLint bufLength = 0;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
      if (bufLength) {
        char* buf = (char*) malloc(bufLength);
        if (buf) {
          glGetProgramInfoLog(program, bufLength, NULL, buf);
          LOGE("Could not link program:\n%s\n", buf);
          free(buf);
        }
      }
      glDeleteProgram(program);
      program = 0;
    }
  }
  return program;
}

//------------------------------------------------------------------------------

unsigned int GL::program;
int          GL::projection;
int          GL::position;
int          GL::texCoord;
int          GL::sampler;

int            GL::width;
int            GL::height;
int            GL::textureWidth;
int            GL::textureHeight;
unsigned char* GL::rgba = NULL;
unsigned int   GL::stageTextureId;
float          GL::vertices[20];

void GL::setupGraphics(int width, int height) {
  // Program

  program = createProgram(gVertexShader, gFragmentShader);
  if (!program) {
    LOGE("Could not create program.");
    return;
  }

  projection = glGetUniformLocation(program, "u_projection");
  sampler    = glGetUniformLocation(program, "s_texture");
  position   = glGetAttribLocation(program,  "a_position");
  texCoord   = glGetAttribLocation(program,  "a_texCoord");

  // This is 2D
  glDisable(GL_DEPTH_TEST);

  resize(width, height);
}

void GL::draw(unsigned char* argb, int width, int height)
{
  // No need to call glClear because the canvas has already been cleared

  if (GL::width != width || GL::height != height) {
    glDeleteTextures(1, &stageTextureId);
    resize(width, height);
  }

  // Update texture

  argb2rgba(argb);
  glBindTexture(GL_TEXTURE_2D, stageTextureId);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);

  // Draw texture

  glVertexAttribPointer(GL::position, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), vertices);
  glVertexAttribPointer(GL::texCoord, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), &vertices[3]);

  glEnableVertexAttribArray(GL::position);
  glEnableVertexAttribArray(GL::texCoord);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, stageTextureId);

  // Set the sampler texture unit to 0
  glUniform1i(GL::sampler, 0);

  static GLushort indices[] = {0, 1, 2, 0, 2, 3};
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
}

//------------------------------------------------------------------------------

int GL::nearestPow2(int n)
{
  int ret = 1;
  while (ret < n) {
    ret <<= 1;
  }
  return ret;
}

void GL::resize(int width, int height)
{
  GL::width  = width;
  GL::height = height;

  textureWidth  = nearestPow2(width);
  textureHeight = nearestPow2(height);

  if (rgba) free(rgba);
  rgba = (unsigned char*) malloc(textureWidth * textureHeight * 4);

  // Viewport

  glViewport(0, 0, width, height);
  glClearColor(0, 0, 0, 0);

  glUseProgram(program);

  int hw = width  / 2;
  int hh = height / 2;

  ESMatrix ot;
  esMatrixLoadIdentity(&ot);
  esOrtho(&ot, -hw, hw, -hh, hh, 0, -20);
  glUniformMatrix4fv(GL::projection, 1, GL_FALSE, (GLfloat*) &ot.m[0][0]);

  // Stage texture

  glGenTextures(1, &stageTextureId);
  glBindTexture(GL_TEXTURE_2D, stageTextureId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  vertices[0] = -hw;
  vertices[1] = hh;
  vertices[2] = 0;

  vertices[3] = 0;
  vertices[4] = 0;

  vertices[5] = -hw;
  vertices[6] = -hh;
  vertices[7] = 0;

  vertices[8] = 0;
  vertices[9] = 1.0 * height / textureHeight;

  vertices[10] = hw;
  vertices[11] = -hh;
  vertices[12] = 0;

  vertices[13] = 1.0 * width / textureWidth;
  vertices[14] = 1.0 * height / textureHeight;

  vertices[15] = hw;
  vertices[16] = hh;
  vertices[17] = 0;

  vertices[18] = 1.0 * width / textureWidth;
  vertices[19] = 0;
}

void GL::argb2rgba(unsigned char* argb)
{
  unsigned char* dst = rgba;
  for (int y = 0; y < height; y++) {
    uint32_t *row = (uint32_t *)(argb + (width * 4) * y);
    for (int x = 0; x < width; x++) {
      int bx = x * 4;
      uint32_t *pixel = row + x;
      uint8_t a = *pixel >> 24;
      uint8_t r = *pixel >> 16;
      uint8_t g = *pixel >> 8;
      uint8_t b = *pixel;
      dst[bx + 3] = a;
      dst[bx + 0] = r;
      dst[bx + 1] = g;
      dst[bx + 2] = b;
    }
    dst += textureWidth * 4;
  }
}
