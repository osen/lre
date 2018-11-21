#include <platform/platform.h>

#include <plank/plank.h>

struct Door
{
  struct Vector3 size;
  sstream *tag;
  sstream *targetPath;
  sstream *targetTag;
};

void DoorSetSize(struct Actor *ctx, struct Vector3 size);
struct Vector3 DoorSize(struct Actor *ctx);
void DoorSetTag(struct Actor *ctx, sstream *tag);
void DoorSetTarget(struct Actor *ctx, sstream *path, sstream *tag);
sstream *DoorTargetPath(struct Actor *ctx);
sstream *DoorTargetTag(struct Actor *ctx);
sstream *DoorTag(struct Actor *ctx);

struct Actor *DoorsPlayerCollide(struct Actor *player);
struct Actor *DoorsFindWithTag(char *tag);
