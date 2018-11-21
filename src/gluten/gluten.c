#include "gluten.h"
#include "linmath.h"

#include <plank/plank.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MULTILINE(...) #__VA_ARGS__

#ifdef WEBGL10
void glutLeaveMainLoop(){}
void glutSetOption(GLenum eWhat, int value){}

const char *fragmentShaderSrc = MULTILINE(
  precision mediump float;

  varying vec2 v_texCoord;
  varying vec3 v_diffuse;

  uniform sampler2D u_texture;

  void main()
  {
    gl_FragColor = vec4(v_diffuse, 1) * texture2D(u_texture, v_texCoord);
  }
);
#endif

#ifdef OPENGL20
const char *fragmentShaderSrc = MULTILINE(
  varying vec2 v_texCoord;
  varying vec3 v_diffuse;

  uniform sampler2D u_texture;

  void main()
  {
    gl_FragColor = vec4(v_diffuse, 1) * texture2D(u_texture, v_texCoord);
  }
);
#endif

#ifndef OPENGL10

GLuint shaderProgramId;
GLuint fragmentShaderId;
GLuint vertexShaderId;

GLint positionId;
GLint texCoordId;
GLint normalId;
GLint modelViewId;
GLint projectionId;
GLint enableLightingId;
GLint lightPositionId;

struct mat4
{
  mat4x4 v;
};

vector(struct mat4) *projections;
vector(struct mat4) *modelViews;
int matrixMode;

float degToRad(float deg)
{
  return deg * 3.14f / 180.0f;
}

const char *vertexShaderSrc = MULTILINE(
  attribute vec3 a_position;
  attribute vec2 a_texCoord;
  attribute vec3 a_normal;

  uniform mat4 u_projection;
  uniform mat4 u_modelView;

  uniform int u_enableLighting;
  uniform vec3 u_lightPosition;

  varying vec2 v_texCoord;
  varying vec3 v_diffuse;

  mat4 internal_inverse(mat4 m) {
  float
      a00 = m[0][0], a01 = m[0][1], a02 = m[0][2], a03 = m[0][3],
      a10 = m[1][0], a11 = m[1][1], a12 = m[1][2], a13 = m[1][3],
      a20 = m[2][0], a21 = m[2][1], a22 = m[2][2], a23 = m[2][3],
      a30 = m[3][0], a31 = m[3][1], a32 = m[3][2], a33 = m[3][3],

      b00 = a00 * a11 - a01 * a10,
      b01 = a00 * a12 - a02 * a10,
      b02 = a00 * a13 - a03 * a10,
      b03 = a01 * a12 - a02 * a11,
      b04 = a01 * a13 - a03 * a11,
      b05 = a02 * a13 - a03 * a12,
      b06 = a20 * a31 - a21 * a30,
      b07 = a20 * a32 - a22 * a30,
      b08 = a20 * a33 - a23 * a30,
      b09 = a21 * a32 - a22 * a31,
      b10 = a21 * a33 - a23 * a31,
      b11 = a22 * a33 - a23 * a32,

      det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;

  return mat4(
      a11 * b11 - a12 * b10 + a13 * b09,
      a02 * b10 - a01 * b11 - a03 * b09,
      a31 * b05 - a32 * b04 + a33 * b03,
      a22 * b04 - a21 * b05 - a23 * b03,
      a12 * b08 - a10 * b11 - a13 * b07,
      a00 * b11 - a02 * b08 + a03 * b07,
      a32 * b02 - a30 * b05 - a33 * b01,
      a20 * b05 - a22 * b02 + a23 * b01,
      a10 * b10 - a11 * b08 + a13 * b06,
      a01 * b08 - a00 * b10 - a03 * b06,
      a30 * b04 - a31 * b02 + a33 * b00,
      a21 * b02 - a20 * b04 - a23 * b00,
      a11 * b07 - a10 * b09 - a12 * b06,
      a00 * b09 - a01 * b07 + a02 * b06,
      a31 * b01 - a30 * b03 - a32 * b00,
      a20 * b03 - a21 * b01 + a22 * b00) / det;
  }

  mat4 internal_transpose(mat4 m) {
  return mat4(m[0][0], m[1][0], m[2][0], m[3][0],
              m[0][1], m[1][1], m[2][1], m[3][1],
              m[0][2], m[1][2], m[2][2], m[3][2],
              m[0][3], m[1][3], m[2][3], m[3][3]);
  }

  void main()
  {
    v_diffuse = vec3(1, 1, 1);

    if(u_enableLighting == 1)
    {
      vec3 fragPos = vec3(u_modelView * vec4(a_position, 1.0));
      vec3 N = normalize(a_normal);
      N = vec3(internal_inverse(internal_transpose(u_modelView)) * vec4(N, 1));
      vec3 lightDir = normalize(u_lightPosition - fragPos);
      float diff = max(dot(N, lightDir), 0.0);
      v_diffuse = diff * vec3(1, 1, 1);
    }

    gl_Position = u_projection * u_modelView * vec4(a_position, 1.0);
    v_texCoord = a_texCoord;
  }
);

