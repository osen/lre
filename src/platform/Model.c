#include "Model.h"
#include "Cache.h"
#include "util.h"
#include "error.h"
#include "math.h"
#include "Mesh.h"
#include "Image.h"
#include "h.out.h"

#include <plank/plank.h>

#include <gluten/gluten.h>

#include <string.h>

struct Material
{
  sstream *name;
  struct Image *texture;
};

struct MaterialGroup
{
  struct Mesh *mesh;
  struct Material *material;
};

struct Part
{
  sstream *name;
  vector(struct MaterialGroup *) *materialGroups;
  struct Vector3 center;
  struct Vector3 min;
  struct Vector3 max;
};

struct Model
{
  vector(struct Part *) *parts;
  vector(struct Material *) *materials;
  vector(struct Triangle) *triangles;
  struct Vector3 center;
  struct Vector3 min;
  struct Vector3 max;
};

struct ModelData
{
  vector(sstream *) *vertexSplit;
  sstream *fullPath;
  vector(Vector3) *positions;
  vector(Vector3) *normals;
  vector(Vector2) *texCoords;
  struct Model *model;
  struct Part *currentPart;
  struct MaterialGroup *currentMg;
  struct Material *currentMaterial;
  vector(struct Triangle) *triangles;
};

void _ModelProcessVLine(vector(sstream *) *splitLine, struct ModelData *data)
{
  vector_push_back(data->positions, Vector3Xyz(
    (float)atof(sstream_cstr(vector_at(splitLine, 1))),
    (float)atof(sstream_cstr(vector_at(splitLine, 2))),
    (float)atof(sstream_cstr(vector_at(splitLine, 3)))));
}

void _ModelProcessVnLine(vector(sstream *) *splitLine, struct ModelData *data)
{
  vector_push_back(data->normals, Vector3Xyz(
    (float)atof(sstream_cstr(vector_at(splitLine, 1))),
    (float)atof(sstream_cstr(vector_at(splitLine, 2))),
    (float)atof(sstream_cstr(vector_at(splitLine, 3)))));
}

void _ModelProcessVtLine(vector(sstream *) *splitLine, struct ModelData *data)
{
  vector_push_back(data->texCoords, Vector2Xy(
    (float)atof(sstream_cstr(vector_at(splitLine, 1))),
    (float)atof(sstream_cstr(vector_at(splitLine, 2)))));
}

struct Part *_PartCreate(char *name)
{
  struct Part *rtn = NULL;

  rtn = palloc(struct Part);
  rtn->name = sstream_new();
  sstream_push_cstr(rtn->name, name);
  rtn->materialGroups = vector_new(struct MaterialGroup *);

  return rtn;
}

struct Vertex _ModelVertexFromSection(sstream *section, struct ModelData *data)
{
  struct Vertex rtn = {0};
  size_t pi = 0;
  size_t ti = 0;

  sstream_split(section, '/', data->vertexSplit);

  pi = atoi(sstream_cstr(vector_at(data->vertexSplit, 0)));
  ti = atoi(sstream_cstr(vector_at(data->vertexSplit, 1)));

  rtn.position = vector_at(data->positions, pi - 1);
  rtn.texCoord = vector_at(data->texCoords, ti - 1);

  rtn.texCoord.y = 1.0f - rtn.texCoord.y;

  if(vector_size(data->vertexSplit) > 2)
  {
    size_t ni = 0;

    ni = atoi(sstream_cstr(vector_at(data->vertexSplit, 2)));
    rtn.normal = vector_at(data->normals, ni - 1);
  }

  return rtn;
}

void _ModelProcessFLine(vector(sstream *) *splitLine, struct ModelData *data)
{
  struct Face f = {0};
  struct Triangle t = {0};

  if(!data->currentPart)
  {
    struct Part *part = NULL;

    part = _PartCreate("Default");
    data->currentPart = part;
    //data->currentMg = NULL; // Not necessary
    vector_push_back(data->model->parts, data->currentPart);
  }

  if(!data->currentMg)
  {
    struct MaterialGroup *mg = NULL;

    mg = palloc(struct MaterialGroup);
    mg->mesh = MeshCreate();

    if(!data->currentMaterial)
    {
      Error("No material specified by model");
    }

    if(!data->currentMaterial->texture)
    {
      Error("Material does not specify a texture");
    }

    mg->material = data->currentMaterial;
    vector_push_back(data->currentPart->materialGroups, mg);
    data->currentMg = mg;
  }

  // Push into data->currentMg->mesh, the face
  // Remember that there may be two triangles a line (4 verts).

  f.a = _ModelVertexFromSection(vector_at(splitLine, 1), data);
  f.b = _ModelVertexFromSection(vector_at(splitLine, 2), data);
  f.c = _ModelVertexFromSection(vector_at(splitLine, 3), data);
  MeshAddFace(data->currentMg->mesh, f);
  t.a = f.a.position;
  t.b = f.b.position;
  t.c = f.c.position;
  vector_push_back(data->triangles, t);

  if(vector_size(splitLine) > 4)
  {
    struct Face f2 = {0};

    f2.a = f.c;
    f2.b = _ModelVertexFromSection(vector_at(splitLine, 4), data);
    f2.c = f.a;
    MeshAddFace(data->currentMg->mesh, f2);
    t.a = f2.a.position;
    t.b = f2.b.position;
    t.c = f2.c.position;
    vector_push_back(data->triangles, t);
  }
}

