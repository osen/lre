typedef struct Font Font;

struct Font *FontLoad(char *path);
void _FontDestroy(struct Font *ctx);
int FontHeight(struct Font *ctx);
int FontWidth(struct Font *ctx, char *sample);

void _FontDisplay(struct Font *ctx, char *text);
