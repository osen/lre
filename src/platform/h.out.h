#include <stdlib.h>

struct HdFile;
struct HdFile *HdFileOpen(char *path);
char *HdFileGets(struct HdFile *ctx, char *buffer, size_t len);
unsigned char *HdFileRead(struct HdFile *ctx, size_t *len);
void HdFileClose(struct HdFile *ctx);
