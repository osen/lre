#include "Cache.h"
#include "Image.h"
#include "Font.h"
#include "Model.h"
#include "Animation.h"

#include <plank/plank.h>

#include <string.h>

struct ImageEntry
{
  struct Image *image;
  sstream *path;
};

struct FontEntry
{
  struct Font *font;
  sstream *path;
};

struct ModelEntry
{
  struct Model *model;
  sstream *path;
};

struct AnimationEntry
{
  struct Animation *animation;
  sstream *path;
};

struct Cache
{
  vector(struct ImageEntry) *images;
  vector(struct FontEntry) *fonts;
  vector(struct ModelEntry) *models;
  vector(struct AnimationEntry) *animations;
};

struct Cache *cache;

void _CacheInitialize()
{
  cache = palloc(struct Cache);
  cache->images = vector_new(struct ImageEntry);
  cache->fonts = vector_new(struct FontEntry);
  cache->models = vector_new(struct ModelEntry);
  cache->animations = vector_new(struct AnimationEntry);
}

void _CachePurge()
{
  while(vector_size(cache->fonts) > 0)
  {
    _FontDestroy(vector_at(cache->fonts, 0).font);
  }

  while(vector_size(cache->models) > 0)
  {
    _ModelDestroy(vector_at(cache->models, 0).model);
  }

  while(vector_size(cache->images) > 0)
  {
    ImageDestroy(vector_at(cache->images, 0).image);
  }

  while(vector_size(cache->animations) > 0)
  {
    _AnimationDestroy(vector_at(cache->animations, 0).animation);
  }
}

void _CacheCleanup()
{
  _CachePurge();
  vector_delete(cache->images);
  vector_delete(cache->fonts);
  vector_delete(cache->models);
  vector_delete(cache->animations);
  pfree(cache);
}

struct Animation *_CacheFetchAnimation(char *path)
{
  size_t i = 0;

  for(i = 0; i < vector_size(cache->animations); i++)
  {
    if(strcmp(path, sstream_cstr(vector_at(cache->animations, i).path)) == 0)
    {
      return vector_at(cache->animations, i).animation;
    }
  }

  return NULL;
}

void _CacheStoreAnimation(struct Animation *animation, char *path)
{
  struct AnimationEntry entry = {0};

  entry.animation = animation;
  entry.path = sstream_new();
  sstream_push_cstr(entry.path, path);

  vector_push_back(cache->animations, entry);
}

void _CachePurgeAnimation(struct Animation *animation)
{
  size_t i = 0;

  for(i = 0; i < vector_size(cache->animations); i++)
  {
    if(animation == vector_at(cache->animations, i).animation)
    {
      sstream_delete(vector_at(cache->animations, i).path);
      vector_erase(cache->animations, i);
      return;
    }
  }
}

struct Image *_CacheFetchImage(char *path)
{
  size_t i = 0;

  for(i = 0; i < vector_size(cache->images); i++)
  {
    if(strcmp(path, sstream_cstr(vector_at(cache->images, i).path)) == 0)
    {
      return vector_at(cache->images, i).image;
    }
  }

  return NULL;
}

void _CacheStoreImage(struct Image *image, char *path)
{
  struct ImageEntry entry = {0};

  entry.image = image;
  entry.path = sstream_new();
  sstream_push_cstr(entry.path, path);

  vector_push_back(cache->images, entry);
}

void _CachePurgeImage(struct Image *image)
{
  size_t i = 0;

  for(i = 0; i < vector_size(cache->images); i++)
  {
    if(image == vector_at(cache->images, i).image)
    {
      sstream_delete(vector_at(cache->images, i).path);
      vector_erase(cache->images, i);
      return;
    }
  }
}

struct Font *_CacheFetchFont(char *path)
{
  size_t i = 0;

  for(i = 0; i < vector_size(cache->fonts); i++)
  {
    if(strcmp(path, sstream_cstr(vector_at(cache->fonts, i).path)) == 0)
    {
      return vector_at(cache->fonts, i).font;
    }
  }

  return NULL;
}

void _CacheStoreFont(struct Font *font, char *path)
{
  struct FontEntry entry = {0};

  entry.font = font;
  entry.path = sstream_new();
  sstream_push_cstr(entry.path, path);

  vector_push_back(cache->fonts, entry);
}

void _CachePurgeFont(struct Font *font)
{
  size_t i = 0;

  for(i = 0; i < vector_size(cache->fonts); i++)
  {
    if(font == vector_at(cache->fonts, i).font)
    {
      sstream_delete(vector_at(cache->fonts, i).path);
      vector_erase(cache->fonts, i);
      return;
    }
  }
}

struct Model *_CacheFetchModel(char *path)
{
  size_t i = 0;

  for(i = 0; i < vector_size(cache->models); i++)
  {
    if(strcmp(path, sstream_cstr(vector_at(cache->models, i).path)) == 0)
    {
      return vector_at(cache->models, i).model;
    }
  }

  return NULL;
}

void _CacheStoreModel(struct Model *model, char *path)
{
  struct ModelEntry entry = {0};

  entry.model = model;
  entry.path = sstream_new();
  sstream_push_cstr(entry.path, path);

  vector_push_back(cache->models, entry);
}

void _CachePurgeModel(struct Model *model)
{
  size_t i = 0;

  for(i = 0; i < vector_size(cache->models); i++)
  {
    if(model == vector_at(cache->models, i).model)
    {
      sstream_delete(vector_at(cache->models, i).path);
      vector_erase(cache->models, i);
      return;
    }
  }
}
