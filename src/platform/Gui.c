#include "Gui.h"
#include "Mesh.h"
#include "Image.h"
#include "Font.h"
#include "math.h"

#include <plank/plank.h>

#include <gluten/gluten.h>

struct Gui
{
  struct Mesh *mesh;
  struct Font *font;
  struct Vector2 size;
};

struct Gui *gui;

void _GuiInitialize()
{
  struct Face f = {0};

  gui = palloc(struct Gui);
  gui->mesh = MeshCreate();

  f.a.position = Vector3Xyz(0, 0, 0);
  f.a.texCoord = Vector2Xy(0, 0);
  f.b.position = Vector3Xyz(0, 1, 0);
  f.b.texCoord = Vector2Xy(0, 1);
  f.c.position = Vector3Xyz(1, 1, 0);
  f.c.texCoord = Vector2Xy(1, 1);
  MeshAddFace(gui->mesh, f);

  f.b = f.a;
  f.a = f.c;
  f.c = f.b;
  f.b.position = Vector3Xyz(1, 0, 0);
  f.b.texCoord = Vector2Xy(1, 0);
  MeshAddFace(gui->mesh, f);
}

void _GuiCleanup()
{
  MeshDestroy(gui->mesh);
  pfree(gui);
}

void _GuiSetScreenSize(int width, int height)
{
  gui->size.x = (float)width;
  gui->size.y = (float)height;
}

struct Vector2 GuiScreenSize()
{
  return gui->size;
}

void GuiImage(struct Vector4 rect, struct Image *image)
{
  gnPushMatrix();
  gnTranslatef(rect.x, rect.y, 0);

  if(!rect.z)
  {
    rect.z = (float)ImageWidth(image);
  }

  if(!rect.w)
  {
    rect.w = (float)ImageHeight(image);
  }

  gnScalef(rect.z, rect.w, 0);
  _ImageBind(image);
  _MeshDisplay(gui->mesh);
  glBindTexture(GL_TEXTURE_2D, 0);
  gnPopMatrix();
}

void _GuiApplyProjection()
{
  gnMatrixMode(GL_PROJECTION);
  gnLoadIdentity();
  gnOrtho(0, gui->size.x, gui->size.y, 0, -1.0f, 1.0f);
  gnMatrixMode(GL_MODELVIEW);
}

void GuiText(struct Vector2 point, char *text)
{
  gnPushMatrix();
  gnTranslatef(point.x, point.y, 0);

  _FontDisplay(gui->font, text);

  gnPopMatrix();
}

void GuiSetFont(struct Font *font)
{
  gui->font = font;
}

void _GuiUpdateScreenSize()
{
  gui->size.x = glutGet(GLUT_WINDOW_WIDTH);
  gui->size.y = glutGet(GLUT_WINDOW_HEIGHT);
}
