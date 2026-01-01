#include "ui/ui_system.h"
#include <stdlib.h>
#include <string.h>

// UI System management
UISystem* ui_system_create(void) {
    UISystem* ui = malloc(sizeof(UISystem));
    if (!ui) return NULL;
    
    ui->root_elements = dynamic_array_create_default();
    ui->hovered_element = NULL;
    ui->focused_element = NULL;
    ui->pressed_element = NULL;
    ui->mouse_pos = (Vector2){0, 0};
    ui->mouse_pressed = false;
    ui->mouse_released = false;
    
    return ui;
}

void ui_system_destroy(UISystem* ui) {
    if (!ui) return;
    
    // Clean up all root elements (will recursively clean children)
    for (size_t i = 0; i < dynamic_array_size(ui->root_elements); i++) {
        UIElement* element = dynamic_array_get(ui->root_elements, i);
        ui_element_destroy(element);
    }
    
    dynamic_array_destroy(ui->root_elements);
    free(ui);
}

void ui_system_update(UISystem* ui, float delta_time) {
    if (!ui) return;
    
    // Update mouse state
    ui->mouse_pos = GetMousePosition();
    ui->mouse_pressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    ui->mouse_released = IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
    
    // Find what element is under the mouse
    UIElement* new_hovered = ui_system_get_element_at(ui, ui->mouse_pos);
    
    // Handle hover changes
    if (ui->hovered_element != new_hovered) {
        if (ui->hovered_element) {
            ui->hovered_element->state = UI_STATE_NORMAL;
        }
        
        ui->hovered_element = new_hovered;
        
        if (ui->hovered_element && ui->hovered_element->interactive) {
            ui->hovered_element->state = UI_STATE_HOVERED;
            if (ui->hovered_element->callbacks.on_hover) {
                ui->hovered_element->callbacks.on_hover(ui->hovered_element);
            }
        }
    }
    
    // Handle mouse press
    if (ui->mouse_pressed && ui->hovered_element && ui->hovered_element->interactive) {
        ui->pressed_element = ui->hovered_element;
        ui->focused_element = ui->hovered_element;
        ui->pressed_element->state = UI_STATE_PRESSED;
    }
    
    // Handle mouse release (click)
    if (ui->mouse_released && ui->pressed_element) {
        if (ui->pressed_element == ui->hovered_element && 
            ui->pressed_element->callbacks.on_click) {
            ui->pressed_element->callbacks.on_click(ui->pressed_element);
        }
        
        ui->pressed_element->state = (ui->pressed_element == ui->hovered_element) 
                                    ? UI_STATE_HOVERED : UI_STATE_NORMAL;
        ui->pressed_element = NULL;
    }
    
    // Update all elements
    for (size_t i = 0; i < dynamic_array_size(ui->root_elements); i++) {
        UIElement* element = dynamic_array_get(ui->root_elements, i);
        if (element->callbacks.on_update) {
            element->callbacks.on_update(element, delta_time);
        }
    }
}

void ui_system_render(UISystem* ui) {
    if (!ui) return;
    
    // Render all root elements (they'll render their children)
    for (size_t i = 0; i < dynamic_array_size(ui->root_elements); i++) {
        UIElement* element = dynamic_array_get(ui->root_elements, i);
        if (element->visible) {
            if (element->callbacks.on_render) {
                element->callbacks.on_render(element);
            } else {
                // Default rendering
                DrawRectangleRec(element->bounds, element->background_color);
                if (element->text) {
                    Vector2 text_pos = {
                        element->bounds.x + 10,
                        element->bounds.y + element->bounds.height/2 - 10
                    };
                    DrawText(element->text, (int)text_pos.x, (int)text_pos.y, 20, element->text_color);
                }
            }
            
            // Render children
            if (element->children) {
                for (size_t j = 0; j < dynamic_array_size(element->children); j++) {
                    UIElement* child = dynamic_array_get(element->children, j);
                    if (child->visible && child->callbacks.on_render) {
                        child->callbacks.on_render(child);
                    }
                }
            }
        }
    }
}

// Element creation
UIElement* ui_element_create(UIElementType type, Rectangle bounds, const char* text) {
    UIElement* element = malloc(sizeof(UIElement));
    if (!element) return NULL;
    
    element->type = type;
    element->state = UI_STATE_NORMAL;
    element->bounds = bounds;
    element->background_color = LIGHTGRAY;
    element->text_color = BLACK;
    element->text = text ? strdup(text) : NULL;
    element->visible = true;
    element->interactive = true;
    
    element->parent = NULL;
    element->children = dynamic_array_create_default();
    
    // Clear callbacks
    memset(&element->callbacks, 0, sizeof(UICallbacks));
    
    element->user_data = NULL;
    
    return element;
}

