#include "platform.h"

#include <gluten/gluten.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

void _PlatformTick()
{
  size_t i = 0;

  _EnvironmentTick();

  for(i = 0; i < vector_size(_actors); i++)
  {
    ActorEvent(vector_at(_actors, i), "tick");
  }

  for(i = 0; i < vector_size(_actors); i++)
  {
    if(!ActorAlive(vector_at(_actors, i)))
    {
      _ActorDestroy(vector_at(_actors, i));
      vector_erase(_actors, i);
      i--;
    }
  }

  if(vector_size(_actors) < 1)
  {
    glutLeaveMainLoop();
  }
}

void _PlatformDisplay()
{
  size_t i = 0;

  for(i = 0; i < vector_size(_actors); i++)
  {
    ActorEvent(vector_at(_actors, i), "display");
  }
}

void _PlatformGui()
{
  size_t i = 0;

  for(i = 0; i < vector_size(_actors); i++)
  {
    ActorEvent(vector_at(_actors, i), "gui");
  }
}

void _idle(void)
{
  _PlatformTick();

  glutPostRedisplay();
  //Sleep(10);
}

void _display(void)
{
  struct Vector2 screenSize = {0};

  glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  _GuiUpdateScreenSize();
  screenSize = GuiScreenSize();
  glViewport(0, 0, (GLsizei)screenSize.x, (GLsizei)screenSize.y);

  gnEnable(GN_TEXTURE_2D);
  gnEnableClientState(GL_VERTEX_ARRAY);
  gnEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glEnable(GL_CULL_FACE);

  _CameraApplyProjection();
  gnLoadIdentity();

  GLfloat pos[4] = {1, 1, 2, 0.0f};
  gnLightfv(GL_LIGHT0, GL_POSITION, pos);

  _CameraApplyView();

  _PlatformDisplay();

  glDisable(GL_DEPTH_TEST);

  _GuiApplyProjection();
  gnLoadIdentity();
  _PlatformGui();

  gnDisableClientState(GL_TEXTURE_COORD_ARRAY);
  gnDisableClientState(GL_VERTEX_ARRAY);
  gnDisable(GN_TEXTURE_2D);
  glDisable(GL_BLEND);
  glDisable(GL_CULL_FACE);

  glutSwapBuffers();
}

void _reshape(int width, int height)
{
  _GuiSetScreenSize(width, height);
}

void _keyboard(unsigned char key, int x, int y)
{
  _InputSetKey(key, 1);
}

void _keyboardUp(unsigned char key, int x, int y)
{
  _InputSetKey(key, 0);
}

void PlatformInitialize(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  glutCreateWindow("LEGO Resident Evil");

  glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

  gnInit();

  _EnvironmentInitialize(argc, argv);
  _CacheInitialize();
  _GuiInitialize();
  _CameraInitialize();
  _DebugInitialize();
  _InputInitialize();

  _actors = vector_new(struct Actor*);

  glutDisplayFunc(_display);
  glutIdleFunc(_idle);
  glutReshapeFunc(_reshape);
  glutKeyboardFunc(_keyboard);
  glutKeyboardUpFunc(_keyboardUp);
}

void PlatformCleanup()
{
  size_t i = 0;

  for(i = 0; i < vector_size(_actors); i++)
  {
    ActorDestroy(vector_at(_actors, i));
  }

  for(i = 0; i < vector_size(_actors); i++)
  {
    _ActorDestroy(vector_at(_actors, i));
  }

  vector_delete(_actors);

  _DebugCleanup();
  _GuiCleanup();
  _CameraCleanup();
  _CacheCleanup();
  _EnvironmentCleanup();
  _InputCleanup();
}

void PlatformRun()
{
  _PlatformTick();
  glutMainLoop();
}
