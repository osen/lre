#include "ModelRenderer.h"
#include "Actor.h"
#include "error.h"
#include "Model.h"
#include "Transform.h"
#include "Debug.h"
#include "Environment.h"
#include "Animation.h"

#include <gluten/gluten.h>

struct AnimationAttachment
{
  struct Animation *animation;
  float frame;
  int loop;
};

void _ModelRendererTick(struct Actor *ctx)
{
  struct ModelRenderer *mr = NULL;

  mr = ActorComponent(ctx, ModelRenderer);

  if(!mr->animation) return;

  mr->frame += mr->fps * TimeDelta();

  if(mr->frame >= AnimationFrameCount(mr->animation))
  {
    mr->frame = 0;
  }
}

void _ModelRendererDisplay(struct Actor *ctx)
{
  struct ModelRenderer *mr = NULL;
  struct Vector3 rotation = {0};
  struct Vector3 position = {0};
  vector(struct Part *) *parts = NULL;
  size_t pi = 0;

  mr = ActorComponent(ctx, ModelRenderer);
  rotation = TransformRotation(ctx);
  position = TransformPosition(ctx);

  //GLfloat pos[4] = {-1, 1, 1, 0.0f};
  //gnLightfv(GL_LIGHT0, GL_POSITION, pos);
  //GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
  //gnLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

  gnPushMatrix();
  gnTranslatef(position.x, position.y, position.z);
  gnRotatef(rotation.x, 1, 0, 0);
  gnRotatef(rotation.y, 0, 1, 0);
  gnRotatef(rotation.z, 0, 0, 1);


  if(!mr->unshaded)
  {
    gnEnable(GN_LIGHTING);
    gnEnable(GL_LIGHT0);
    gnEnableClientState(GL_NORMAL_ARRAY);
  }

  parts = ModelParts(mr->model);

  for(pi = 0; pi < vector_size(parts); pi++)
  {
    struct Vector3 translation = {0};
    struct Vector3 rotation = {0};
    struct Part *p = vector_at(parts, pi);

    if(mr->animation)
    {
      struct Vector3 size = {0};
      struct Transformation t = AnimationTransformation(mr->animation, PartName(p), mr->frame);

      size = PartSize(p);
      translation = t.translation;
      translation.x /= 100.0f;
      translation.x *= size.x;
      translation.y /= 100.0f;
      translation.y *= size.y;
      translation.z /= 100.0f;
      translation.z *= size.z;
      rotation = t.rotation;
    }

    _PartDisplay(p, translation, rotation);
  }

  //_ModelDisplay(mr->model);

  if(!mr->unshaded)
  {
    gnDisableClientState(GL_NORMAL_ARRAY);
    gnDisable(GL_LIGHT0);
    gnDisable(GN_LIGHTING);
  }

  gnPopMatrix();
}

void ModelRendererInit(struct Actor *ctx)
{
  struct ModelRenderer *mr = NULL;

  mr = ActorComponent(ctx, ModelRenderer);
  mr->fps = 1;

  ActorAddEvent(ctx, "tick", _ModelRendererTick);
  ActorAddEvent(ctx, "display", _ModelRendererDisplay);
}

void ModelRendererSetModel(struct Actor *ctx, struct Model *model)
{
  struct ModelRenderer *mr = NULL;

  mr = ActorComponent(ctx, ModelRenderer);
  mr->model = model;
}

void ModelRendererSetUnshaded(struct Actor *ctx, int unshaded)
{
  struct ModelRenderer *mr = NULL;

  mr = ActorComponent(ctx, ModelRenderer);
  mr->unshaded = unshaded;
}

void ModelRendererAttachAnimation(struct Actor *ctx, struct Animation *animation)
{
  struct ModelRenderer *mr = NULL;

  mr = ActorComponent(ctx, ModelRenderer);

  if(mr->animation == animation)
  {
    return;
  }

  mr->animation = animation;
  mr->frame = 0;
}

void ModelRendererSetFps(struct Actor *ctx, float fps)
{
  struct ModelRenderer *mr = NULL;

  mr = ActorComponent(ctx, ModelRenderer);
  mr->fps = fps;
}
