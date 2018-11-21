#include "RoomScreen.h"

#include <platform/platform.h>

int main(int argc, char *argv[])
{
  struct Actor *roomScreen = NULL;

  PlatformInitialize(argc, argv);

  roomScreen = ActorCreate();
  ActorAddComponent(roomScreen, RoomScreen);
  RoomScreenSetRoom(roomScreen, "rooms/entrance_hall_1", "");

  PlatformRun();

  PlatformCleanup();

  return 0;
}
