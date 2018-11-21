#include "ScriptManager.h"
#include "Room.h"
#include "Door.h"
#include "CameraManager.h"

#include <platform/platform.h>

#include <string.h>

void _ScriptManagerProcessCommand(struct Actor *ctx, struct ScriptManager *sm);

void _ScriptManagerExecute(struct Actor *ctx)
{
  struct ScriptManager *sm = NULL;
  sstream *line = NULL;

  sm = ActorComponent(ctx, ScriptManager);

  while(1)
  {
    if(sm->counter >= vector_size(sm->lines)) return;

    line = vector_at(sm->lines, sm->counter);
    sstream_splitwhitespace(line, sm->command);
    _ScriptManagerProcessCommand(ctx, sm);
    sm->counter++;
  }
}

void _ScriptManagerParseRoom(struct Actor *ctx)
{
  praw(struct HdFile *) file = NULL;
  sstream *line = NULL;
  sstream *path = NULL;
  struct ScriptManager *sm = NULL;
  struct Room *r = NULL;

  sm = ActorComponent(ctx, ScriptManager);
  r = ActorComponent(ctx, Room);
  path = sstream_new();
  sstream_push_cstr(path, "assets/");
  sstream_push_cstr(path, sstream_cstr(r->path));
  sstream_push_cstr(path, ".dat");

  line = sstream_new();
  file = palloc(struct HdFile *);
  *file = HdFileOpen(sstream_cstr(path));
  sstream_delete(path);

  sm->command = vector_new(sstream *);
  sm->lines = vector_new(sstream *);

  if(!*file)
  {
    Error("Failed to open room for loading");
  }

  while(fgetline(*file, line))
  {
    sstream *newLine = NULL;

    if(sstream_length(line) < 1) continue;

    newLine = sstream_new();
    sstream_push_cstr(newLine, sstream_cstr(line));
    vector_push_back(sm->lines, newLine);
  }

  sstream_delete(line);
  HdFileClose(*file); pfree(file);
}

void _ScriptManagerAddDoor(struct Actor *ctx, struct ScriptManager *sm,
  sstream *tag, struct Vector3 position, struct Vector3 direction, struct Vector3 size, sstream *targetPath, sstream *targetTag)
{
  struct Actor *door = NULL;

  //printf("Added door: %s\n", sstream_cstr(tag));
  //Log("Adding door");
  door = ActorCreate();
  ActorAddComponent(door, Door);
  TransformSetPosition(door, position);
  TransformSetRotation(door, direction);
  DoorSetTag(door, tag);
  DoorSetSize(door, size);
  DoorSetTarget(door, targetPath, targetTag);
}

void _ScriptManagerProcessCommand(struct Actor *ctx, struct ScriptManager *sm)
{
  sstream *cmd = NULL;

  cmd = vector_at(sm->command, 0);
  //printf("%s\n", sstream_cstr(cmd));

  if(strcmp("map", sstream_cstr(cmd)) == 0)
  {
    sstream *path = vector_at(sm->command, 1);

    ActorAddComponent(ctx, ModelRenderer);
    ModelRendererSetModel(ctx, ModelLoad(sstream_cstr(path)));
    ModelRendererSetUnshaded(ctx, 1);
    ActorAddComponent(ctx, ModelCollider);
  }
  else if(strcmp("camera", sstream_cstr(cmd)) == 0)
  {
    sstream *tag = NULL;
    struct Vector3 target = {0};
    struct Vector3 position = {0};
    struct Vector3 rotation = {0};

    tag = vector_at(sm->command, 1);
    target.x = atof(sstream_cstr(vector_at(sm->command, 2)));
    target.y = atof(sstream_cstr(vector_at(sm->command, 3)));
    target.z = atof(sstream_cstr(vector_at(sm->command, 4)));
    position.x = atof(sstream_cstr(vector_at(sm->command, 5)));
    position.y = atof(sstream_cstr(vector_at(sm->command, 6)));
    position.z = atof(sstream_cstr(vector_at(sm->command, 7)));
    rotation.x = atof(sstream_cstr(vector_at(sm->command, 8)));
    rotation.y = atof(sstream_cstr(vector_at(sm->command, 9)));
    rotation.z = atof(sstream_cstr(vector_at(sm->command, 10)));

    CameraManagerAddTarget(ctx, target, position, rotation);
  }
  else if(strcmp("door", sstream_cstr(cmd)) == 0)
  {
    sstream *tag = NULL;
    struct Vector3 position = {0};
    struct Vector3 direction = {0};
    struct Vector3 size = {0};

    tag = vector_at(sm->command, 1);
    position.x = atof(sstream_cstr(vector_at(sm->command, 2)));
    position.y = atof(sstream_cstr(vector_at(sm->command, 3)));
    position.z = atof(sstream_cstr(vector_at(sm->command, 4)));
    direction.x = atof(sstream_cstr(vector_at(sm->command, 5)));
    direction.y = atof(sstream_cstr(vector_at(sm->command, 6)));
    direction.z = atof(sstream_cstr(vector_at(sm->command, 7)));
    size.x = atof(sstream_cstr(vector_at(sm->command, 8)));
    size.y = atof(sstream_cstr(vector_at(sm->command, 9)));
    size.z = atof(sstream_cstr(vector_at(sm->command, 10)));

    _ScriptManagerAddDoor(ctx, sm, tag, position, direction, size,
      vector_at(sm->command, 11), vector_at(sm->command, 12));
  }
}

void _ScriptManagerTick(struct Actor *ctx)
{
  _ScriptManagerExecute(ctx);
}

void _ScriptManagerDestroy(struct Actor *ctx)
{
  struct ScriptManager *sm = NULL;
  size_t i = 0;

  sm = ActorComponent(ctx, ScriptManager);
  vector_delete(sm->actors);
  vector_delete(sm->doors);

  for(i = 0; i < vector_size(sm->lines); i++)
  {
    sstream_delete(vector_at(sm->lines, i));
  }

  vector_delete(sm->lines);

  for(i = 0; i < vector_size(sm->command); i++)
  {
    sstream_delete(vector_at(sm->command, i));
  }

  vector_delete(sm->command);
}

void ScriptManagerInit(struct Actor *ctx)
{
  struct ScriptManager *sm = NULL;

  sm = ActorComponent(ctx, ScriptManager);
  sm->actors = vector_new(struct Actor *);
  sm->doors = vector_new(struct Actor *);

  _ScriptManagerParseRoom(ctx);
  _ScriptManagerExecute(ctx);

  //ActorAddEvent(ctx, "begin", _ScriptManagerBegin);
  ActorAddEvent(ctx, "tick", _ScriptManagerTick);
  ActorAddEvent(ctx, "destroy", _ScriptManagerDestroy);
}

