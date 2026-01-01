#ifndef UI_SYSTEM_H
#define UI_SYSTEM_H

#include <raylib.h>
#include <stdbool.h>
#include "core/dynamic_array.h"

typedef enum {
    UI_PANEL,
    UI_BUTTON,
    UI_LABEL,
    UI_LISTBOX,
    UI_TREE_VIEW
} UIElementType;

typedef enum {
    UI_STATE_NORMAL,
    UI_STATE_HOVERED,
    UI_STATE_PRESSED,
    UI_STATE_DISABLED
} UIState;

typedef struct UIElement UIElement;

typedef struct {
    void (*on_click)(UIElement* element);
    void (*on_hover)(UIElement* element);
    void (*on_update)(UIElement* element, float delta_time);
    void (*on_render)(UIElement* element);
} UICallbacks;

struct UIElement {
    UIElementType type;
    UIState state;
    Rectangle bounds;
    Color background_color;
    Color text_color;
    char* text;
    bool visible;
    bool interactive;
    
    // Hierarchy
    UIElement* parent;
    DynamicArray* children; // Array of UIElement*
    
    // Callbacks
    UICallbacks callbacks;
    
    // Type-specific data
    void* user_data;
};

typedef struct {
    DynamicArray* root_elements; // Top-level UI elements
    UIElement* hovered_element;
    UIElement* focused_element;
    UIElement* pressed_element;
    Vector2 mouse_pos;
    bool mouse_pressed;
    bool mouse_released;
} UISystem;

// UI System management
UISystem* ui_system_create(void);
void ui_system_destroy(UISystem* ui);
void ui_system_update(UISystem* ui, float delta_time);
void ui_system_render(UISystem* ui);

// Element creation
UIElement* ui_element_create(UIElementType type, Rectangle bounds, const char* text);
void ui_element_destroy(UIElement* element);
void ui_element_add_child(UIElement* parent, UIElement* child);
UIElement* ui_element_remove_child(UIElement* parent, UIElement* child);

// Layout helpers
UIElement* ui_create_panel(Rectangle bounds, Color bg_color);
UIElement* ui_create_button(Rectangle bounds, const char* text, void (*on_click)(UIElement*));
UIElement* ui_create_label(Vector2 position, const char* text);

// Hit testing
UIElement* ui_system_get_element_at(UISystem* ui, Vector2 position);
UIElement* ui_element_get_hit(UIElement* element, Vector2 position);
bool ui_element_contains_point(UIElement* element, Vector2 point);

// Rendering callbacks
void ui_button_render(UIElement* element);

#endif // UI_SYSTEM_H