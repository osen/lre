#include <platform/platform.h>

struct Room
{
  sstream *path;
};

void RoomSetPath(struct Actor *ctx, char *path);
