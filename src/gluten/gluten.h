#ifndef GLUTEN_GLUTEN_H
#define GLUTEN_GLUTEN_H

#ifdef EMSCRIPTEN
  #define WEBGL10
#else
  //#define OPENGL10
  #define OPENGL20
#endif

#ifdef OPENGL10
  #include <GL/glew.h>
  #include <GL/freeglut.h>
  #define GN_TEXTURE_2D GL_TEXTURE_2D
  #define GN_LIGHTING GL_LIGHTING
#endif
#ifdef OPENGL20
  #include <GL/glew.h>
  #include <GL/freeglut.h>
  #define GN_TEXTURE_2D 56
  #define GN_LIGHTING 57
#endif
#ifdef WEBGL10
  #include <GL/glew.h>
  #include <GL/glut.h>
  #define GN_TEXTURE_2D 56
  #define GN_LIGHTING 57
  #define GLUT_ACTION_ON_WINDOW_CLOSE 99
  #define GLUT_ACTION_GLUTMAINLOOP_RETURNS 100
  void glutLeaveMainLoop();
  void glutSetOption(GLenum eWhat, int value);
#endif

/*
#define glPushMatrix gnPushMatrix_
#define glPopMatrix gnPopMatrix_
#define gluPerspective gnPerspective_
#define glLoadIdentity gnLoadIdentity_
#define glEnableClientState gnEnableClientState_
#define glDisableClientState gnDisableClientState_
#define glMatrixMode gnMatrixMode_
#define glOrtho gnOrtho_
#define glVertexPointer gnVertexPointer_
#define glTexCoordPointer gnTexCoordPointer_
#define glNormalPointer gnNormalPointer_
#define glTranslatef gnTranslatef_
#define glScalef gnScalef_
#define glRotatef gnRotatef_
*/

void gnInit();

void gnPerspective(GLdouble fovy, GLdouble aspect,
  GLdouble zNear, GLdouble zFar);

void gnOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top,
  GLdouble nearVal, GLdouble farVal);

void gnEnable(int cap);
void gnDisable(int cap);
void gnLoadIdentity(void);
void gnPushMatrix(void);
void gnPopMatrix(void);
void gnEnableClientState(GLenum cap);
void gnDisableClientState(GLenum cap);
void gnMatrixMode(GLenum mode);
void gnDrawArrays(GLenum mode, GLint first, GLsizei count);

void gnVertexPointer(GLint size,  GLenum type, GLsizei stride,
  const GLvoid *pointer);

void gnTexCoordPointer(GLint size, GLenum type, GLsizei stride,
  const GLvoid *pointer);

void gnNormalPointer(GLenum type, GLsizei stride, const GLvoid *pointer);

void gnTranslatef(GLfloat x, GLfloat y, GLfloat z);
void gnScalef(GLfloat x, GLfloat y, GLfloat z);
void gnRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
void gnLightfv(GLenum light, GLenum pname, const GLfloat *params);

#endif