void _ModelProcessOLine(vector(sstream *) *splitLine, struct ModelData *data)
{
  struct Part *part = NULL;

  part = _PartCreate(sstream_cstr(vector_at(splitLine, 1)));
  data->currentPart = part;
  data->currentMg = NULL;
  vector_push_back(data->model->parts, data->currentPart);
}

void _ModelProcessUsemtlLine(vector(sstream *) *splitLine, struct ModelData *data)
{
  sstream *name = NULL;
  size_t i = 0;

  data->currentMg = NULL;
  name = vector_at(splitLine, 1);

  for(i = 0; i < vector_size(data->model->materials); i++)
  {
    struct Material *material = vector_at(data->model->materials, i);

    if(strcmp(sstream_cstr(name), sstream_cstr(material->name)) == 0)
    {
      data->currentMaterial = material;
      return;
    }
  }

  Error("Failed to find specified material name");
}

void _ModelParseMtlFile(char *path, struct ModelData *data)
{
  praw(struct HdFile *) file = NULL;
  sstream *line = NULL;
  vector(sstream *) *splitLine = NULL;
  struct Material *currentMaterial = NULL;
  sstream *mtlPath = NULL;

  mtlPath = sstream_new();
  sstream_push_cstr(mtlPath, "assets/");
  sstream_push_cstr(mtlPath, path);

  line = sstream_new();
  file = palloc(struct HdFile *);
  *file = HdFileOpen(sstream_cstr(mtlPath));
  sstream_delete(mtlPath);
  splitLine = vector_new(sstream *);

  if(!*file)
  {
    Error("Failed to open material file for loading");
  }

  while(fgetline(*file, line))
  {
    if(sstream_length(line) < 1) continue;
    sstream_splitwhitespace(line, splitLine);

    if(strcmp(sstream_cstr(vector_at(splitLine, 0)), "newmtl") == 0)
    {
      currentMaterial = palloc(struct Material);
      currentMaterial->name = sstream_new();
      sstream_push_cstr(currentMaterial->name, sstream_cstr(vector_at(splitLine, 1)));
      vector_push_back(data->model->materials, currentMaterial);
    }
    else if(strcmp(sstream_cstr(vector_at(splitLine, 0)), "map_Kd") == 0)
    {
      sstream *texPath = NULL;

      texPath = sstream_new();
      sstream_push_cstr(texPath, path);
      sstream_cropbeforelast(texPath, '/');
      sstream_cropbeforelast(texPath, '\\');
      sstream_push_char(texPath, '/');
      sstream_push_cstr(texPath, sstream_cstr(vector_at(splitLine, 1)));
      sstream_cropbeforelast(texPath, '.');
      currentMaterial->texture = ImageLoad(sstream_cstr(texPath));
      sstream_delete(texPath);
    }
  }

  vector_sstream_clear(splitLine);
  vector_delete(splitLine);
  sstream_delete(line);
  HdFileClose(*file); pfree(file);
}

void _ModelProcessMtllibLine(vector(sstream *) *splitLine, struct ModelData *data)
{
  sstream *path = NULL;

  path = sstream_new();
  sstream_push_cstr(path, sstream_cstr(data->fullPath));
  sstream_cropbeforelast(path, '/');
  sstream_cropbeforelast(path, '\\');
  sstream_push_char(path, '/');
  sstream_push_cstr(path, sstream_cstr(vector_at(splitLine, 1)));

  _ModelParseMtlFile(sstream_cstr(path), data);

  sstream_delete(path);
}

