#include "Image.h"
#include "Cache.h"
#include "error.h"
#include "Color.h"
#include "util.h"
#define STB_IMAGE_IMPLEMENTATION
#include "contrib/stb_image.h"

#include <plank/plank.h>

#include <gluten/gluten.h>

struct Image
{
  int width;
  int height;
  vector(unsigned char) *data;
  praw(GLuint) texture;
  int dirty;
};

struct Image *ImageCreate(int width, int height)
{
  struct Image *rtn = NULL;

  rtn = palloc(struct Image);
  rtn->width = width;
  rtn->height = height;
  rtn->data = vector_new(unsigned char);

  while(vector_size(rtn->data) < (unsigned)(width * height * 4))
  {
    vector_push_back(rtn->data, 0);
  }

  rtn->dirty = 1;

  return rtn;
}

struct Image *ImageLoad(char *path)
{
  struct Image *rtn = NULL;
  int width = 0;
  int height = 0;
  int n = 0;
  praw(unsigned char *) image = NULL;
  unsigned i = 0;
  sstream *fullPath = NULL;
  praw(unsigned char *) fileBuffer = NULL;
  size_t fileSize = 0;

  rtn = _CacheFetchImage(path);
  if(rtn) return rtn;

  fullPath = sstream_new();
  sstream_push_cstr(fullPath, "assets/");
  sstream_push_cstr(fullPath, path);
  sstream_push_cstr(fullPath, ".png");
  fileBuffer = palloc(unsigned char *);

  *fileBuffer = fgetall(fullPath, &fileSize);

  if(!*fileBuffer)
  {
    sstream_clear(fullPath);
    sstream_push_cstr(fullPath, "assets/");
    sstream_push_cstr(fullPath, path);
    sstream_push_cstr(fullPath, ".jpg");
    *fileBuffer = fgetall(fullPath, &fileSize);

    if(!*fileBuffer)
    {
      Error("Failed to read image");
    }
  }

  image = palloc(unsigned char *);
  //*image = stbi_load(sstream_cstr(fullPath), &width, &height, &n, 4);
  *image = stbi_load_from_memory(*fileBuffer, (int)fileSize, &width, &height, &n, 4);
  free(*fileBuffer); pfree(fileBuffer);

  if(!*image)
  {
    Error("Failed to process image");
  }

  sstream_delete(fullPath);
  rtn = palloc(struct Image);
  rtn->width = width;
  rtn->height = height;
  rtn->data = vector_new(unsigned char);

  for(i = 0; i < width * height * 4; i++)
  {
    vector_push_back(rtn->data, (*image)[i]);
  }

  rtn->dirty = 1;

  free(*image); pfree(image);
  _CacheStoreImage(rtn, path);

  return rtn;
}

/*
struct Image *ImageCreate(int width, int height)
{
  struct Image *rtn = NULL;

  return rtn;
}
*/

void ImageDestroy(struct Image *ctx)
{
  _CachePurgeImage(ctx);

  if(ctx->texture)
  {
    glDeleteTextures(1, ctx->texture); pfree(ctx->texture);
  }

  vector_delete(ctx->data);
  pfree(ctx);
}

int ImageWidth(struct Image *ctx)
{
  return ctx->width;
}

int ImageHeight(struct Image *ctx)
{
  return ctx->height;
}

struct Color ImagePixel(struct Image *ctx, int x, int y)
{
  struct Color rtn = {0};
  size_t idx = 0;

  if(x >= ctx->width || y >= ctx->height || x < 0 || y < 0)
  {
    Error("Image pixel out of bounds");
  }

  idx = (y * ctx->width * 4) + x * 4;

  rtn.r = vector_at(ctx->data, idx);
  rtn.g = vector_at(ctx->data, idx + 1);
  rtn.b = vector_at(ctx->data, idx + 2);
  rtn.a = vector_at(ctx->data, idx + 3);

  return rtn;
}

void ImageSetPixel(struct Image *ctx, int x, int y, struct Color color)
{
  size_t idx = 0;

  if(x >= ctx->width || y >= ctx->height || x < 0 || y < 0)
  {
    Error("Image pixel out of bounds");
  }

  idx = (y * ctx->width * 4) + x * 4;
  vector_set(ctx->data, idx, color.r);
  vector_set(ctx->data, idx + 1, color.g);
  vector_set(ctx->data, idx + 2, color.b);
  vector_set(ctx->data, idx + 3, color.a);
}

void _ImageBind(struct Image *ctx)
{
  if(ctx->dirty)
  {
    if(!ctx->texture)
    {
      ctx->texture = palloc(GLuint);
      glGenTextures(1, ctx->texture);
    }

    glBindTexture(GL_TEXTURE_2D, *ctx->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ctx->width, ctx->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, vector_data(ctx->data));

    if(glGenerateMipmap)
    {
      glGenerateMipmap(GL_TEXTURE_2D);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }
    else
    {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    ctx->dirty = 0;
  }

  glBindTexture(GL_TEXTURE_2D, *ctx->texture);
}
