#include "Actor.h"
#include "Transform.h"
#include "error.h"

#include <stdlib.h>
#include <string.h>

struct ComponentEntry
{
  sstream *type;
  void *data;
};

struct EventEntry
{
  sstream *name;
  void (*func)(struct Actor *);
  int began;
  int destroyEvent;
};

struct Actor
{
  vector(struct ComponentEntry) *components;
  vector(struct EventEntry) *events;
  int active;
  int alive;
  int began;
  vector(sstream *) *tags;
};

vector(struct Actor *) *_actors;

struct Actor *ActorCreate()
{
  struct Actor *rtn = NULL;

  rtn = palloc(struct Actor);
  rtn->tags = vector_new(sstream *);
  rtn->components = vector_new(struct ComponentEntry);
  rtn->events = vector_new(struct EventEntry);
  rtn->alive = 1;
  rtn->active = 1;

  ActorAddComponent(rtn, Transform);

  vector_push_back(_actors, rtn);

  return rtn;
}

void ActorDestroy(struct Actor *ctx)
{
  ActorEvent(ctx, "destroy");
  ctx->alive = 0;
}

void _ActorDestroy(struct Actor *ctx)
{
  size_t i = 0;

  for(i = 0; i < vector_size(ctx->events); i++)
  {
    sstream_delete(vector_at(ctx->events, i).name);
  }

  vector_delete(ctx->events);

  for(i = 0; i < vector_size(ctx->components); i++)
  {
    pfree(vector_at(ctx->components, i).data); 
    sstream_delete(vector_at(ctx->components, i).type);
  }

  vector_delete(ctx->components);
  vector_delete(ctx->tags);
  pfree(ctx);
}

void *_ActorAddComponent(struct Actor *ctx, size_t size, char *type, void (*initFunc)(struct Actor *))
{
  void *rtn = NULL;
  struct ComponentEntry entry = {0};

  rtn = _palloc(size, type);
  entry.data = rtn;
  entry.type = sstream_new();
  sstream_push_cstr(entry.type, type);
  vector_push_back(ctx->components, entry);
  initFunc(ctx);

  return rtn;
}

void ActorAddEvent(struct Actor *ctx, char *event,
  void (*func)(struct Actor *))
{
  struct EventEntry entry = {0};

  entry.name = sstream_new();
  sstream_push_cstr(entry.name, event);
  entry.func = func;

  if(strcmp(event, "begin") == 0)
  {
    ctx->began = 0;
  }
  else if(strcmp(event, "destroy") == 0)
  {
    entry.began = 1;
    entry.destroyEvent = 1;
  }

  vector_push_back(ctx->events, entry);
}

void ActorEvent(struct Actor *ctx, char *event)
{
  size_t i = 0;

  if(!ctx->alive) return;
  if(!ctx->active) return;

  if(!ctx->began)
  {
    if(strcmp(event, "tick") == 0)
    {
      ctx->began = 1;

      for(i = 0; i < vector_size(ctx->events); i++)
      {
        struct EventEntry ee = vector_at(ctx->events, i);

        if(!ee.began)
        {
          if(strcmp(sstream_cstr(ee.name), "begin") == 0)
          {
            if(ctx->alive || ee.destroyEvent)
            {
              ee.func(ctx);
            }
          }

          ee.began = 1;
          vector_set(ctx->events, i, ee);
        }
      }
    }
  }

  for(i = 0; i < vector_size(ctx->events); i++)
  {
    struct EventEntry ee = vector_at(ctx->events, i);

    if(!ctx->began && !ee.began) continue;

    if(strcmp(sstream_cstr(ee.name), event) == 0)
    {
      if(ctx->alive || ee.destroyEvent)
      {
        ee.func(ctx);
      }

      if(!ee.began)
      {
        ee.began = 1;
        vector_set(ctx->events, i, ee);
      }
    }
  }
}

void *_ActorComponent(struct Actor *ctx, char *type)
{
  size_t i = 0;

  for(i = 0; i < vector_size(ctx->components); i++)
  {
    if(strcmp(type, sstream_cstr(vector_at(ctx->components, i).type)) == 0)
    {
      return vector_at(ctx->components, i).data;
    }
  }

  Error("Actor does not contain the specified component");
  return NULL;
}

int _ActorHasComponent(struct Actor *ctx, char *type)
{
  size_t i = 0;

  for(i = 0; i < vector_size(ctx->components); i++)
  {
    if(strcmp(type, sstream_cstr(vector_at(ctx->components, i).type)) == 0)
    {
      return 1;
    }
  }

  return 0;
}

int ActorAlive(struct Actor *ctx)
{
  return ctx->alive;
}

void _ActorsWithComponent(char *type, vector(struct Actor *) *actors)
{
  size_t ai = 0;

  for(ai = 0; ai < vector_size(_actors); ai++)
  {
    size_t ci = 0;
    struct Actor *actor = vector_at(_actors, ai);

    if(!actor->alive) continue;

    for(ci = 0; ci < vector_size(actor->components); ci++)
    {
      struct ComponentEntry component = vector_at(actor->components, ci);

      if(strcmp(type, sstream_cstr(component.type)) == 0)
      {
        vector_push_back(actors, actor);
        break;
      }
    }
  }
}

void ActorsDestroy()
{
  size_t ai = 0;

  for(ai = 0; ai < vector_size(_actors); ai++)
  {
    struct Actor *a = vector_at(_actors, ai);
    ActorDestroy(a);
  }
}
