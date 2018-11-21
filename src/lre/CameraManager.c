#include "CameraManager.h"
#include "Player.h"

void _CameraManagerDestroy(struct Actor *ctx)
{
  struct CameraManager *cm = NULL;

  cm = ActorComponent(ctx, CameraManager);
  vector_delete(cm->cameras);
  vector_delete(cm->players);
}

void _CameraManagerTick(struct Actor *ctx)
{
  size_t pi = 0;
  struct Vector3 pos = {0};
  size_t ci = 0;
  struct CameraManager *cm = NULL;
  float closest = 0;
  struct CameraInfo cci = {0};

  cm = ActorComponent(ctx, CameraManager);
  vector_clear(cm->players);
  ActorsWithComponent(Player, cm->players);

  if(vector_size(cm->players) < 1)
  {
    return;
  }

  pos = TransformPosition(vector_at(cm->players, 0));

  for(pi = 1; pi < vector_size(cm->players); pi++)
  {
    struct Actor *player = vector_at(cm->players, pi);

    //if(PlayerId(player) != 0) continue;
    pos = Vector3Add(pos, TransformPosition(player));
  }

  pos = Vector3Divide(pos, Vector3Xyz(
    vector_size(cm->players),
    vector_size(cm->players),
    vector_size(cm->players)));

  if(vector_size(cm->cameras) < 1)
  {
    return;
  }

  closest = Vector3Distance(pos, vector_at(cm->cameras, 0).target);
  cci = vector_at(cm->cameras, 0);

  for(ci = 0; ci < vector_size(cm->cameras); ci++)
  {
    float dist = Vector3Distance(pos, vector_at(cm->cameras, ci).target);

    if(dist < closest)
    {
      cci = vector_at(cm->cameras, ci);
      closest = dist;
    }
  }

  CameraSetPosition(cci.position);
  CameraSetRotation(cci.rotation);
}

void CameraManagerInit(struct Actor *ctx)
{
  struct CameraManager *cm = NULL;

  cm = ActorComponent(ctx, CameraManager);
  cm->cameras = vector_new(struct CameraInfo);
  cm->players = vector_new(struct Actor *);

  ActorAddEvent(ctx, "destroy", _CameraManagerDestroy);
  ActorAddEvent(ctx, "tick", _CameraManagerTick);
}

void CameraManagerAddTarget(struct Actor *ctx, struct Vector3 target,
  struct Vector3 position, struct Vector3 rotation)
{
  struct CameraManager *cm = NULL;
  struct CameraInfo ci = {0};

  cm = ActorComponent(ctx, CameraManager);
  ci.target = target;
  ci.position = position;
  ci.rotation = rotation;
  vector_push_back(cm->cameras, ci);
}
