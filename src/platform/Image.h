typedef struct Image Image;

struct Color;

struct Image *ImageLoad(char *path);
struct Image *ImageCreate(int width, int height);

void ImageDestroy(struct Image *ctx);
int ImageWidth(struct Image *ctx);
int ImageHeight(struct Image *ctx);

void ImageSetPixel(struct Image *ctx, int x, int y, struct Color color);
struct Color ImagePixel(struct Image *ctx, int x, int y);
void _ImageBind(struct Image *ctx);