void _ModelParseFile(struct ModelData *data)
{
  praw(struct HdFile *) file = NULL;
  sstream *line = NULL;
  vector(sstream *) *splitLine = NULL;
  sstream *path = NULL;

  path = sstream_new();
  sstream_push_cstr(path, "assets/");
  sstream_push_cstr(path, sstream_cstr(data->fullPath));

  line = sstream_new();
  file = palloc(struct HdFile *);
  *file = HdFileOpen(sstream_cstr(path));
  splitLine = vector_new(sstream *);

  if(!*file)
  {
    printf("Model: %s\n", sstream_cstr(path));
    Error("Failed to open model for loading");
  }

  sstream_delete(path);

  while(fgetline(*file, line))
  {
    if(sstream_length(line) < 1) continue;
    sstream_splitwhitespace(line, splitLine);

    if(strcmp(sstream_cstr(vector_at(splitLine, 0)), "v") == 0)
    {
      _ModelProcessVLine(splitLine, data);
    }
    else if(strcmp(sstream_cstr(vector_at(splitLine, 0)), "vn") == 0)
    {
      _ModelProcessVnLine(splitLine, data);
    }
    else if(strcmp(sstream_cstr(vector_at(splitLine, 0)), "vt") == 0)
    {
      _ModelProcessVtLine(splitLine, data);
    }
    else if(strcmp(sstream_cstr(vector_at(splitLine, 0)), "f") == 0)
    {
      _ModelProcessFLine(splitLine, data);
    }
    else if(strcmp(sstream_cstr(vector_at(splitLine, 0)), "o") == 0 ||
      strcmp(sstream_cstr(vector_at(splitLine, 0)), "g") == 0)
    {
      _ModelProcessOLine(splitLine, data);
    }
    else if(strcmp(sstream_cstr(vector_at(splitLine, 0)), "usemtl") == 0)
    {
      _ModelProcessUsemtlLine(splitLine, data);
    }
    else if(strcmp(sstream_cstr(vector_at(splitLine, 0)), "mtllib") == 0)
    {
      _ModelProcessMtllibLine(splitLine, data);
    }
  }

  vector_sstream_clear(splitLine);
  vector_delete(splitLine);
  sstream_delete(line);
  HdFileClose(*file); pfree(file);
}

void _ModelCenterMesh(struct Model *ctx)
{
  size_t pi = 0;

  for(pi = 0; pi < vector_size(ctx->parts); pi++)
  {
    size_t mgi = 0;
    struct Vector3 c = {0};
    struct Part *part = vector_at(ctx->parts, pi);

    c = part->center;

    for(mgi = 0; mgi < vector_size(part->materialGroups); mgi++)
    {
      struct MaterialGroup *mg = vector_at(part->materialGroups, mgi);

      _MeshShift(mg->mesh, Vector3Xyz(-c.x, -c.y, -c.z));
    }
  }
}

void _ModelProcessBounds(struct Model *ctx)
{
  size_t pi = 0;

  for(pi = 0; pi < vector_size(ctx->parts); pi++)
  {
    size_t mgi = 0;
    struct Part *part = vector_at(ctx->parts, pi);

    for(mgi = 0; mgi < vector_size(part->materialGroups); mgi++)
    {
      struct Vector3 mmax = {0};
      struct Vector3 mmin = {0};
      struct Mesh *m = vector_at(part->materialGroups, mgi)->mesh;

      mmax = MeshMax(m);
      mmin = MeshMin(m);

      if(mgi < 1)
      {
        part->max = mmax;
        part->min = mmin;
      }

      if(mmax.x > part->max.x) part->max.x = mmax.x;
      if(mmax.y > part->max.y) part->max.y = mmax.y;
      if(mmax.z > part->max.z) part->max.z = mmax.z;
      if(mmin.x < part->min.x) part->min.x = mmin.x;
      if(mmin.y < part->min.y) part->min.y = mmin.y;
      if(mmin.z < part->min.z) part->min.z = mmin.z;
    }

    part->center.x = (part->min.x + part->max.x) / 2.0f;
    part->center.y = (part->min.y + part->max.y) / 2.0f;
    part->center.z = (part->min.z + part->max.z) / 2.0f;

    //printf("Part: %f %f %f\n", part->center.x, part->center.y, part->center.z);

    if(pi < 1)
    {
      ctx->max = part->max;
      ctx->min = part->min;
    }

    if(part->max.x > ctx->max.x) ctx->max.x = part->max.x;
    if(part->max.y > ctx->max.y) ctx->max.y = part->max.y;
    if(part->max.z > ctx->max.z) ctx->max.z = part->max.z;
    if(part->min.x < ctx->min.x) ctx->min.x = part->min.x;
    if(part->min.y < ctx->min.y) ctx->min.y = part->min.y;
    if(part->min.z < ctx->min.z) ctx->min.z = part->min.z;
  }

  ctx->center.x = (ctx->min.x + ctx->max.x) / 2.0f;
  ctx->center.y = (ctx->min.y + ctx->max.y) / 2.0f;
  ctx->center.z = (ctx->min.z + ctx->max.z) / 2.0f;

  //printf("Model: %f %f %f\n", ctx->center.x, ctx->center.y, ctx->center.z);
}

struct Model *ModelLoad(char *path)
{
  struct Model *rtn = NULL;
  struct ModelData *data = NULL;

