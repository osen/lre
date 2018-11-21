#include "math.h"

struct Actor;

struct Transform
{
  struct Vector3 position;
  struct Vector3 rotation;
};

struct Vector3 TransformPosition(struct Actor *ctx);
void TransformSetPosition(struct Actor *ctx, struct Vector3 position);
void TransformSetRotation(struct Actor *ctx, struct Vector3 rotation);

void TransformTranslate(struct Actor *ctx, struct Vector3 translation);

void TransformRotate(struct Actor *ctx, struct Vector3 rotation);
struct Vector3 TransformRotation(struct Actor *ctx);
