#include <plank/plank.h>

struct Triangle;
struct CollisionColumn;
struct Vector3;
struct Model;

struct ModelCollider
{
  vector(struct Triangle) *triangles;
  vector(struct CollisionColumn *) *columns;
  struct Model *model;
};

int _ModelColliderBoxOverlap(struct ModelCollider *mc, struct Vector3 position, struct Vector3 size);
