#include "Player.h"
#include "Door.h"
#include "TransitionScreen.h"

void _PlayerTick(Actor *ctx)
{
  struct Player *p = NULL;
  int animSelector = 0;

  p = ActorComponent(ctx, Player);
  //if(p->id != 0) return;

  if(InputKey('q'))
  {
    ActorsDestroy();
  }

  if(InputKey('e'))
  {
    struct Actor *door = NULL;

    door = DoorsPlayerCollide(ctx);

    if(door)
    {
      struct Actor *a = NULL;
      sstream *path = NULL;
      sstream *tag = NULL;

      path = sstream_copy(DoorTargetPath(door));
      tag = sstream_copy(DoorTargetTag(door));
      ActorsDestroy();
      a = ActorCreate();
      ActorAddComponent(a, TransitionScreen);
      TransitionScreenSetTarget(a, path, tag);
      sstream_delete(path);
      sstream_delete(tag);
    }
  }

  if(InputKey('w'))
  {
    TransformTranslate(ctx, Vector3Xyz(0, 0, 3.0f * TimeDelta()));
    animSelector = 1;
  }
  else if(InputKey('s'))
  {
    TransformTranslate(ctx, Vector3Xyz(0, 0, -3.0f * TimeDelta()));
  }

  if(InputKey('a'))
  {
    TransformRotate(ctx, Vector3Xyz(0, 90 * TimeDelta(), 0));
    animSelector = 1;
  }
  else if(InputKey('d'))
  {
    TransformRotate(ctx, Vector3Xyz(0, -90 * TimeDelta(), 0));
    animSelector = 1;
  }

  if(animSelector == 1)
  {
    ModelRendererAttachAnimation(ctx, p->runAnim);
  }
  else
  {
    ModelRendererAttachAnimation(ctx, NULL);
  }
}

void _PlayerGui(Actor *ctx)
{
  struct Player *p = NULL;
  struct Vector3 pos = {0};

  p = ActorComponent(ctx, Player);
  if(p->id != 0) return;

  pos = TransformPosition(ctx);
  sstream_clear(p->debugText);
  sstream_push_cstr(p->debugText, "X: ");
  sstream_push_int(p->debugText, (int)pos.x);
  sstream_push_cstr(p->debugText, " Y: ");
  sstream_push_int(p->debugText, (int)pos.y);
  sstream_push_cstr(p->debugText, " Z: ");
  sstream_push_int(p->debugText, (int)pos.z);
  GuiText(Vector2Xy(10, 200), sstream_cstr(p->debugText));
}

void _PlayerDestroy(Actor *ctx)
{
  struct Player *p = NULL;

  p = ActorComponent(ctx, Player);
  sstream_delete(p->debugText);
}

void _PlayerBegin(Actor *ctx)
{
  struct Model *model = NULL;
  struct Player *p = NULL;

  p = ActorComponent(ctx, Player);

  if(p->id == 0)
  {
    model = ModelLoad("models/harry/harry");
    p->runAnim = AnimationLoad("models/harry/run");
  }
  else if(p->id == 1)
  {
    model = ModelLoad("models/captain/captain");
    p->runAnim = AnimationLoad("models/captain/run");
  }
  else
  {
    model = ModelLoad("models/darth/darth");
    p->runAnim = AnimationLoad("models/darth/run");
  }

  ActorAddComponent(ctx, Gravity);
  ActorAddComponent(ctx, BoxCollider);
  ActorAddComponent(ctx, ModelRenderer);

  ModelRendererSetModel(ctx, model);
  ModelRendererSetFps(ctx, 6);
  p->debugText = sstream_new();
}

void PlayerInit(Actor *ctx)
{
  ActorAddEvent(ctx, "begin", _PlayerBegin);
  ActorAddEvent(ctx, "tick", _PlayerTick);
  ActorAddEvent(ctx, "gui", _PlayerGui);
  ActorAddEvent(ctx, "destroy", _PlayerDestroy);
}

void PlayerSetId(struct Actor *ctx, int id)
{
  struct Player *p = NULL;

  p = ActorComponent(ctx, Player);
  p->id = id;
}

int PlayerId(struct Actor *ctx)
{
  struct Player *p = NULL;

  p = ActorComponent(ctx, Player);
  return p->id;
}
