#include <platform/platform.h>
#include <plank/plank.h>

struct CameraInfo
{
  struct Vector3 position;
  struct Vector3 rotation;
  struct Vector3 target;
};

struct CameraManager
{
  vector(struct CameraInfo) *cameras;
  vector(struct Actor *) *players;
};

void CameraManagerAddTarget(struct Actor *ctx, struct Vector3 target,
  struct Vector3 position, struct Vector3 rotation);
