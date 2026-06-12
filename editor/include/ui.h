#ifndef UI_HH
#define UI_HH

#include "editor_state.h"
#include "jalan_engine.h"
#include "raylib-nuklear.h"
#include "main.h"

#define TOP_MENU_HEIGHT 30
#define DEFAULT_FONT_SIZE 18
#define LEFT_PANEL_WIDTH 200
#define LAYER_PANEL_HEIGHT 200

typedef struct UI {
    struct nk_context *ctx;
    unsigned int screen_width;
    unsigned int screen_height;
    JalanEngine *engine;
    EditorState *editorState;
    int nextLayerId;            // Counter for generating unique layer IDs
    struct nk_image atlasNkImage; // Atlas PNG converted for Nuklear rendering
    // Drag-drop state
    bool dragging;
    TextureAtlas *dragTextureAtlas; // The atlas entry being dragged
    // Entity move state
    EditorEntity *movingEntity;    // Entity currently being moved
    Vector2 moveOffset;            // Offset between mouse and entity position at drag start
    // Entity resize state
    bool resizing;                 // True while dragging the resize handle
    Vector2 resizeStartMouse;      // Mouse position at resize start
    Vector2 resizeStartScale;      // Entity scale at resize start
    float resizeBaseWidth;         // Frame width at resize start
    float resizeBaseHeight;        // Frame height at resize start
} UI;

UI *ui_create(unsigned int screen_width, unsigned int screen_height, int fontSize, JalanEngine *engine);

void ui_update(UI *ui);

void ui_render(UI *ui);

void ui_destroy(UI *ui);

#endif // UI_HH