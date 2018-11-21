#include "Camera.h"
#include "math.h"
#include "Gui.h"
#include "Input.h"

#include <plank/plank.h>

#include <gluten/gluten.h>

struct Camera
{
  struct Vector3 position;
  struct Vector3 rotation;
};

struct Camera *camera;

void _CameraInitialize()
{
  camera = palloc(struct Camera);
/*
  //camera->position.z = -10;
  camera->position.y = 7.5f;
  camera->position.x = -15;
  camera->rotation.x = -30.0f;
  camera->rotation.y = -90.0f;
*/
}

void _CameraCleanup()
{
  pfree(camera);
}

void _CameraApplyProjection()
{
  struct Vector2 screenSize = {0};

  gnMatrixMode(GL_PROJECTION);
  gnLoadIdentity();
  screenSize = GuiScreenSize();

  gnPerspective(55.0f, screenSize.x / screenSize.y, 0.1f, 1000.0f);

  gnMatrixMode(GL_MODELVIEW);
}

void _CameraApplyView()
{
  gnRotatef(camera->rotation.x, -1, 0, 0);
  gnRotatef(camera->rotation.y, 0, -1, 0);
  gnTranslatef(-camera->position.x, -camera->position.y, -camera->position.z);
}

void CameraSetPosition(struct Vector3 position)
{
  camera->position = position;
}

void CameraSetRotation(struct Vector3 rotation)
{
  camera->rotation = rotation;
}