void ui_element_destroy(UIElement* element) {
    if (!element) return;
    
    // Destroy children first
    if (element->children) {
        for (size_t i = 0; i < dynamic_array_size(element->children); i++) {
            UIElement* child = dynamic_array_get(element->children, i);
            ui_element_destroy(child);
        }
        dynamic_array_destroy(element->children);
    }
    
    free(element->text);
    free(element);
}

void ui_element_add_child(UIElement* parent, UIElement* child) {
    if (!parent || !child) return;
    
    child->parent = parent;
    
    // Convert child's relative coordinates to absolute coordinates
    child->bounds.x += parent->bounds.x;
    child->bounds.y += parent->bounds.y;
    
    dynamic_array_push(parent->children, child);
}

// Hit testing - finds topmost element at position
UIElement* ui_system_get_element_at(UISystem* ui, Vector2 position) {
    if (!ui) return NULL;
    
    // Check root elements in reverse order (topmost first)
    for (int i = (int)dynamic_array_size(ui->root_elements) - 1; i >= 0; i--) {
        UIElement* element = dynamic_array_get(ui->root_elements, (size_t)i);
        UIElement* hit = ui_element_get_hit(element, position);
        if (hit) return hit;
    }
    
    return NULL;
}

// Helper function for recursive hit testing
UIElement* ui_element_get_hit(UIElement* element, Vector2 position) {
    if (!element || !element->visible) return NULL;
    
    // Check children first (they're on top)
    if (element->children) {
        for (int i = (int)dynamic_array_size(element->children) - 1; i >= 0; i--) {
            UIElement* child = dynamic_array_get(element->children, (size_t)i);
            UIElement* hit = ui_element_get_hit(child, position);
            if (hit) return hit;
        }
    }
    
    // Check this element
    if (ui_element_contains_point(element, position)) {
        return element;
    }
    
    return NULL;
}

bool ui_element_contains_point(UIElement* element, Vector2 point) {
    return CheckCollisionPointRec(point, element->bounds);
}

// Layout helpers
UIElement* ui_create_panel(Rectangle bounds, Color bg_color) {
    UIElement* panel = ui_element_create(UI_PANEL, bounds, NULL);
    if (panel) {
        panel->background_color = bg_color;
        panel->interactive = false; // Panels usually don't interact
    }
    return panel;
}

UIElement* ui_create_button(Rectangle bounds, const char* text, void (*on_click)(UIElement*)) {
    UIElement* button = ui_element_create(UI_BUTTON, bounds, text);
    if (button) {
        button->callbacks.on_click = on_click;
        button->callbacks.on_render = ui_button_render; // You'll need to implement this
        button->background_color = GRAY;
        button->text_color = WHITE;
    }
    return button;
}

UIElement* ui_create_label(Vector2 position, const char* text) {
    Rectangle bounds = { position.x, position.y, 200, 30 }; // Default size
    UIElement* label = ui_element_create(UI_LABEL, bounds, text);
    if (label) {
        label->interactive = false;
        label->background_color = BLANK; // Transparent
        label->text_color = BLACK;
    }
    return label;
}

// Button rendering callback
void ui_button_render(UIElement* element) {
    Color bg_color = element->background_color;
    
    // Change color based on state
    switch (element->state) {
        case UI_STATE_HOVERED:
            bg_color = ColorBrightness(bg_color, 0.2f);
            break;
        case UI_STATE_PRESSED:
            bg_color = ColorBrightness(bg_color, -0.2f);
            break;
        default:
            break;
    }
    
    DrawRectangleRec(element->bounds, bg_color);
    DrawRectangleLinesEx(element->bounds, 2, BLACK);
    
    if (element->text) {
        Vector2 text_size = MeasureTextEx(GetFontDefault(), element->text, 20, 1);
        Vector2 text_pos = {
            element->bounds.x + (element->bounds.width - text_size.x) / 2,
            element->bounds.y + (element->bounds.height - text_size.y) / 2
        };
        DrawText(element->text, (int)text_pos.x, (int)text_pos.y, 20, element->text_color);
    }
}