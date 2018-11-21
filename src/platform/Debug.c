#include "Debug.h"
#include "math.h"
#include "Mesh.h"
#include "Image.h"
#include "Color.h"

#include <plank/plank.h>

#include <gluten/gluten.h>

struct Debug
{
  struct Mesh *cubeMesh;
  struct Image *redHighlight;
};

struct Debug *debug;

void _DebugInitialize()
{
  struct Face f = {0};

  debug = palloc(struct Debug);

  debug->cubeMesh = MeshCreate();

  /* Back */
  f.a.position = Vector3Xyz(-0.5f, -0.5f, -0.5f);
  f.a.texCoord = Vector2Xy(0, 0);
  f.b.position = Vector3Xyz(-0.5f, 0.5f, -0.5f);
  f.b.texCoord = Vector2Xy(0, 1);
  f.c.position = Vector3Xyz(0.5f, 0.5f, -0.5f);
  f.c.texCoord = Vector2Xy(1, 1);
  MeshAddFace(debug->cubeMesh, f);

  f.b = f.a;
  f.a = f.c;
  f.c = f.b;
  f.b.position = Vector3Xyz(0.5f, -0.5f, -0.5f);
  f.b.texCoord = Vector2Xy(1, 0);
  MeshAddFace(debug->cubeMesh, f);

  /* Front */
  f.a.position = Vector3Xyz(0.5f, 0.5f, 0.5f);
  f.a.texCoord = Vector2Xy(0, 0);
  f.b.position = Vector3Xyz(-0.5f, 0.5f, 00.5f);
  f.b.texCoord = Vector2Xy(0, 1);
  f.c.position = Vector3Xyz(0.5f, -0.5f, 0.5f);
  f.c.texCoord = Vector2Xy(1, 1);
  MeshAddFace(debug->cubeMesh, f);

  f.a.position = Vector3Xyz(-0.5f, -0.5f, 0.5f);
  f.a.texCoord = Vector2Xy(0, 0);
  f.b = f.c;
  f.c.position = Vector3Xyz(-0.5f, 0.5f, 0.5f);
  f.c.texCoord = Vector2Xy(0, 1);
  MeshAddFace(debug->cubeMesh, f);

  /* Left */
  f.a.position = Vector3Xyz(0.5f, -0.5f, -0.5f);
  f.a.texCoord = Vector2Xy(0.5f, 0);
  f.b.position = Vector3Xyz(0.5f, 0.5f, -0.5f);
  f.b.texCoord = Vector2Xy(0.5f, 1);
  f.c.position = Vector3Xyz(0.5f, 0.5f, 0.5f);
  f.c.texCoord = Vector2Xy(1, 1);
  MeshAddFace(debug->cubeMesh, f);

  f.b = f.a;
  f.a = f.c;
  f.c = f.b;
  f.b.position = Vector3Xyz(0.5f, -0.5f, 0.5f);
  f.b.texCoord = Vector2Xy(1, 0);
  MeshAddFace(debug->cubeMesh, f);

  /* Right */
  f.a.position = Vector3Xyz(-0.5f, -0.5f, -0.5f);
  f.a.texCoord = Vector2Xy(0, 0);
  f.b.position = Vector3Xyz(-0.5f, -0.5f, 0.5f);
  f.b.texCoord = Vector2Xy(0, 1);
  f.c.position = Vector3Xyz(-0.5f, 0.5f, 0.5f);
  f.c.texCoord = Vector2Xy(1, 1);
  MeshAddFace(debug->cubeMesh, f);

  f.a = f.c;
  f.b.position = Vector3Xyz(-0.5f, 0.5f, -0.5f);
  f.b.texCoord = Vector2Xy(1, 0);
  f.c.position = Vector3Xyz(-0.5f, -0.5f, -0.5f);
  f.c.texCoord = Vector2Xy(0, 0);
  MeshAddFace(debug->cubeMesh, f);

  debug->redHighlight = ImageCreate(2, 2);
  ImageSetPixel(debug->redHighlight, 0, 0, ColorRgba(255, 0, 0, 150));
  ImageSetPixel(debug->redHighlight, 1, 0, ColorRgba(255, 0, 0, 150));
  ImageSetPixel(debug->redHighlight, 0, 1, ColorRgba(255, 0, 0, 150));
  ImageSetPixel(debug->redHighlight, 1, 1, ColorRgba(255, 0, 0, 150));
}

void _DebugCleanup()
{
  ImageDestroy(debug->redHighlight);
  MeshDestroy(debug->cubeMesh);
  pfree(debug);
}

void DebugCube(struct Vector3 center, struct Vector3 size)
{
  //gnDisable(GL_CULL_FACE);
  //gnDisable(GL_DEPTH_TEST);
  _ImageBind(debug->redHighlight);
  gnPushMatrix();
  gnTranslatef(center.x, center.y, center.z);
  gnScalef(size.x, size.y, size.z);
  _MeshDisplay(debug->cubeMesh);
  gnPopMatrix();
  glBindTexture(GL_TEXTURE_2D, 0);
  //gnEnable(GL_CULL_FACE);
  //gnEnable(GL_DEPTH_TEST);
}