  rtn = _CacheFetchModel(path);
  if(rtn) return rtn;

  rtn = palloc(struct Model);
  rtn->parts = vector_new(struct Part *);
  rtn->materials = vector_new(struct Material *);

  data = palloc(struct ModelData);
  data->triangles = vector_new(struct Triangle);
  data->fullPath = sstream_new();
  sstream_push_cstr(data->fullPath, path);
  sstream_push_cstr(data->fullPath, ".obj");
  data->positions = vector_new(Vector3);
  data->normals = vector_new(Vector3);
  data->texCoords = vector_new(Vector2);
  data->model = rtn;
  data->vertexSplit = vector_new(sstream *);

  _ModelParseFile(data);

  rtn->triangles = data->triangles;
  vector_sstream_clear(data->vertexSplit);
  vector_delete(data->vertexSplit);
  vector_delete(data->positions);
  vector_delete(data->normals);
  vector_delete(data->texCoords);
  sstream_delete(data->fullPath);
  pfree(data);

  _ModelProcessBounds(rtn);
  _ModelCenterMesh(rtn);

  _CacheStoreModel(rtn, path);

  return rtn;
}

void _ModelDestroy(struct Model *ctx)
{
  size_t i = 0;

  _CachePurgeModel(ctx);

  for(i = 0; i < vector_size(ctx->parts); i++)
  {
    size_t j = 0;
    struct Part *part = vector_at(ctx->parts, i);

    for(j = 0; j < vector_size(part->materialGroups); j++)
    {
      struct MaterialGroup *mg = vector_at(part->materialGroups, j);

      MeshDestroy(mg->mesh);
      pfree(mg);
    }

    vector_delete(part->materialGroups);
    sstream_delete(part->name);
    pfree(part);
  }

  vector_delete(ctx->parts);

  for(i = 0; i < vector_size(ctx->materials); i++)
  {
    struct Material *material = vector_at(ctx->materials, i);

    sstream_delete(material->name);
    /*ImageDestroy(material->texture);*/
    pfree(material);
  }

  vector_delete(ctx->materials);
  vector_delete(ctx->triangles);

  pfree(ctx);
}

/*
void _ModelDisplay(struct Model *ctx)
{
  size_t pi = 0;

  for(pi = 0; pi < vector_size(ctx->parts); pi++)
  {
    size_t mgi = 0;
    struct Part *part = vector_at(ctx->parts, pi);

    for(mgi = 0; mgi < vector_size(part->materialGroups); mgi++)
    {
      struct MaterialGroup *mg = vector_at(part->materialGroups, mgi);

      _ImageBind(mg->material->texture);
      gnPushMatrix();
      gnTranslatef(part->center.x, part->center.y, part->center.z);
      _MeshDisplay(mg->mesh);
      gnPopMatrix();
      glBindTexture(GL_TEXTURE_2D, 0);
    }
  }
}
*/

void _PartDisplay(struct Part *ctx, struct Vector3 translation, struct Vector3 rotation)
{
  size_t mgi = 0;

  for(mgi = 0; mgi < vector_size(ctx->materialGroups); mgi++)
  {
    struct MaterialGroup *mg = vector_at(ctx->materialGroups, mgi);

    _ImageBind(mg->material->texture);
    gnPushMatrix();
    gnTranslatef(ctx->center.x, ctx->center.y, ctx->center.z);
    gnTranslatef(translation.x, translation.y, translation.z);
    gnRotatef(rotation.x, 1, 0, 0);
    gnRotatef(rotation.y, 0, 1, 0);
    gnRotatef(rotation.z, 0, 0, 1);
    _MeshDisplay(mg->mesh);
    gnPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
  }
}

struct Vector3 ModelSize(struct Model *ctx)
{
  struct Vector3 rtn = {0};

  rtn.x = ctx->max.x - ctx->min.x;
  rtn.y = ctx->max.y - ctx->min.y;
  rtn.z = ctx->max.z - ctx->min.z;

  return rtn;
}

struct Vector3 ModelCenter(struct Model *ctx)
{
  return ctx->center;
}

vector(struct Triangle) *ModelTriangles(struct Model *ctx)
{
  return ctx->triangles;
}

struct Vector3 ModelMin(struct Model *ctx)
{
  return ctx->min;
}

vector(struct Part *) *ModelParts(struct Model *ctx)
{
  return ctx->parts;
}

char *PartName(struct Part *ctx)
{
  return sstream_cstr(ctx->name);
}

struct Vector3 PartSize(struct Part *ctx)
{
  struct Vector3 rtn = {0};

  rtn.x = ctx->max.x - ctx->min.x;
  rtn.y = ctx->max.y - ctx->min.y;
  rtn.z = ctx->max.z - ctx->min.z;

  return rtn;
}
