#include <plank/plank.h>

struct Actor;
struct Model;
struct Animation;
struct AnimationAttachment;

struct ModelRenderer
{
  struct Model *model;
  //vector(struct AnimationAttachment *) *attachments;
  struct Animation *animation;
  float frame;
  float fps;
  int unshaded;
};

void ModelRendererSetModel(struct Actor *ctx, struct Model *model);
void ModelRendererAttachAnimation(struct Actor *ctx, struct Animation *animation);
//void ModelRendererDetachAnimation(struct Actor *ctx, struct Animation *animation);
void ModelRendererSetUnshaded(struct Actor *ctx, int unshaded);
void ModelRendererSetFps(struct Actor *ctx, float fps);
