#include "ModelCollider.h"
#include "Actor.h"
#include "ModelRenderer.h"
#include "math.h"
#include "Model.h"
#include "Gui.h"
#include "error.h"

#define COLUMNS 5
#define OVERLAP 1

struct CollisionColumn
{
  struct Vector3 size;
  struct Vector3 position;
  vector(struct Triangle) *triangles;
};

void _ModelColliderDestroy(struct Actor *ctx)
{
  struct ModelCollider *mc = NULL;
  size_t i = 0;

  mc = ActorComponent(ctx, ModelCollider);

  for(i = 0; i < vector_size(mc->columns); i++)
  {
    vector_delete(vector_at(mc->columns, i)->triangles);
    pfree(vector_at(mc->columns, i));
  }

  vector_delete(mc->columns);
}

void _ModelColliderBegin(struct Actor *ctx)
{
  struct ModelCollider *mc = NULL;
  struct ModelRenderer *mr = NULL;
  struct Vector3 columnSize = {0};
  size_t y = 0;
  vector(struct Triangle) *triangles = NULL;
  size_t cci = 0;
  size_t ti = 0;

  mc = ActorComponent(ctx, ModelCollider);
  mc->columns = vector_new(struct CollisionColumn *);
  mr = ActorComponent(ctx, ModelRenderer);
  mc->model = mr->model;

  columnSize = ModelSize(mr->model);
  columnSize.x /= COLUMNS;
  columnSize.z /= COLUMNS;

  for(y = 0; y < COLUMNS; y++)
  {
    size_t x = 0;

    for(x = 0; x < COLUMNS; x++)
    {
      struct CollisionColumn *cc = palloc(struct CollisionColumn);
      cc->size = columnSize;
      cc->position.x += ModelMin(mr->model).x + (x * cc->size.x) + (cc->size.x / 2.0f);
      cc->position.y += ModelMin(mr->model).y + (cc->size.y / 2.0f);
      cc->position.z += ModelMin(mr->model).z + (y * cc->size.z) + (cc->size.z / 2.0f);
      cc->size.x += OVERLAP;
      cc->size.y += OVERLAP;
      cc->size.z += OVERLAP;
      cc->triangles = vector_new(struct Triangle);
      vector_push_back(mc->columns, cc);
    }
  }

  mc->triangles = ModelTriangles(mr->model);
  triangles = ModelTriangles(mr->model);

  for(ti = 0; ti < vector_size(triangles); ti++)
  {
    struct Triangle t = vector_at(triangles, ti);
    int found = 0;

    for(cci = 0; cci < vector_size(mc->columns); cci++)
    {
      struct CollisionColumn *cc = vector_at(mc->columns, cci);

      //printf("Pos: %f %f %f\n", cc->position.x, cc->position.y, cc->position.z);
      //printf("Size: %f %f %f\n", cc->size.x, cc->size.y, cc->size.z);

      if(TriangleBoxOverlap(t, cc->position, cc->size))
      {
        found = 1;
        vector_push_back(cc->triangles, t);
      }
    }

    if(!found)
    {
      printf("A: %f %f %f\n", t.a.x, t.a.y, t.a.z);
      printf("B: %f %f %f\n", t.b.x, t.b.y, t.b.z);
      printf("C: %f %f %f\n", t.c.x, t.c.y, t.c.z);
      Error("Orphaned triangle found when generating spacial partitions");
    }
  }
}

void ModelColliderInit(struct Actor *ctx)
{
  ActorAddEvent(ctx, "begin", _ModelColliderBegin);
  ActorAddEvent(ctx, "destroy", _ModelColliderDestroy);
}

int _ModelColliderBoxOverlap(struct ModelCollider *mc, struct Vector3 position, struct Vector3 size)
{
  size_t cci = 0;
  size_t ti = 0;
  struct Vector3 offset = position;
  size_t x = 0;
  size_t z = 0;

  if(!mc->model) return 0;

  offset.x -= ModelMin(mc->model).x;
  offset.x *= (float)COLUMNS / ModelSize(mc->model).x;
  offset.z -= ModelMin(mc->model).z;
  offset.z *= (float)COLUMNS / ModelSize(mc->model).z;

  x = offset.x;
  z = offset.z;

  struct CollisionColumn *cc = vector_at(mc->columns, COLUMNS * z + x);
  vector(struct Triangle) *triangles = cc->triangles;

  for(ti = 0; ti < vector_size(triangles); ti++)
  {
    if(TriangleBoxOverlap(vector_at(triangles, ti), position, size))
    {
      return 1;
    }
  }

  return 0;
}

