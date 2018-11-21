#include "Font.h"
#include "Cache.h"
#include "error.h"
#include "Image.h"
#include "Color.h"
#include "Mesh.h"

#include <plank/plank.h>

#include <gluten/gluten.h>

#include <string.h>
#include <time.h>

#define TEXT_EXPIRE 5

struct Glyph
{
  char c;
  int x;
  int y;
  int width;
  int height;
  struct Image *image;
};

struct Text
{
  sstream *text;
  struct Image *image;
  struct Mesh *mesh;
  time_t expire;
};

struct Font
{
  struct Image *image;
  vector(struct Glyph) *glyphs;
  vector(struct Text) *texts;
};

static const char *characters = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

int _FontDirtyColumn(struct Font *ctx, int x)
{
  int y = 0;

  for(y = 0; y < ImageHeight(ctx->image); y++)
  {
    struct Color col = {0};

    col = ImagePixel(ctx->image, x, y);

    if(col.a != 0) return 1;
  }

  return 0;
}

void _FontGenerateGlyphs(struct Font *ctx)
{
  int x = 0;
  size_t i = 0;

  for(i = 0; i < strlen(characters); i++)
  {
    struct Glyph g = {0};
    g.c = characters[i];
    g.image = ctx->image;
    g.y = 0; 
    g.height = ImageHeight(ctx->image);

    while(1)
    {
      if(x >= ImageWidth(ctx->image))
      {
        Error("End of image before char found");
      }

      if(_FontDirtyColumn(ctx, x) == 1)
      {
        g.x = x;
        break;
      }

      x++;
    }

    while(1)
    {
      if(x >= ImageWidth(ctx->image))
      {
        Error("End of image before end of char found");
      }

      if(_FontDirtyColumn(ctx, x) == 0)
      { 
        g.width = x - g.x;
        break;
      }

      x++;
    }

    vector_push_back(ctx->glyphs, g);

    if(g.c == '-')
    { 
      g.c = ' '; 
      g.height = 1;
      vector_push_back(ctx->glyphs, g);
    }
  }
}

struct Font *FontLoad(char *path)
{
  struct Font *rtn = NULL;

  rtn = _CacheFetchFont(path);
  if(rtn) return rtn;

  rtn = palloc(struct Font);
  rtn->image = ImageLoad(path);
  rtn->glyphs = vector_new(struct Glyph);
  rtn->texts = vector_new(struct Text);

  _FontGenerateGlyphs(rtn);

  _CacheStoreFont(rtn, path);

  return rtn;
}

void _FontDestroy(struct Font *ctx)
{
  size_t i = 0;

  _CachePurgeFont(ctx);

  for(i = 0; i < vector_size(ctx->texts); i++)
  {
    sstream_delete(vector_at(ctx->texts, i).text);
    MeshDestroy(vector_at(ctx->texts, i).mesh);
  }

  vector_delete(ctx->texts);
  vector_delete(ctx->glyphs);
  /* ImageDestroy(ctx->image); */
  pfree(ctx);
}

struct Glyph _FontGlyph(struct Font *ctx, char c)
{
  size_t i = 0;
  struct Glyph rtn = {0};

  for(i = 0; i < vector_size(ctx->glyphs); i++)
  {
    if(vector_at(ctx->glyphs, i).c == c)
    {
      return vector_at(ctx->glyphs, i);
    }
  }

  Error("Requested glyph not found");
  return rtn;
}

struct Text _FontText(struct Font *ctx, char *text)
{
  size_t i = 0;
  struct Text rtn = {0};
  struct Vector2 t = {0};
  time_t now = 0;
  float x = 0;

  now = time(NULL);

  for(i = 0; i < vector_size(ctx->texts); i++)
  {
    if(vector_at(ctx->texts, i).expire <= now)
    {
      struct Text t = vector_at(ctx->texts, i);
      sstream_delete(t.text);
      MeshDestroy(t.mesh);
      vector_erase(ctx->texts, i);
      i--;
    }
  }

  for(i = 0; i < vector_size(ctx->texts); i++)
  {
    if(strcmp(sstream_cstr(vector_at(ctx->texts, i).text), text) == 0)
    {
      struct Text t = vector_at(ctx->texts, i);

      t.expire = now + TEXT_EXPIRE;
      vector_set(ctx->texts, i, t);

      return vector_at(ctx->texts, i);
    }
  }

  rtn.expire = now + TEXT_EXPIRE;
  rtn.image = ctx->image;
  rtn.text = sstream_new();
  sstream_push_cstr(rtn.text, text);
  rtn.mesh = MeshCreate();
  t = Vector2Xy(1.0f / ImageWidth(ctx->image), 1.0f / ImageHeight(ctx->image));

  for(i = 0; i < sstream_length(rtn.text); i++)
  {
    struct Glyph g = {0};
    struct Face f = {0};

    g = _FontGlyph(ctx, sstream_at(rtn.text, i));

    f.a.position = Vector3Xyz(x, 0, 0);
    f.a.texCoord = Vector2Xy((float)g.x * t.x, 1.0f - ((float)g.y + (float)g.height) * t.y);
    f.b.position = Vector3Xyz(x, (float)g.height, 0);
    f.b.texCoord = Vector2Xy((float)g.x * t.x, 1.0f - (float)g.y * t.y);
    f.c.position = Vector3Xyz(x + (float)g.width, (float)g.height, 0);
    f.c.texCoord = Vector2Xy(((float)g.x + (float)g.width) * t.x, 1.0f - (float)g.y * t.y);
    MeshAddFace(rtn.mesh, f);

    f.b = f.a;
    f.a = f.c;
    f.c = f.b;
    f.b.position = Vector3Xyz(x + (float)g.width, 0, 0);
    f.b.texCoord = Vector2Xy(((float)g.x + (float)g.width) * t.x, 1.0f - ((float)g.y + (float)g.height) * t.y);
    MeshAddFace(rtn.mesh, f);

    x += (float)g.width + 1.0f;
  }

  vector_push_back(ctx->texts, rtn);

  return rtn;
}

int FontHeight(struct Font *ctx)
{
  struct Glyph g = _FontGlyph(ctx, 'A');

  return g.height;
}

int FontWidth(struct Font *ctx, char *sample)
{
  int rtn = 0;
  size_t i = 0;
  int space = 0;

  for(i = 0; i < strlen(sample); i++)
  {
    struct Glyph g = _FontGlyph(ctx, sample[i]);

    rtn += space + g.width;
    space = 1;
  }

  return rtn;
}

void _FontDisplay(struct Font *ctx, char *text)
{
  struct Text t = {0};

  t = _FontText(ctx, text);
  _ImageBind(t.image);
  _MeshDisplay(t.mesh);
  glBindTexture(GL_TEXTURE_2D, 0);
}
