#include "Room.h"
#include "CameraManager.h"
#include "ScriptManager.h"

void _RoomDestroy(Actor *ctx)
{
  struct Room *r = NULL;

  r = ActorComponent(ctx, Room);
  sstream_delete(r->path);
}

void RoomInit(Actor *ctx)
{
  struct Room *r = NULL;

  r = ActorComponent(ctx, Room);
  r->path = sstream_new();

  ActorAddComponent(ctx, CameraManager);

  ActorAddEvent(ctx, "destroy", _RoomDestroy);
}

void RoomSetPath(struct Actor *ctx, char *path)
{
  struct Room *r = NULL;

  r = ActorComponent(ctx, Room);
  sstream_clear(r->path);
  sstream_push_cstr(r->path, path);

  ActorAddComponent(ctx, ScriptManager);
}

