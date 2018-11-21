#include "BoxCollider.h"
#include "Actor.h"
#include "Debug.h"
#include "ModelRenderer.h"
#include "math.h"
#include "Model.h"
#include "Transform.h"
#include "ModelCollider.h"
#include "Gravity.h"
#include "Environment.h"

#define TRYSTEP 0.01f
#define MAXSTEP 0.5f
#define TRYINC 0.05f
#define MAXINC 0.5f

int _BoxColliderCollidingWithModelCollider(struct BoxCollider *bc, struct ModelCollider *mc)
{
/*
  vector(struct Triangle) *triangles = NULL;
  size_t ti = 0;

  triangles = mc->triangles;

  for(ti = 0; ti < vector_size(triangles); ti++)
  {
    if(TriangleBoxOverlap(vector_at(triangles, ti), bc->center, bc->size))
    {
      return 1;
    }
  }

  return 0;
*/
  return _ModelColliderBoxOverlap(mc, bc->center, bc->size);
}

int _BoxColliderUncollideModelCollider(struct BoxCollider *bc, struct Actor *collider, float gravity)
{
  struct ModelCollider *mc = NULL;
  float curr = TRYSTEP;

  mc = ActorComponent(collider, ModelCollider);
  bc->center.y += gravity;

  while(1)
  {
    if(!_BoxColliderCollidingWithModelCollider(bc, mc)) break;
    bc->center = Vector3Add(bc->center, Vector3Xyz(0, curr, 0));
    if(!_BoxColliderCollidingWithModelCollider(bc, mc)) break;
    bc->center = Vector3Add(bc->center, Vector3Xyz(0, -curr, 0));

    curr += TRYSTEP;
    if(curr >= MAXSTEP) break;
  }

  curr = TRYINC;

  while(1)
  {
    if(!_BoxColliderCollidingWithModelCollider(bc, mc)) break;
    bc->center = Vector3Add(bc->center, Vector3Xyz(curr, 0, 0));
    if(!_BoxColliderCollidingWithModelCollider(bc, mc)) break;
    bc->center = Vector3Add(bc->center, Vector3Xyz(-curr, 0, 0));
    bc->center = Vector3Add(bc->center, Vector3Xyz(-curr, 0, 0));
    if(!_BoxColliderCollidingWithModelCollider(bc, mc)) break;
    bc->center = Vector3Add(bc->center, Vector3Xyz(curr, 0, 0));
    bc->center = Vector3Add(bc->center, Vector3Xyz(0, 0, curr));
    if(!_BoxColliderCollidingWithModelCollider(bc, mc)) break;
    bc->center = Vector3Add(bc->center, Vector3Xyz(0, 0, -curr));
    bc->center = Vector3Add(bc->center, Vector3Xyz(0, 0, -curr));
    if(!_BoxColliderCollidingWithModelCollider(bc, mc)) break;
    bc->center = Vector3Add(bc->center, Vector3Xyz(0, 0, curr));

    curr += TRYINC;
    if(curr >= MAXINC) return 0;
  }

  bc->center.y -= gravity;
  if(!_BoxColliderCollidingWithModelCollider(bc, mc)) return 1;
  bc->center.y += gravity;

  return 1;
}

void _BoxColliderTick(struct Actor *ctx)
{
  size_t i = 0;
  struct BoxCollider *bc = NULL;
  struct ModelRenderer *mr = NULL;
  float gravity = 0;
  struct Vector3 center = {0};

  mr = ActorComponent(ctx, ModelRenderer);
  bc = ActorComponent(ctx, BoxCollider);

  /* Keep height to actual center of triangles but bring the x / z origin to middle of transform */
  center = ModelCenter(mr->model);
  center.x = 0;
  center.z = 0;

  bc->center = Vector3Add(TransformPosition(ctx), center);
  bc->size = ModelSize(mr->model);

  /* Bring width / length to an average between the two */
  //bc->size.x += bc->size.z;
  //bc->size.x /= 2.0f;
  //bc->size.z = bc->size.x;

  /* Pick the smallest dimension for width / length */
  if(bc->size.x < bc->size.z)
  {
    bc->size.z = bc->size.x;
  }
  else
  {
    bc->size.x = bc->size.z;
  }

  vector_clear(bc->collidingActors);
  ActorsWithComponent(ModelCollider, bc->collidingActors);

  if(ActorHasComponent(ctx, Gravity))
  {
    gravity = GravityStrength(ctx) * TimeDelta();
  }

  for(i = 0; i < vector_size(bc->collidingActors); i++)
  {
    struct Actor *actor = vector_at(bc->collidingActors, i);

    if(!_BoxColliderUncollideModelCollider(bc, actor, gravity))
    {
      bc->center = bc->lastCenter;
      break;
    }
  }

  TransformSetPosition(ctx, Vector3Subtract(bc->center, center));
  bc->lastCenter = bc->center;
}

void _BoxColliderDisplay(struct Actor *ctx)
{
  struct BoxCollider *bc = NULL;

  bc = ActorComponent(ctx, BoxCollider);
  DebugCube(bc->center, bc->size);
}

void _BoxColliderDestroy(struct Actor *ctx)
{
  struct BoxCollider *bc = NULL;

  bc = ActorComponent(ctx, BoxCollider);
  vector_delete(bc->collidingActors);
}

void BoxColliderInit(struct Actor *ctx)
{
  struct BoxCollider *bc = NULL;

  bc = ActorComponent(ctx, BoxCollider);
  bc->collidingActors = vector_new(struct Actor *);

  //ActorAddEvent(ctx, "display", _BoxColliderDisplay);
  ActorAddEvent(ctx, "tick", _BoxColliderTick);
  ActorAddEvent(ctx, "destroy", _BoxColliderDestroy);
}

struct Vector3 BoxColliderCenter(struct Actor *ctx)
{
  struct BoxCollider *bc = NULL;

  bc = ActorComponent(ctx, BoxCollider);

  return bc->center;
}

struct Vector3 BoxColliderSize(struct Actor *ctx)
{
  struct BoxCollider *bc = NULL;

  bc = ActorComponent(ctx, BoxCollider);

  return bc->size;
}

