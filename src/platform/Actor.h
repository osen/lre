#include <plank/plank.h>

typedef struct Actor Actor;

extern vector(struct Actor *) *_actors;

struct Actor *ActorCreate();
void _ActorDestroy(struct Actor *ctx);
void ActorDestroy(struct Actor *ctx);

void *_ActorAddComponent(struct Actor *ctx, size_t size, char *type, void (*initFunc)(struct Actor *));
#define ActorAddComponent(W, T) \
{ \
  void T##Init(struct Actor *ctx); \
  (struct T*)_ActorAddComponent(W, sizeof(struct T), #T, T##Init); \
}

void ActorEvent(struct Actor *ctx, char *event);

void ActorAddEvent(struct Actor *ctx, char *event,
  void (*func)(struct Actor *));

void *_ActorComponent(struct Actor *ctx, char *type);
#define ActorComponent(W, T) \
  (struct T*)_ActorComponent(W, #T)

int _ActorHasComponent(struct Actor *ctx, char *type);
#define ActorHasComponent(W, T) \
  _ActorHasComponent(W, #T)

void _ActorsWithComponent(char *type, vector(struct Actor *) *actors);
#define ActorsWithComponent(T, A) \
  _ActorsWithComponent(#T, A)

int ActorAlive(struct Actor *ctx);

void ActorsDestroy();
