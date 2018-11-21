#ifndef PLATFORM_MATH_H
#define PLATFORM_MATH_H

struct Vector2
{
  float x;
  float y;
};

typedef struct Vector2 Vector2;

struct Vector2 Vector2Xy(float x, float y);

struct Vector3
{
  float x;
  float y;
  float z;
};

typedef struct Vector3 Vector3;

struct Vector3 Vector3Xyz(float x, float y, float z);
struct Vector3 Vector3Add(struct Vector3 a, struct Vector3 b);
struct Vector3 Vector3Subtract(struct Vector3 a, struct Vector3 b);
struct Vector3 Vector3Divide(struct Vector3 a, struct Vector3 b);
float Vector3Distance(struct Vector3 a, struct Vector3 b);
struct Vector3 Vector3Normalize(struct Vector3 ctx);

struct Vector4
{
  float x;
  float y;
  float z;
  float w;
};

typedef struct Vector4 Vector4;

struct Vector4 Vector4Xyzw(float x, float y, float z, float w);
struct Vector4 Vector4Xy(float x, float y);

struct Triangle
{
  struct Vector3 a;
  struct Vector3 b;
  struct Vector3 c;
};

int TriangleBoxOverlap(struct Triangle ctx, struct Vector3 center, struct Vector3 size);
struct Vector3 TriangleNormal(struct Triangle ctx);

int BoxBoxOverlap(struct Vector3 ap, struct Vector3 as, struct Vector3 bp, struct Vector3 bs);

#endif
