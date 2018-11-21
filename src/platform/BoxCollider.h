#include "math.h"

#include <plank/plank.h>

struct BoxCollider
{
  struct Vector3 center;
  struct Vector3 lastCenter;
  struct Vector3 size;
  vector(struct Actor *) *collidingActors;
};

struct Vector3 BoxColliderCenter(struct Actor *ctx);
struct Vector3 BoxColliderSize(struct Actor *ctx);
