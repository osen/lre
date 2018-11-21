struct Image;
struct Font;
struct Model;
struct Animation;

void _CacheInitialize();
void _CacheCleanup();

struct Image *_CacheFetchImage(char *path);
void _CacheStoreImage(struct Image *image, char *path);
void _CachePurgeImage(struct Image *image);

struct Font *_CacheFetchFont(char *path);
void _CacheStoreFont(struct Font *font, char *path);
void _CachePurgeFont(struct Font *font);

struct Model *_CacheFetchModel(char *path);
void _CacheStoreModel(struct Model *model, char *path);
void _CachePurgeModel(struct Model *model);

struct Animation *_CacheFetchAnimation(char *path);
void _CacheStoreAnimation(struct Animation *animation, char *path);
void _CachePurgeAnimation(struct Animation *animation);