void _gnCleanup()
{
  vector_delete(projections);
  vector_delete(modelViews);
}

#endif

void gnInit()
{
#ifdef OPENGL10
  if(glewInit() != GLEW_OK)
  {
    printf("Failed to initialize OpenGL");
    abort();
  }
#else
  GLint success = 0;
  char infoLog[512] = {0};

  printf("Initializing Gluten\n");

  if(glewInit() != GLEW_OK)
  {
    printf("Failed to initialize OpenGL");
    abort();
  }

  projections = vector_new(struct mat4);
  modelViews = vector_new(struct mat4);

  atexit(_gnCleanup);

  vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShaderId, 1, &vertexShaderSrc, NULL);
  glCompileShader(vertexShaderId);
  glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);

  if(!success)
  {
    glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
    printf("Failed to compile vertex shader: %s\n", infoLog);
    abort();
  }

  fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShaderId, 1, &fragmentShaderSrc, NULL);
  glCompileShader(fragmentShaderId);
  glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);

  if(!success)
  {
    glGetShaderInfoLog(fragmentShaderId, 512, NULL, infoLog);
    printf("Failed to compile fragment shader: %s\n", infoLog);
    abort();
  }

  shaderProgramId = glCreateProgram();
  glAttachShader(shaderProgramId, vertexShaderId);
  glAttachShader(shaderProgramId, fragmentShaderId);
  glLinkProgram(shaderProgramId);
  glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &success);

  if(!success)
  {
    glGetProgramInfoLog(shaderProgramId, 512, NULL, infoLog);
    printf("Failed to link shader program: %s\n", infoLog);
    abort();
  }

  glUseProgram(shaderProgramId);

  positionId = glGetAttribLocation(shaderProgramId, "a_position");
  texCoordId = glGetAttribLocation(shaderProgramId, "a_texCoord");
  normalId = glGetAttribLocation(shaderProgramId, "a_normal");
  modelViewId = glGetUniformLocation(shaderProgramId, "u_modelView");
  projectionId = glGetUniformLocation(shaderProgramId, "u_projection");
  enableLightingId = glGetUniformLocation(shaderProgramId, "u_enableLighting");
  lightPositionId = glGetUniformLocation(shaderProgramId, "u_lightPosition");

  if(modelViewId == -1 || projectionId == -1 ||
    enableLightingId == -1 || lightPositionId == -1)
  {
    printf("Failed to obtain uniform locations from shader program\n");
    abort();
  }

  if(positionId == -1 || texCoordId == -1 || normalId == -1)
  {
    printf("Failed to obtain attrib locations from shader program\n");
    abort();
  }

  mat4x4 m = {0};
  mat4x4_identity(m);
  struct mat4 t = {0};
  memcpy(t.v, m, sizeof(m));
  vector_push_back(modelViews, t);
  vector_push_back(projections, t);
  matrixMode = 1;
#endif
}

