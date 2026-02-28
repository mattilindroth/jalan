#ifndef UI_HH
#define UI_HH

#include "raylib-nuklear.h"

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