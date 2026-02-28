#ifndef UI_HH
#define UI_HH

#include "raylib-nuklear.h"

#define TOP_MENU_HEIGHT 30
#define DEFAULT_FONT_SIZE 18
#define LEFT_PANEL_WIDTH 200


typedef struct UI {
    struct nk_context *ctx;
    unsigned int screen_width;
    unsigned int screen_height;
} UI;

UI *ui_create(unsigned int screen_width, unsigned int screen_height, int fontSize);

void ui_update(UI *ui);

void ui_render(UI *ui);

void ui_destroy(UI *ui);

#endif // UI_HH