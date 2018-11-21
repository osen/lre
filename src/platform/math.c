#include "math.h"

#include <math.h>

struct Vector2 Vector2Xy(float x, float y)
{
  struct Vector2 rtn = {0};

  rtn.x = x;
  rtn.y = y;

  return rtn;
}

struct Vector3 Vector3Xyz(float x, float y, float z)
{
  struct Vector3 rtn = {0};

  rtn.x = x;
  rtn.y = y;
  rtn.z = z;

  return rtn;
}

struct Vector3 Vector3Add(struct Vector3 a, struct Vector3 b)
{
  struct Vector3 rtn = {0};

  rtn = a;
  rtn.x += b.x;
  rtn.y += b.y;
  rtn.z += b.z;

  return rtn;
}

struct Vector3 Vector3Subtract(struct Vector3 a, struct Vector3 b)
{
  struct Vector3 rtn = {0};

  rtn = a;
  rtn.x -= b.x;
  rtn.y -= b.y;
  rtn.z -= b.z;

  return rtn;
}

struct Vector3 Vector3Divide(struct Vector3 a, struct Vector3 b)
{
  struct Vector3 rtn = {0};

  rtn = a;
  rtn.x /= b.x;
  rtn.y /= b.y;
  rtn.z /= b.z;

  return rtn;
}

struct Vector3 Vector3Normalize(struct Vector3 ctx)
{
  float len = 0;

  len = sqrt(ctx.x*ctx.x+ctx.y*ctx.y+ctx.z*ctx.z);
  ctx.x/=len;
  ctx.y/=len;
  ctx.z/=len;

  return ctx;
}

float Vector3Distance(struct Vector3 a, struct Vector3 b)
{
  struct Vector3 t = {
    b.x - a.x,
    b.y - a.y,
    b.z - a.z
  };

  return sqrt(t.x * t.x + t.y * t.y + t.z * t.z);
}

struct Vector4 Vector4Xyzw(float x, float y, float z, float w)
{
  struct Vector4 rtn = {0};

  rtn.x = x;
  rtn.y = y;
  rtn.z = z;
  rtn.w = w;

  return rtn;
}

struct Vector4 Vector4Xy(float x, float y)
{
  struct Vector4 rtn = {0};

  rtn.x = x;
  rtn.y = y;

  return rtn;
}


int triBoxOverlap(float boxcenter[3],
  float boxhalfsize[3],float triverts[3][3]);

int TriangleBoxOverlap(struct Triangle ctx, struct Vector3 center, struct Vector3 size)
{
  float boxcenter[3] = {center.x, center.y, center.z};
  float boxhalfsize[3] = {size.x / 2.0f, size.y / 2.0f, size.z / 2.0f};
  float triverts[3][3] = {
    {ctx.a.x, ctx.a.y, ctx.a.z},
    {ctx.b.x, ctx.b.y, ctx.b.z},
    {ctx.c.x, ctx.c.y, ctx.c.z}
  };

  return triBoxOverlap(boxcenter, boxhalfsize, triverts);
}

struct Vector3 TriangleNormal(struct Triangle ctx)
{
  struct Vector3 rtn = {0};

  rtn.x = ctx.a.y*ctx.b.z-ctx.a.z*ctx.b.y;
  rtn.y = ctx.a.z*ctx.b.x-ctx.a.x*ctx.b.z;
  rtn.z = ctx.a.x*ctx.b.y-ctx.a.y*ctx.b.x;

  rtn = Vector3Normalize(rtn);

  return rtn;
}

int BoxBoxOverlap(struct Vector3 ap, struct Vector3 as, struct Vector3 bp, struct Vector3 bs)
{
  if(ap.x > bp.x)
  {
    if(ap.x - as.x / 2.0f > bp.x + bs.x / 2.0f)
    {
      return 0;
    }
  }
  else
  {
    if(bp.x - bs.x / 2.0f > ap.x + as.x / 2.0f)
    {
      return 0;
    }
  }

  if(ap.y > bp.y)
  {
    if(ap.y - as.y / 2.0f > bp.y + bs.y / 2.0f)
    {
      return 0;
    }
  }
  else
  {
    if(bp.y - bs.y / 2.0f > ap.y + as.y / 2.0f)
    {
      return 0;
    }
  }

  if(ap.z > bp.z)
  {
    if(ap.z - as.z / 2.0f > bp.z + bs.z / 2.0f)
    {
      return 0;
    }
  }
  else
  {
    if(bp.z - bs.z / 2.0f > ap.z + as.z / 2.0f)
    {
      return 0;
    }
  }

  return 1;
}