void gnEnable(int cap)
{
#ifdef OPENGL10
  glEnable(cap);
#else
  if(cap == GN_TEXTURE_2D)
  {
    // TODO
  }
  else if(cap == GN_LIGHTING)
  {
    glUniform1i(enableLightingId, 1);
  }
#endif
}

void gnDisable(int cap)
{
#ifdef OPENGL10
  glDisable(cap);
#else
  if(cap == GN_TEXTURE_2D)
  {
    // TODO
  }
  else if(cap == GN_LIGHTING)
  {
    glUniform1i(enableLightingId, 0);
  }
#endif
}

void gnPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear,
  GLdouble zFar)
{
#ifdef OPENGL10
  gluPerspective(fovy, aspect, zNear, zFar);
#else
  if(matrixMode != 0)
  {
    printf("It is odd to do this to model view matrix\n");
    abort();
  }

  struct mat4 a = vector_at(projections, vector_size(projections) - 1);
  mat4x4_perspective(a.v, degToRad(fovy), aspect, zNear, zFar);
  vector_set(projections, vector_size(projections) - 1, a);
#endif
}

void gnOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top,
  GLdouble nearVal, GLdouble farVal)
{
#ifdef OPENGL10
  glOrtho(left, right, bottom, top, nearVal, farVal);
#else
  if(matrixMode != 0)
  {
    printf("It is odd to do this to model view matrix\n");
    abort();
  }

  struct mat4 a = vector_at(projections, vector_size(projections) - 1);
  mat4x4_ortho(a.v, left, right, bottom, top, nearVal, farVal);
  vector_set(projections, vector_size(projections) - 1, a);
#endif
}

void gnLoadIdentity()
{
#ifdef OPENGL10
  glLoadIdentity();
#else
  if(matrixMode == 0)
  {
    struct mat4 a = vector_at(projections, vector_size(projections) - 1);
    mat4x4_identity(a.v);
    vector_set(projections, vector_size(projections) - 1, a);
  }
  else if(matrixMode == 1)
  {
    struct mat4 a = vector_at(modelViews, vector_size(modelViews) - 1);
    mat4x4_identity(a.v);
    vector_set(modelViews, vector_size(modelViews) - 1, a);
  }
#endif
}

void gnPushMatrix()
{
#ifdef OPENGL10
  glPushMatrix();
#else
  if(matrixMode == 0)
  {
    struct mat4 a = vector_at(projections, vector_size(projections) - 1);
    vector_push_back(projections, a);
  }
  else if(matrixMode == 1)
  {
    struct mat4 a = vector_at(modelViews, vector_size(modelViews) - 1);
    vector_push_back(modelViews, a);
  }
#endif
}

void gnPopMatrix()
{
#ifdef OPENGL10
  glPopMatrix();
#else
  if(matrixMode == 0)
  {
    vector_erase(projections, vector_size(projections) - 1);
  }
  else if(matrixMode == 1)
  {
    vector_erase(modelViews, vector_size(modelViews) - 1);
  }
#endif
}

void gnEnableClientState(GLenum cap)
{
#ifdef OPENGL10
  glEnableClientState(cap);
#else
  if(cap == GL_VERTEX_ARRAY)
  {
    glEnableVertexAttribArray(positionId);
  }
  else if(cap == GL_TEXTURE_COORD_ARRAY)
  {
    glEnableVertexAttribArray(texCoordId);
  }
  else if(cap == GL_NORMAL_ARRAY)
  {
    glEnableVertexAttribArray(normalId);
  }
#endif
}

void gnDisableClientState(GLenum cap)
{
#ifdef OPENGL10
  glDisableClientState(cap);
#else
  if(cap == GL_VERTEX_ARRAY)
  {
    glDisableVertexAttribArray(positionId);
  }
  else if(cap == GL_TEXTURE_COORD_ARRAY)
  {
    glDisableVertexAttribArray(texCoordId);
  }
  else if(cap == GL_NORMAL_ARRAY)
  {
    glDisableVertexAttribArray(normalId);
  }
#endif
}

