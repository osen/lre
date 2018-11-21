#include "Mesh.h"
#include "error.h"

#include <plank/plank.h>

#include <gluten/gluten.h>

#define BUFFER_OFFSET(bytes) ((GLubyte*)NULL + (bytes))

struct Mesh
{
  vector(struct Face) *faces;
  int dirty;
  praw(GLuint) buffer;
  struct Vector3 min;
  struct Vector3 max;
  struct Vector3 center;
};

struct Mesh *MeshCreate()
{
  struct Mesh *rtn = NULL;

  rtn = palloc(struct Mesh);
  rtn->faces = vector_new(struct Face);

  return rtn;
}

void MeshDestroy(struct Mesh *ctx)
{
  if(ctx->buffer)
  {
    glDeleteBuffers(1, ctx->buffer); pfree(ctx->buffer);
  }

  vector_delete(ctx->faces);
  pfree(ctx);
}

void MeshAddFace(struct Mesh *ctx, struct Face face)
{
  if(vector_size(ctx->faces) < 1)
  {
    ctx->max = face.a.position;
    ctx->min = face.a.position;
  }

  vector_push_back(ctx->faces, face);
  ctx->dirty = 1;

  if(face.a.position.x > ctx->max.x) ctx->max.x = face.a.position.x;
  if(face.a.position.y > ctx->max.y) ctx->max.y = face.a.position.y;
  if(face.a.position.z > ctx->max.z) ctx->max.z = face.a.position.z;
  if(face.b.position.x > ctx->max.x) ctx->max.x = face.b.position.x;
  if(face.b.position.y > ctx->max.y) ctx->max.y = face.b.position.y;
  if(face.b.position.z > ctx->max.z) ctx->max.z = face.b.position.z;
  if(face.c.position.x > ctx->max.x) ctx->max.x = face.c.position.x;
  if(face.c.position.y > ctx->max.y) ctx->max.y = face.c.position.y;
  if(face.c.position.z > ctx->max.z) ctx->max.z = face.c.position.z;

  if(face.a.position.x < ctx->min.x) ctx->min.x = face.a.position.x;
  if(face.a.position.y < ctx->min.y) ctx->min.y = face.a.position.y;
  if(face.a.position.z < ctx->min.z) ctx->min.z = face.a.position.z;
  if(face.b.position.x < ctx->min.x) ctx->min.x = face.b.position.x;
  if(face.b.position.y < ctx->min.y) ctx->min.y = face.b.position.y;
  if(face.b.position.z < ctx->min.z) ctx->min.z = face.b.position.z;
  if(face.c.position.x < ctx->min.x) ctx->min.x = face.c.position.x;
  if(face.c.position.y < ctx->min.y) ctx->min.y = face.c.position.y;
  if(face.c.position.z < ctx->min.z) ctx->min.z = face.c.position.z;

  ctx->center.x = (ctx->min.x + ctx->max.x) / 2.0f;
  ctx->center.y = (ctx->min.y + ctx->max.y) / 2.0f;
  ctx->center.z = (ctx->min.z + ctx->max.z) / 2.0f;
}

void _MeshShift(struct Mesh *ctx, struct Vector3 shift)
{
  size_t i = 0;

  ctx->dirty = 1;

  for(i = 0; i < vector_size(ctx->faces); i++)
  {
    struct Face f = vector_at(ctx->faces, i);

    f.a.position.x += shift.x;
    f.a.position.y += shift.y;
    f.a.position.z += shift.z;

    f.b.position.x += shift.x;
    f.b.position.y += shift.y;
    f.b.position.z += shift.z;

    f.c.position.x += shift.x;
    f.c.position.y += shift.y;
    f.c.position.z += shift.z;

    vector_set(ctx->faces, i, f);
  }
}

void _MeshDisplay(struct Mesh *ctx)
{
  if(ctx->dirty)
  {
    vector(GLfloat) *data = NULL;
    size_t i = 0;

    if(vector_size(ctx->faces) < 1)
    {
      Error("Attempting to display empty mesh");
    }

    data = vector_new(GLfloat);

    for(i = 0; i < vector_size(ctx->faces); i++)
    {
      struct Face f = vector_at(ctx->faces, i);

      vector_push_back(data, f.a.position.x);
      vector_push_back(data, f.a.position.y);
      vector_push_back(data, f.a.position.z);
      vector_push_back(data, f.a.texCoord.x);
      vector_push_back(data, f.a.texCoord.y);
      vector_push_back(data, f.a.normal.x);
      vector_push_back(data, f.a.normal.y);
      vector_push_back(data, f.a.normal.z);

      vector_push_back(data, f.b.position.x);
      vector_push_back(data, f.b.position.y);
      vector_push_back(data, f.b.position.z);
      vector_push_back(data, f.b.texCoord.x);
      vector_push_back(data, f.b.texCoord.y);
      vector_push_back(data, f.b.normal.x);
      vector_push_back(data, f.b.normal.y);
      vector_push_back(data, f.b.normal.z);

      vector_push_back(data, f.c.position.x);
      vector_push_back(data, f.c.position.y);
      vector_push_back(data, f.c.position.z);
      vector_push_back(data, f.c.texCoord.x);
      vector_push_back(data, f.c.texCoord.y);
      vector_push_back(data, f.c.normal.x);
      vector_push_back(data, f.c.normal.y);
      vector_push_back(data, f.c.normal.z);
    }

    //for(i = 0; i < vector_size(data); i++)
    //{
    //  printf("D: %f\n", vector_at(data, i));
    //}

    //printf("Mesh Center: %f %f %f\n", ctx->center.x, ctx->center.y, ctx->center.z);

    if(!ctx->buffer)
    {
      ctx->buffer = palloc(GLuint);
      glGenBuffers(1, ctx->buffer);
    }

    glBindBuffer(GL_ARRAY_BUFFER, *ctx->buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vector_size(data), vector_data(data), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    vector_delete(data);
    ctx->dirty = 0;
  }

  glBindBuffer(GL_ARRAY_BUFFER, *ctx->buffer);
  gnVertexPointer(3, GL_FLOAT, 8 * sizeof(GLfloat), 0);
  gnTexCoordPointer(2, GL_FLOAT, 8 * sizeof(GLfloat), BUFFER_OFFSET(3 * sizeof(float)));
  gnNormalPointer(GL_FLOAT, 8 * sizeof(GLfloat), BUFFER_OFFSET(5 * sizeof(float)));
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  gnDrawArrays(GL_TRIANGLES, 0, vector_size(ctx->faces) * 3);
}

struct Vector3 MeshCenter(struct Mesh *ctx)
{
  return ctx->center;
}

struct Vector3 MeshMax(struct Mesh *ctx)
{
  return ctx->max;
}

struct Vector3 MeshMin(struct Mesh *ctx)
{
  return ctx->min;
}
