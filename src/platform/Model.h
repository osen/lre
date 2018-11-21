#include <plank/plank.h>

struct Vector3;

typedef struct Model Model;
typedef struct Part Part;

struct Model *ModelLoad(char *path);
void _ModelDestroy(struct Model *ctx);
//void _ModelDisplay(struct Model *ctx);
void _PartDisplay(struct Part *ctx, struct Vector3 translation, struct Vector3 rotation);

struct Vector3 ModelSize(struct Model *ctx);
struct Vector3 ModelCenter(struct Model *ctx);
vector(struct Triangle) *ModelTriangles(struct Model *ctx);
struct Vector3 ModelMin(struct Model *ctx);

vector(struct Part *) *ModelParts(struct Model *ctx);
char *PartName(struct Part *ctx);
struct Vector3 PartSize(struct Part *ctx);
