#include <platform/platform.h>
#include <plank/plank.h>

struct Player
{
  sstream *debugText;
  int id;
  struct Animation *runAnim;
};

void PlayerSetId(struct Actor *ctx, int id);
int PlayerId(struct Actor *ctx);
