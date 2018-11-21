#include "math.h"

struct Vertex
{
  struct Vector3 position;
  struct Vector3 normal;
  struct Vector2 texCoord;
};

struct Face
{
  struct Vertex a;
  struct Vertex b;
  struct Vertex c;
};

typedef struct Mesh Mesh;

struct Mesh *MeshCreate();
void MeshDestroy(struct Mesh *ctx);

void MeshAddFace(struct Mesh *ctx, struct Face face);

void _MeshDisplay(struct Mesh *ctx);
void _MeshShift(struct Mesh *ctx, struct Vector3 shift);

struct Vector3 MeshCenter(struct Mesh *ctx);
struct Vector3 MeshMax(struct Mesh *ctx);
struct Vector3 MeshMin(struct Mesh *ctx);
