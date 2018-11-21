#include "Environment.h"

#include <gluten/gluten.h>

#include <plank/plank.h>

#include <time.h>

struct Environment
{
  float deltaTime;
  time_t lastTime;
};

struct Environment *environment;

void _EnvironmentInitialize(int argc, char *argv[])
{
  environment = palloc(struct Environment);

  environment->lastTime = glutGet(GLUT_ELAPSED_TIME);
}

void _EnvironmentTick()
{
  time_t curr = glutGet(GLUT_ELAPSED_TIME);
  time_t diff = curr - environment->lastTime;
  environment->deltaTime = (float)diff / 1000.0f;
  environment->lastTime = curr;

  if(environment->deltaTime > 1)
  {
    environment->deltaTime = 1;
  }
}

void _EnvironmentCleanup()
{
  pfree(environment);
}

float EnvironmentDeltaTime()
{
  return environment->deltaTime;
}

float TimeDelta()
{
  return EnvironmentDeltaTime();
}
