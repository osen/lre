#include "Gravity.h"
#include "Actor.h"
#include "Transform.h"
#include "math.h"
#include "Environment.h"

void _GravityTick(struct Actor *ctx)
{
  struct Vector3 pos = {0};

  pos = TransformPosition(ctx);
  pos.y -= GravityStrength(ctx) * TimeDelta();
  TransformSetPosition(ctx, pos);
}

void GravitySetStrength(struct Actor *ctx, float strength)
{
  struct Gravity *g = ActorComponent(ctx, Gravity);

  g->strength = strength;
}

float GravityStrength(struct Actor *ctx)
{
  struct Gravity *g = ActorComponent(ctx, Gravity);

  return g->strength;
}

void GravityInit(struct Actor *ctx)
{
  GravitySetStrength(ctx, 2.5f);
  ActorAddEvent(ctx, "tick", _GravityTick);
}
