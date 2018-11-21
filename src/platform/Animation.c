#include "Animation.h"
#include "Cache.h"
#include "error.h"
#include "util.h"
#include "h.out.h"

#include <string.h>

struct Frame
{
  vector(struct Transformation) *transformations;
};

struct Animation
{
  vector(struct Frame *) *frames;
};

void _AnimationProcessTLine(vector(sstream *) *splitLine, struct Frame *frame)
{
  struct Transformation t = {0};

  t.partName = sstream_new();
  sstream_push_cstr(t.partName, sstream_cstr(vector_at(splitLine, 1)));
  t.translation.x = atof(sstream_cstr(vector_at(splitLine, 2)));
  t.translation.y = atof(sstream_cstr(vector_at(splitLine, 3)));
  t.translation.z = atof(sstream_cstr(vector_at(splitLine, 4)));
  t.rotation.x = atof(sstream_cstr(vector_at(splitLine, 5)));
  t.rotation.y = atof(sstream_cstr(vector_at(splitLine, 6)));
  t.rotation.z = atof(sstream_cstr(vector_at(splitLine, 7)));
  vector_push_back(frame->transformations, t);
}

struct Animation *AnimationLoad(char *path)
{
  struct Animation *rtn = NULL;
  sstream *fullPath = NULL;
  praw(struct HdFile *) file = NULL;
  sstream *line = NULL;
  vector(sstream *) *splitLine = NULL;
  struct Frame *frame = NULL;

  rtn = _CacheFetchAnimation(path);
  if(rtn) return rtn;

  fullPath = sstream_new();
  sstream_push_cstr(fullPath, "assets/");
  sstream_push_cstr(fullPath, path);
  sstream_push_cstr(fullPath, ".anm");

  line = sstream_new();
  file = palloc(struct HdFile *);
  *file = HdFileOpen(sstream_cstr(fullPath));
  sstream_delete(fullPath);
  splitLine = vector_new(sstream *);

  if(!*file)
  {
    Error("Failed to open model for loading");
  }

  rtn = palloc(struct Animation);
  rtn->frames = vector_new(struct Frame *);

  while(fgetline(*file, line))
  {
    if(sstream_length(line) < 1) continue;
    sstream_splitwhitespace(line, splitLine);

    if(strcmp(sstream_cstr(vector_at(splitLine, 0)), "f") == 0)
    {
      frame = palloc(struct Frame);
      frame->transformations = vector_new(struct Transformation);
      vector_push_back(rtn->frames, frame);
    }
    else if(strcmp(sstream_cstr(vector_at(splitLine, 0)), "t") == 0)
    {
      _AnimationProcessTLine(splitLine, frame);
    }
  }

  vector_sstream_clear(splitLine);
  vector_delete(splitLine);
  sstream_delete(line);
  HdFileClose(*file); pfree(file);

  _CacheStoreAnimation(rtn, path);

  return rtn;
}

void _AnimationDestroy(struct Animation *ctx)
{
  size_t fi = 0;
  size_t ti = 0;

  _CachePurgeAnimation(ctx);

  for(fi = 0; fi < vector_size(ctx->frames); fi++)
  {
    struct Frame *f = vector_at(ctx->frames, fi);

    for(ti = 0; ti < vector_size(f->transformations); ti++)
    {
      sstream_delete(vector_at(f->transformations, ti).partName);
    }

    vector_delete(f->transformations);
    pfree(f);
  }

  vector_delete(ctx->frames);
  pfree(ctx);
}

struct Transformation AnimationTransformation(struct Animation *ctx, char *partName, float frame)
{
  struct Transformation rtn = {0};
  struct Frame *f1 = NULL;
  struct Frame *f2 = NULL;
  struct Transformation t1 = {0};
  struct Transformation t2 = {0};
  size_t ti = 0;
  struct Vector3 diff = {0};
  int iFrame = frame;
  float st = frame - iFrame;

  f1 = vector_at(ctx->frames, (size_t)frame);

  if(frame + 1 >= AnimationFrameCount(ctx))
  {
    f2 = vector_at(ctx->frames, 0);
  }
  else
  {
    f2 = vector_at(ctx->frames, (size_t)frame + 1);
  }

  for(ti = 0; ti < vector_size(f1->transformations); ti++)
  {
    if(strcmp(partName, sstream_cstr(vector_at(f1->transformations, ti).partName)) == 0)
    {
      t1 = vector_at(f1->transformations, ti);
    }
  }

  for(ti = 0; ti < vector_size(f2->transformations); ti++)
  {
    if(strcmp(partName, sstream_cstr(vector_at(f2->transformations, ti).partName)) == 0)
    {
      t2 = vector_at(f2->transformations, ti);
    }
  }

  rtn = t1;
  diff = Vector3Subtract(t2.translation, t1.translation);
  rtn.translation.x += diff.x * st;
  rtn.translation.y += diff.y * st;
  rtn.translation.z += diff.z * st;
  diff = Vector3Subtract(t2.rotation, t1.rotation);
  rtn.rotation.x += diff.x * st;
  rtn.rotation.y += diff.y * st;
  rtn.rotation.z += diff.z * st;

  return rtn;
}

int AnimationFrameCount(struct Animation *ctx)
{
  return vector_size(ctx->frames);
}
