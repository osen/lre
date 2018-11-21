#include "Transform.h"
#include "Actor.h"

#include <stdlib.h>
#include <math.h>

void TransformInit(struct Actor *ctx)
{

}

void TransformRotate(struct Actor *ctx, struct Vector3 rotation)
{
  struct Transform *t = NULL;

  t = ActorComponent(ctx, Transform);
  t->rotation.x += rotation.x;
  t->rotation.y += rotation.y;
  t->rotation.z += rotation.z;
}

struct Vector3 TransformRotation(struct Actor *ctx)
{
  struct Transform *t = NULL;

  t = ActorComponent(ctx, Transform);

  return t->rotation;
}

struct Vector3 TransformPosition(struct Actor *ctx)
{
  struct Transform *t = NULL;

  t = ActorComponent(ctx, Transform);

  return t->position;
}

void TransformSetPosition(struct Actor *ctx, struct Vector3 position)
{
  struct Transform *t = NULL;

  t = ActorComponent(ctx, Transform);
  t->position = position;
}

void TransformSetRotation(struct Actor *ctx, struct Vector3 rotation)
{
  struct Transform *t = NULL;

  t = ActorComponent(ctx, Transform);
  t->rotation = rotation;
}

void TransformTranslate(struct Actor *ctx, struct Vector3 translation)
{
  struct Transform *t = NULL;
  float rad = 0;

  t = ActorComponent(ctx, Transform);
  rad = t->rotation.y * 3.14f / 180.0f;
  t->position.x += sin(rad) * translation.z;
  t->position.z += cos(rad) * translation.z;
  t->position.z += sin(rad) * translation.x;
  t->position.x -= cos(rad) * translation.x;
  t->position.y += translation.y;
}

