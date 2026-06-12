#include "editor_state.h"
#include "dynamic_array.h"
#include "entity.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/** EDITOR ENTITY */
EditorEntity *editor_entity_create(Entity *entity) {
    EditorEntity *editorEntity = (EditorEntity *)malloc(sizeof(EditorEntity));
    if (!editorEntity) {
        fprintf(stderr, "Failed to allocate memory for EditorEntity\n");
        return NULL;
    }
    editorEntity->entity = entity;
    editorEntity->selected = false;
    editorEntity->isDragged = false;
    editorEntity->hovered = false;
    return editorEntity;
}

EditorEntity *editor_entity_destroy(EditorEntity *editorEntity) {
    if (!editorEntity) return NULL;
    entity_destroy(editorEntity->entity);
    free(editorEntity);
    return NULL;
}

/** EDITOR LAYER */

EditorLayer *editor_layer_create(ParallaxLayer *layer, const char *name) {
    EditorLayer *editorLayer = (EditorLayer *)malloc(sizeof(EditorLayer));
    if (!editorLayer) {
        fprintf(stderr, "Failed to allocate memory for EditorLayer\n");
        return NULL;
    }
    editorLayer->layer = layer;
    editorLayer->name = strdup(name); // Duplicate the name string
    editorLayer->selected = false;
    editorLayer->visible = true;
    editorLayer->locked = false;
    return editorLayer;
}

EditorLayer *editor_layer_destroy(EditorLayer *editorLayer) {
    if (!editorLayer) return NULL;
    free(editorLayer->layer);
    free(editorLayer->name);
    free(editorLayer);
    return NULL;
}

/** EDITOR STATE */
EditorState *editor_state_create() {
    EditorState *state = (EditorState *)malloc(sizeof(EditorState));
    if (!state) {
        fprintf(stderr, "Failed to allocate memory for EditorState\n");
        return NULL;
    }
    state->editorLayers = dynamic_array_create_default();
    state->editorEntities = dynamic_array_create_default();
    state->selectedEntity = NULL; // No active layer by default
    state->selectedLayer = NULL; // No active entity by default
    return state;
}

EditorLayer *editor_state_get_active_layer(EditorState *state) {
    if (!state) return NULL;
    return state->selectedLayer;
}

int editor_state_set_active_layer(EditorState *state, EditorLayer *layer) {
    if (!state) return -1;

    //Ensure the layer exists in the editor state before setting it as active
    bool layerExists = false;
    for (size_t i = 0; i < dynamic_array_size(state->editorLayers); i++) {
        EditorLayer *existingLayer = (EditorLayer *)dynamic_array_get(state->editorLayers, i);
        existingLayer->selected = false; // Deselect all layers
        if (existingLayer == layer) {
            layerExists = true;
            existingLayer->selected = true; // Select the new active layer
        }
    }
    if (!layerExists) {
        fprintf(stderr, "Layer not found in editor state\n");
        return -1; // Layer not found
    }

    state->selectedLayer = layer;
    return 0; // Success
}

EditorEntity *editor_state_get_active_entity(EditorState *state) {
    if (!state) return NULL;
    return state->selectedEntity;
}

int editor_state_set_active_entity(EditorState *state, EditorEntity *entity) {
    if (!state) return -1;

    // Ensure the entity exists in the editor state before setting it as active
    bool entityExists = false;
    for (size_t i = 0; i < dynamic_array_size(state->editorEntities); i++) {
        EditorEntity *existingEntity = (EditorEntity *)dynamic_array_get(state->editorEntities, i);
        existingEntity->selected = false; // Deselect all entities
        if (existingEntity == entity) {
            entityExists = true;
            existingEntity->selected = true; // Select the new active entity
        }
    }
    if (!entityExists) {
        fprintf(stderr, "Entity not found in editor state\n");
        return -1; // Entity not found
    }

    state->selectedEntity = entity;
    return 0; // Success
}

void editor_state_add_entity(EditorState *state, Entity *entity) {
    if (!state || !entity) return;

    EditorEntity *editorEntity = editor_entity_create(entity);
    if (!editorEntity) {
        fprintf(stderr, "Failed to create EditorEntity\n");
        return;
    }
    dynamic_array_push(state->editorEntities, editorEntity);
}

EditorLayer *editor_state_get_layer_of_entity(EditorState *state, EditorEntity *entity) {
    if (!state || !entity) return NULL;

    for(size_t i = 0; i < dynamic_array_size(state->editorLayers); i++) {
        EditorLayer *editorLayer = (EditorLayer *)dynamic_array_get(state->editorLayers, i);
        if (editorLayer->layer == entity->entity->parallaxLayer) {
            return editorLayer;
        }
    }
    return NULL; // Layer not found for the given entity
}

void editor_state_add_layer(EditorState *state, ParallaxLayer *layer, const char *name) {
    if (!state || !layer) return;

    EditorLayer *editorLayer = editor_layer_create(layer, name);
    if (!editorLayer) {
        fprintf(stderr, "Failed to create EditorLayer\n");
        return;
    }
    dynamic_array_push(state->editorLayers, editorLayer);
}

EditorEntity *editor_state_get_entity_under_cursor(EditorState *state, Vector2 cursorPos) {
    if (!state) return NULL;

    // Iterate in reverse to get the topmost entity first
    for (int i = dynamic_array_size(state->editorEntities) - 1; i >= 0; i--) {
        EditorEntity *editorEntity = (EditorEntity *)dynamic_array_get(state->editorEntities, i);
        Entity *entity = editorEntity->entity;
        Sprite *sprite = entity->sprite;

        // Use animation frame dimensions if available, otherwise full texture
        float w = (float)sprite->texture->width;
        float h = (float)sprite->texture->height;
        if (sprite->activeAnimationId >= 0) {
            for (int idx = 0; idx < (int)dynamic_array_size(sprite->animations); idx++) {
                Animation *anim = (Animation *)dynamic_array_get(sprite->animations, idx);
                if (anim->id == sprite->activeAnimationId && dynamic_array_size(anim->frames) > 0) {
                    Rectangle *frame = (Rectangle *)dynamic_array_get(anim->frames, anim->currentFrameIndex);
                    w = frame->width;
                    h = frame->height;
                    break;
                }
            }
        }

        Rectangle entityRect = {
            sprite->position.x,
            sprite->position.y,
            w * sprite->scale.x,
            h * sprite->scale.y
        };
        if (CheckCollisionPointRec(cursorPos, entityRect)) {
            return editorEntity; // Return the first (topmost) entity under the cursor
        }
    }
    return NULL; // No entity found under cursor
}

EditorState *editor_state_destroy(EditorState *state) {
    if (!state) return NULL;

    // Free editor layers
    for (size_t i = 0; i < dynamic_array_size(state->editorLayers); i++) {
        EditorLayer *editorLayer = (EditorLayer *)dynamic_array_get(state->editorLayers, i);
        editor_layer_destroy(editorLayer);
    }
    dynamic_array_destroy(state->editorLayers);

    // Free editor entities
    for (size_t i = 0; i < dynamic_array_size(state->editorEntities); i++) {
        EditorEntity *editorEntity = (EditorEntity *)dynamic_array_get(state->editorEntities, i);
        editor_entity_destroy(editorEntity);
    }
    dynamic_array_destroy(state->editorEntities);

    free(state);
    return NULL;
}
