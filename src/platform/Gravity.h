#include "Actor.h"

struct Gravity
{
  float strength;
};

void GravitySetStrength(struct Actor *ctx, float strength);
float GravityStrength(struct Actor *ctx);
