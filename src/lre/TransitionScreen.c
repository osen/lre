#include "TransitionScreen.h"
#include "RoomScreen.h"

#include <platform/platform.h>

void _TransitionScreenGui(Actor *ctx)
{
  struct TransitionScreen *ts = NULL;

  ts = ActorComponent(ctx, TransitionScreen);
  GuiText(Vector2Xy(10, 200), sstream_cstr(ts->path));
  GuiText(Vector2Xy(10, 300), sstream_cstr(ts->tag));
}

void _TransitionScreenDestroy(struct Actor *ctx)
{
  struct TransitionScreen *ts = NULL;

  ts = ActorComponent(ctx, TransitionScreen);
  sstream_delete(ts->path);
  sstream_delete(ts->tag);
}

void _TransitionScreenTick(struct Actor *ctx)
{
  struct TransitionScreen *ts = NULL;

  ts = ActorComponent(ctx, TransitionScreen);
  ts->timeout -= TimeDelta();

  if(ts->timeout <= 0)
  {
    struct Actor *rs = NULL;
    sstream *path = NULL;
    sstream *tag = NULL;

    path = sstream_copy(ts->path);
    tag = sstream_copy(ts->tag);
    ActorsDestroy();
    rs = ActorCreate();
    ActorAddComponent(rs, RoomScreen);
    RoomScreenSetRoom(rs, sstream_cstr(path), sstream_cstr(tag));
    sstream_delete(path);
    sstream_delete(tag);
  }
}

void TransitionScreenInit(struct Actor *ctx)
{
  struct TransitionScreen *ts = NULL;
  struct Model *model = NULL;

  ts = ActorComponent(ctx, TransitionScreen);
  ts->timeout = 5;

  CameraSetPosition(Vector3Xyz(0, 0, 0));
  CameraSetRotation(Vector3Xyz(0, 0, 0));

  model = ModelLoad("models/curuthers/curuthers");
  ts->object = ActorCreate();
  ActorAddComponent(ts->object, ModelRenderer);
  ModelRendererSetModel(ts->object, model);
  TransformSetPosition(ts->object, Vector3Xyz(0, 0, -10));

  ts->path = sstream_new();
  ts->tag = sstream_new();

  ActorAddEvent(ctx, "tick", _TransitionScreenTick);
  ActorAddEvent(ctx, "gui", _TransitionScreenGui);
  ActorAddEvent(ctx, "destroy", _TransitionScreenDestroy);
}

void TransitionScreenSetTarget(struct Actor *ctx, sstream *path, sstream *tag)
{
  struct TransitionScreen *ts = NULL;

  ts = ActorComponent(ctx, TransitionScreen);
  sstream_clear(ts->path);
  sstream_clear(ts->tag);
  sstream_push_cstr(ts->path, sstream_cstr(path));
  sstream_push_cstr(ts->tag, sstream_cstr(tag));
}
