#ifndef EDITOR_LAYOUT_H
#define EDITOR_LAYOUT_H

typedef struct EditorLayout EditorLayout;

// Editor layout management
EditorLayout* editor_layout_create(int screen_width, int screen_height);
void editor_layout_destroy(EditorLayout* layout);
void editor_layout_update(EditorLayout* layout, float delta_time);
void editor_layout_render(EditorLayout* layout);

#endif // EDITOR_LAYOUT_H