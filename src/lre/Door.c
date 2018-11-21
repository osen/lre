#include "Door.h"

#include <string.h>

void _DoorDisplay(struct Actor *ctx)
{
  struct Door *d = NULL;

  d = ActorComponent(ctx, Door);
  DebugCube(TransformPosition(ctx), d->size);
}

void _DoorDestroy(struct Actor *ctx)
{
  struct Door *d = NULL;

  d = ActorComponent(ctx, Door);
  sstream_delete(d->targetTag);
  sstream_delete(d->targetPath);
  sstream_delete(d->tag);
}

void DoorInit(struct Actor *ctx)
{
  struct Door *d = NULL;

  d = ActorComponent(ctx, Door);
  d->size = Vector3Xyz(1, 2, 1);
  d->targetPath = sstream_new();
  d->targetTag = sstream_new();
  d->tag = sstream_new();

  ActorAddEvent(ctx, "display", _DoorDisplay);
  ActorAddEvent(ctx, "destroy", _DoorDestroy);
}

void DoorSetSize(struct Actor *ctx, struct Vector3 size)
{
  struct Door *d = NULL;

  d = ActorComponent(ctx, Door);
  d->size = size;
}

struct Actor *DoorsPlayerCollide(struct Actor *player)
{
  vector(struct Actor *) *doors = NULL;
  size_t di = 0;

  doors = vector_new(struct Actor *);

  ActorsWithComponent(Door, doors);

  for(di = 0; di < vector_size(doors); di++)
  {
    struct Actor *door = vector_at(doors, di);
    struct Vector3 ap = {0};
    struct Vector3 as = {0};
    struct Vector3 bp = {0};
    struct Vector3 bs = {0};

    ap = BoxColliderCenter(player);
    as = BoxColliderSize(player);
    bp = TransformPosition(door);
    bs = DoorSize(door);

    if(BoxBoxOverlap(ap, as, bp, bs))
    {
      vector_delete(doors);

      return door;
    }
  }

  vector_delete(doors);

  return NULL;
}

struct Actor *DoorsFindWithTag(char *tag)
{
  vector(struct Actor *) *doors = NULL;
  size_t di = 0;

  doors = vector_new(struct Actor *);

  ActorsWithComponent(Door, doors);

  for(di = 0; di < vector_size(doors); di++)
  {
    struct Actor *door = vector_at(doors, di);

    if(strcmp(tag, sstream_cstr(DoorTag(door))) == 0)
    {
      vector_delete(doors);

      return door;
    }
  }

  vector_delete(doors);

  return NULL;
}

struct Vector3 DoorSize(struct Actor *ctx)
{
  struct Door *d = NULL;

  d = ActorComponent(ctx, Door);

  return d->size;
}

void DoorSetTarget(struct Actor *ctx, sstream *path, sstream *tag)
{
  struct Door *d = NULL;

  d = ActorComponent(ctx, Door);
  sstream_clear(d->targetPath);
  sstream_push_cstr(d->targetPath, sstream_cstr(path));
  sstream_clear(d->targetTag);
  sstream_push_cstr(d->targetTag, sstream_cstr(tag));
}

void DoorSetTag(struct Actor *ctx, sstream *tag)
{
  struct Door *d = NULL;

  d = ActorComponent(ctx, Door);
  sstream_clear(d->tag);
  sstream_push_cstr(d->tag, sstream_cstr(tag));
}

sstream *DoorTargetPath(struct Actor *ctx)
{
  struct Door *d = NULL;

  d = ActorComponent(ctx, Door);

  return d->targetPath;
}

sstream *DoorTargetTag(struct Actor *ctx)
{
  struct Door *d = NULL;

  d = ActorComponent(ctx, Door);

  return d->targetTag;
}

sstream *DoorTag(struct Actor *ctx)
{
  struct Door *d = NULL;

  d = ActorComponent(ctx, Door);

  return d->tag;
}
