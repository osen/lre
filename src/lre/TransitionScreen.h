#include <platform/platform.h>

#include <plank/plank.h>

struct TransitionScreen
{
  struct Actor *object;
  sstream *path;
  sstream *tag;
  int type;
  float timeout;
};

void TransitionScreenSetTarget(struct Actor *ctx, sstream *path, sstream *tag);

