#include "ui/ui_system.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    UISystem* ui;
    UIElement* left_panel;    // Entity list
    UIElement* right_panel;   // Scene view
    UIElement* bottom_panel;  // Properties
    UIElement* top_panel;     // Toolbar
    
    UIElement* play_button;
    UIElement* save_button;
    UIElement* load_button;
} EditorLayout;

// Button callbacks
void on_play_clicked(UIElement* element) {
    printf("Play button clicked!\n");
}

void on_play_hover(UIElement* element) {
    // Example hover effect: change background color
    printf("Play button hovered!\n");
}

void on_save_clicked(UIElement* element) {
    printf("Save button clicked!\n");
}

void on_save_hover(UIElement* element) {
    printf("Save button hovered!\n");
}

void on_load_clicked(UIElement* element) {
    printf("Load button clicked!\n");
}

void on_load_hover(UIElement* element) {
    printf("Load button hovered!\n");
}

EditorLayout* editor_layout_create(int screen_width, int screen_height) {
    EditorLayout* layout = malloc(sizeof(EditorLayout));
    if (!layout) return NULL;
    
    layout->ui = ui_system_create();
    
    // Define panel dimensions
    const int toolbar_height = 60;
    const int left_panel_width = 250;
    const int bottom_panel_height = 200;
    
    // Create panels
    Rectangle top_rect = { 0, 0, screen_width, toolbar_height };
    layout->top_panel = ui_create_panel(top_rect, DARKGRAY);
    dynamic_array_push(layout->ui->root_elements, layout->top_panel);
    
    Rectangle left_rect = { 0, toolbar_height, left_panel_width, screen_height - toolbar_height - bottom_panel_height };
    layout->left_panel = ui_create_panel(left_rect, LIGHTGRAY);
    dynamic_array_push(layout->ui->root_elements, layout->left_panel);
    
    Rectangle right_rect = { left_panel_width, toolbar_height, screen_width - left_panel_width, screen_height - toolbar_height - bottom_panel_height };
    layout->right_panel = ui_create_panel(right_rect, WHITE);
    dynamic_array_push(layout->ui->root_elements, layout->right_panel);
    
    Rectangle bottom_rect = { 0, screen_height - bottom_panel_height, screen_width, bottom_panel_height };
    layout->bottom_panel = ui_create_panel(bottom_rect, GRAY);
    dynamic_array_push(layout->ui->root_elements, layout->bottom_panel);
    
    // Create toolbar buttons
    layout->play_button = ui_create_button((Rectangle){10, 10, 80, 40}, "Play", on_play_clicked);
    layout->play_button->callbacks.on_hover = on_play_hover;
    ui_element_add_child(layout->top_panel, layout->play_button);
    
    layout->save_button = ui_create_button((Rectangle){100, 10, 80, 40}, "Save", on_save_clicked);
    layout->save_button->callbacks.on_hover = on_save_hover;
    ui_element_add_child(layout->top_panel, layout->save_button);
    
    layout->load_button = ui_create_button((Rectangle){190, 10, 80, 40}, "Load", on_load_clicked);
    layout->load_button->callbacks.on_hover = on_load_hover;
    ui_element_add_child(layout->top_panel, layout->load_button);
    
    // Add labels to panels
    UIElement* left_label = ui_create_label((Vector2){10, 10}, "Entities & Scenes");
    ui_element_add_child(layout->left_panel, left_label);
    
    UIElement* right_label = ui_create_label((Vector2){10, 10}, "Scene View");
    ui_element_add_child(layout->right_panel, right_label);
    
    UIElement* bottom_label = ui_create_label((Vector2){10, 10}, "Properties");
    ui_element_add_child(layout->bottom_panel, bottom_label);
    
    return layout;
}

void editor_layout_destroy(EditorLayout* layout) {
    if (!layout) return;
    
    ui_system_destroy(layout->ui);
    free(layout);
}

void editor_layout_update(EditorLayout* layout, float delta_time) {
    ui_system_update(layout->ui, delta_time);
}

void editor_layout_render(EditorLayout* layout) {
    ui_system_render(layout->ui);
    
    // Draw panel borders
    DrawRectangleLinesEx(layout->left_panel->bounds, 2, BLACK);
    DrawRectangleLinesEx(layout->right_panel->bounds, 2, BLACK);
    DrawRectangleLinesEx(layout->bottom_panel->bounds, 2, BLACK);
    DrawRectangleLinesEx(layout->top_panel->bounds, 2, BLACK);
}