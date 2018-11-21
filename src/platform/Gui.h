struct Vector4;
struct Image;
struct Font;
struct Vector2;

void _GuiInitialize();
void _GuiCleanup();
void _GuiSetScreenSize(int width, int height);
void _GuiUpdateScreenSize();

void _GuiApplyProjection();

void GuiSetFont(struct Font *font);

void GuiImage(struct Vector4 rect, struct Image *image);

void GuiText(struct Vector2 point, char *text);

struct Vector2 GuiScreenSize();

