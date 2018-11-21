#include "math.h"

#include <plank/plank.h>

typedef struct Animation Animation;

struct Transformation
{
  sstream *partName;
  struct Vector3 translation;
  struct Vector3 rotation;
};

struct Animation *AnimationLoad(char *path);
void _AnimationDestroy(struct Animation *ctx);
struct Transformation AnimationTransformation(struct Animation *ctx, char *partName, float frame);
int AnimationFrameCount(struct Animation *ctx);
