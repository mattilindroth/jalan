#include <stdio.h>
#include <stdlib.h>

#include "ui.h"

UI *ui_create(unsigned int screen_width, unsigned int screen_height, int fontSize) {
    UI *ui = (UI *)malloc(sizeof(UI));
    if (!ui) {
        fprintf(stderr, "Failed to allocate memory for UI\n");
        return NULL;
    }
    ui->screen_width = screen_width;
    ui->screen_height = screen_height;
    ui->ctx = InitNuklear(fontSize);
    return ui;
}

void ui_update(UI *ui) {
    if (!ui) return;
    UpdateNuklear(ui->ctx);

    // declare your UI

    //Declare top menu
    if(nk_begin(ui->ctx, "Top Menu", nk_rect(0,0,ui->screen_width,TOP_MENU_HEIGHT), NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BORDER)) {
        nk_menubar_begin(ui->ctx);
        nk_layout_row_static(ui->ctx, 25, 60, 3);

        if (nk_menu_begin_label(ui->ctx, "File", NK_TEXT_LEFT, nk_vec2(100, 120))) {
            nk_layout_row_dynamic(ui->ctx, 25, 1);
            if (nk_menu_item_label(ui->ctx, "New",  NK_TEXT_LEFT)) { /* ... */ }
            if (nk_menu_item_label(ui->ctx, "Open", NK_TEXT_LEFT)) { /* ... */ }
            if (nk_menu_item_label(ui->ctx, "Save", NK_TEXT_LEFT)) { /* ... */ }
            nk_menu_end(ui->ctx);
        }
        if (nk_menu_begin_label(ui->ctx, "Edit", NK_TEXT_LEFT, nk_vec2(100, 60))) {
            nk_layout_row_dynamic(ui->ctx, 25, 1);
            // add edit items here
            nk_menu_end(ui->ctx);
        }
        if (nk_menu_begin_label(ui->ctx, "View", NK_TEXT_LEFT, nk_vec2(100, 60))) {
            nk_layout_row_dynamic(ui->ctx, 25, 1);
            // add view items here
            nk_menu_end(ui->ctx);
        }
        nk_menubar_end(ui->ctx);
    }
    nk_end(ui->ctx);

    if (nk_begin(ui->ctx, "My Panel", nk_rect(0,TOP_MENU_HEIGHT,LEFT_PANEL_WIDTH,500), NK_WINDOW_BORDER)) {
        nk_layout_row_dynamic(ui->ctx, 30, 1);
        if (nk_button_label(ui->ctx, "Click me")) { /* ... */ }
    }
    nk_end(ui->ctx);
}

void ui_render(UI *ui) {
    if (!ui) return;
    DrawNuklear(ui->ctx);
}

void ui_destroy(UI *ui) {
    if (!ui) return;
    UnloadNuklear(ui->ctx);
    free(ui);
}