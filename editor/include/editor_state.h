#ifndef EDITOR_STATE_HH
#define EDITOR_STATE_HH

#include "entity.h"
#include "parallax.h"
#include "dynamic_array.h"
#include <stdbool.h>

typedef struct EditorEntity {
    Entity *entity;
    bool selected;
    bool isDragged;
    bool hovered;
    //bool resizing;
} EditorEntity;

EditorEntity *editor_entity_create(Entity *entity);

EditorEntity *editor_entity_destroy(EditorEntity *editorEntity);

typedef struct EditorLayer {
    ParallaxLayer *layer;
    char *name; // Optional: name for the layer
    bool selected;
    bool visible;
    bool locked;
} EditorLayer;

EditorLayer *editor_layer_create(ParallaxLayer *layer, const char *name);

EditorLayer *editor_layer_destroy(EditorLayer *editorLayer);

typedef struct EditorState {
    DynamicArray *editorLayers; // Array of EditorLayer
    DynamicArray *editorEntities; // Array of EditorEntity
    EditorEntity *selectedEntity;
    EditorLayer *selectedLayer;
} EditorState;

EditorState *editor_state_create();

EditorLayer *editor_state_get_active_layer(EditorState *state);

int editor_state_set_active_layer(EditorState *state, EditorLayer *layer);

EditorEntity *editor_state_get_active_entity(EditorState *state);

EditorLayer *editor_state_get_layer_of_entity(EditorState *state, EditorEntity *entity);

int editor_state_set_active_entity(EditorState *state, EditorEntity *entity);

void editor_state_add_entity(EditorState *state, Entity *entity);

void editor_state_add_layer(EditorState *state, ParallaxLayer *layer, const char *name);

EditorEntity *editor_state_get_entity_under_cursor(EditorState *state, Vector2 cursorPos);

EditorState *editor_state_destroy(EditorState *state);

#endif // EDITOR_STATE_HH