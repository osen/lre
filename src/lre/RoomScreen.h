#include <platform/platform.h>

struct RoomScreen
{
  float timeout;
  Image *logo;
  Font *font;
};

void RoomScreenSetRoom(struct Actor *ctx, char *path, char *tag);