void gnMatrixMode(GLenum mode)
{
#ifdef OPENGL10
  glMatrixMode(mode);
#else
  if(mode == GL_PROJECTION)
  {
    matrixMode = 0;
  }
  else if(mode == GL_MODELVIEW)
  {
    matrixMode = 1;
  }
  else
  {
    printf("Unsupported matrix mode\n");
    abort();
  }
#endif
}

void gnVertexPointer(GLint size,  GLenum type, GLsizei stride,
  const GLvoid *pointer)
{
#ifdef OPENGL10
  glVertexPointer(size, type, stride, pointer);
#else
  glVertexAttribPointer(positionId, size, type, GL_FALSE, stride, pointer);
#endif
}

void gnTexCoordPointer(GLint size, GLenum type, GLsizei stride,
  const GLvoid *pointer)
{
#ifdef OPENGL10
  glTexCoordPointer(size, type, stride, pointer);
#else
  glVertexAttribPointer(texCoordId, size, type, GL_FALSE, stride, pointer);
#endif
}

void gnNormalPointer(GLenum type, GLsizei stride, const GLvoid *pointer)
{
#ifdef OPENGL10
  glNormalPointer(type, stride, pointer);
#else
  glVertexAttribPointer(normalId, 3, type, GL_FALSE, stride, pointer);
#endif
}

void gnTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
#ifdef OPENGL10
  glTranslatef(x, y, z);
#else
  if(matrixMode != 1)
  {
    printf("It is odd to do this to projection matrix\n");
    abort();
  }

  struct mat4 a = vector_at(modelViews, vector_size(modelViews) - 1);
  mat4x4_translate_in_place(a.v, x, y, z);
  vector_set(modelViews, vector_size(modelViews) - 1, a);
#endif
}

void gnScalef(GLfloat x, GLfloat y, GLfloat z)
{
#ifdef OPENGL10
  glScalef(x, y, z);
#else
  if(matrixMode != 1)
  {
    printf("It is odd to do this to projection matrix\n");
    abort();
  }

  struct mat4 a = vector_at(modelViews, vector_size(modelViews) - 1);
  struct mat4 b = {0};
  mat4x4_scale_aniso(b.v, a.v, x, y, z);
  vector_set(modelViews, vector_size(modelViews) - 1, b);
#endif
}

void gnRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
#ifdef OPENGL10
  glRotatef(angle, x, y, z);
#else
  if(matrixMode != 1)
  {
    printf("It is odd to do this to projection matrix\n");
    abort();
  }

  struct mat4 a = vector_at(modelViews, vector_size(modelViews) - 1);
  struct mat4 b = {0};
  mat4x4_rotate(b.v, a.v, x, y, z, degToRad(angle));
  vector_set(modelViews, vector_size(modelViews) - 1, b);
#endif
}

void gnDrawArrays(GLenum mode, GLint first, GLsizei count)
{
#ifdef OPENGL10
  glDrawArrays(mode, first, count);
#else
  struct mat4 a = vector_at(modelViews, vector_size(modelViews) - 1);

  glUniformMatrix4fv(modelViewId, 1, GL_FALSE, (GLfloat *)a.v);

  a = vector_at(projections, vector_size(projections) - 1);

  glUniformMatrix4fv(projectionId, 1, GL_FALSE, (GLfloat *)a.v);

  glDrawArrays(mode, first, count);
#endif
}

void gnLightfv(GLenum light, GLenum pname, const GLfloat *params)
{
#ifdef OPENGL10
  glLightfv(light, pname, params);
#else
  struct mat4 mv = {0};
  vec4 r = {0};
  float dist = 1;

  if(params[3] == 0) dist = 10000;

  //vec4 t = {params[0], params[1], params[2], params[3]};
  vec4 t = {params[0] * dist, params[1] * dist, params[2] * dist, params[3] * dist};
  //vec4 t = {params[0] * dist, params[1] * dist, params[2] * dist, 1};
  mv = vector_at(modelViews, vector_size(modelViews) - 1);
  mat4x4_mul_vec4(r, mv.v, t);
  //glUniform4f(lightPositionId, r[0], r[1], r[2], params[3]);
  glUniform3f(lightPositionId, r[0], r[1], r[2]);
#endif
}
