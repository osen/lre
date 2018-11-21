#include "RoomScreen.h"
#include "Player.h"
#include "Room.h"
#include "ScriptManager.h"
#include "Door.h"

#include <string.h>

void _RoomScreenTick(Actor *ctx)
{
  struct RoomScreen *is = NULL;

  is = ActorComponent(ctx, RoomScreen);
  is->timeout -= EnvironmentDeltaTime();

  if(is->timeout <= 0)
  {
    //Log("RoomScreen complete");
    //ActorDestroy(ctx);
  }
}

void _RoomScreenGui(Actor *ctx)
{
  struct RoomScreen *is = NULL;

  is = ActorComponent(ctx, RoomScreen);
  GuiImage(Vector4Xy(10, 10), is->logo);
  GuiText(Vector2Xy(150, 25), "LEGO Resident Evil");
  GuiText(Vector2Xy(150, 50), "Internal Prototype rev21");
}

void _RoomScreenBegin(Actor *ctx)
{
  //Log("Begin!");
}

void RoomScreenInit(Actor *ctx)
{
  struct RoomScreen *is = NULL;
  struct Actor *p1 = NULL;
  struct Actor *p2 = NULL;
  struct Actor *p3 = NULL;
  struct Actor *p4 = NULL;

  is = ActorComponent(ctx, RoomScreen);
  is->timeout = 10;

  is->logo = ImageLoad("images/logo");
  is->font = FontLoad("fonts/sum");

  ActorAddEvent(ctx, "tick", _RoomScreenTick);
  ActorAddEvent(ctx, "begin", _RoomScreenBegin);
  ActorAddEvent(ctx, "gui", _RoomScreenGui);

  GuiSetFont(is->font);

  return;

  p1 = ActorCreate();
  ActorAddComponent(p1, Player);

  p2 = ActorCreate();
  ActorAddComponent(p2, Player);
  PlayerSetId(p2, 1);
  TransformSetPosition(p2, Vector3Xyz(4, 0, 0));

  p3 = ActorCreate();
  ActorAddComponent(p3, Player);
  PlayerSetId(p3, 2);
  TransformSetPosition(p3, Vector3Xyz(0, 0, 4));

  p4 = ActorCreate();
  ActorAddComponent(p4, Player);
  PlayerSetId(p4, 3);
  TransformSetPosition(p4, Vector3Xyz(4, 0, 4));
}

void RoomScreenSetRoom(struct Actor *ctx, char *path, char *tag)
{
  struct Actor *room = NULL;
  struct Actor *player = NULL;

  room = ActorCreate();
  ActorAddComponent(room, Room);
  RoomSetPath(room, path);

  player = ActorCreate();
  ActorAddComponent(player, Player);

  if(strcmp(tag, "") != 0)
  {
    struct Actor *door = NULL;

    door = DoorsFindWithTag(tag);

    if(!door)
    {
      Error("Failed to find door with the specified tag");
    }
  }
}
