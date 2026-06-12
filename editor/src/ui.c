#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ui.h"
#include "dynamic_array.h"
#include "entity.h"
#include "sprite.h"

UI *ui_create(unsigned int screen_width, unsigned int screen_height, int fontSize, JalanEngine *engine) {
    UI *ui = (UI *)malloc(sizeof(UI));
    if (!ui) {
        fprintf(stderr, "Failed to allocate memory for UI\n");
        return NULL;
    }

    if(!engine) {
        fprintf(stderr, "UI creation failed: engine pointer is NULL\n");
        free(ui);
        return NULL;
    }
    
    ui->engine = engine;
    ui->screen_width = screen_width;
    ui->screen_height = screen_height;
    ui->ctx = InitNuklear(fontSize);
    ui->editorState = editor_state_create();
    // Start from the existing layer count so new IDs don't collide
    ui->nextLayerId = parallax_get_layer_count(engine->parallax);

    ui->dragging = false;
    ui->dragTextureAtlas = NULL;
    ui->movingEntity = NULL;
    ui->moveOffset = (Vector2){0, 0};
    ui->resizing = false;

    // Convert atlas texture to Nuklear image for sub-region rendering
    if (engine->atlasTexture) {
        ui->atlasNkImage = TextureToNuklear(*engine->atlasTexture);
    }

    //Add all parallax layers to editor state
    for (int i = 0; i < dynamic_array_size(engine->parallax->layers); i++) {
        ParallaxLayer *layer = (ParallaxLayer *)dynamic_array_get(engine->parallax->layers, i);
        editor_state_add_layer(ui->editorState, layer, "Layer 0");
    }   

    //Add all entities to editor state
    for (int i = 0; i < dynamic_array_size(engine->entities); i++) {
        Entity *entity = (Entity *)dynamic_array_get(engine->entities, i);
        ParallaxLayer *layer = entity->parallaxLayer;

        editor_state_add_entity(ui->editorState, entity);
    }

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

    if (nk_begin(ui->ctx, "Layers", nk_rect(0,TOP_MENU_HEIGHT,LEFT_PANEL_WIDTH,LAYER_PANEL_HEIGHT), NK_WINDOW_BORDER)) {
        nk_layout_row_dynamic(ui->ctx, 30, 1);
        if (nk_button_label(ui->ctx, "New layer")) {
            char name[32];
            snprintf(name, sizeof(name), "Layer %d", ui->nextLayerId);
            jalan_engine_add_parallax_layer(ui->engine, ui->nextLayerId, 1.0f);
            ParallaxLayer *newLayer = parallax_get_layer_by_id(ui->engine->parallax, ui->nextLayerId);
            if (newLayer) {
                editor_state_add_layer(ui->editorState, newLayer, name);
            }
            ui->nextLayerId++;
        }
        // add layer items here
        nk_layout_row_dynamic(ui->ctx, 25, 1);
        for (int i = 0; i < dynamic_array_size(ui->editorState->editorLayers); i++) {
            EditorLayer *layer = (EditorLayer *)dynamic_array_get(ui->editorState->editorLayers, i);
            if (nk_selectable_label(ui->ctx, layer->name, NK_TEXT_LEFT, &layer->selected)) {
                editor_state_set_active_layer(ui->editorState, layer);
            }
        }
    }
    nk_end(ui->ctx);

    if (nk_begin(ui->ctx, "Textures", nk_rect(0, TOP_MENU_HEIGHT + LAYER_PANEL_HEIGHT, LEFT_PANEL_WIDTH, SCREEN_HEIGHT - (TOP_MENU_HEIGHT + LAYER_PANEL_HEIGHT)), NK_WINDOW_BORDER)) {
        if (ui->engine->atlasTexture) {
            int thumbSize = 32;
            int spacing = 4;
            int cols = (LEFT_PANEL_WIDTH - 16) / (thumbSize + spacing); // fit as many as possible
            if (cols < 1) cols = 1;
            int count = (int)dynamic_array_size(ui->engine->assetLoader->textureAtlases);

            // Style the image button to look flat (no background/border)
            struct nk_style_button imgBtnStyle = ui->ctx->style.button;
            imgBtnStyle.normal = nk_style_item_color(nk_rgba(0, 0, 0, 0));
            imgBtnStyle.hover = nk_style_item_color(nk_rgba(80, 80, 80, 100));
            imgBtnStyle.active = nk_style_item_color(nk_rgba(60, 60, 60, 150));
            imgBtnStyle.padding = nk_vec2(0, 0);

            for (int row = 0; row < (count + cols - 1) / cols; row++) {
                nk_layout_row_static(ui->ctx, thumbSize + spacing, thumbSize + spacing, cols);
                for (int col = 0; col < cols; col++) {
                    int idx = row * cols + col;
                    if (idx >= count) break;
                    TextureAtlas *ta = (TextureAtlas *)dynamic_array_get(ui->engine->assetLoader->textureAtlases, idx);
                    struct nk_image thumb = nk_subimage_ptr(
                        ui->atlasNkImage.handle.ptr,
                        ui->engine->atlasTexture->width,
                        ui->engine->atlasTexture->height,
                        nk_rect(ta->positionX, ta->positionY, ta->width, ta->height)
                    );
                    // Get widget bounds before drawing, then use nk_image for display
                    struct nk_rect bounds = nk_widget_bounds(ui->ctx);
                    nk_button_image_styled(ui->ctx, &imgBtnStyle, thumb);

                    // Start drag on mouse PRESS (not release) over this thumbnail
                    if (!ui->dragging && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        Vector2 mp = GetMousePosition();
                        if (mp.x >= bounds.x && mp.x <= bounds.x + bounds.w &&
                            mp.y >= bounds.y && mp.y <= bounds.y + bounds.h) {
                            ui->dragging = true;
                            ui->dragTextureAtlas = ta;
                        }
                    }
                }
            }
        } else {
            nk_layout_row_dynamic(ui->ctx, 25, 1);
            nk_label(ui->ctx, "No atlas loaded", NK_TEXT_CENTERED);
        }
    }
    nk_end(ui->ctx);

    // Handle drag-drop: drop on mouse release
    if (ui->dragging) {
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePos = GetMousePosition();
            // Drop only if mouse is over the scene area (right of the left panel)
            if (mousePos.x > LEFT_PANEL_WIDTH && mousePos.y > TOP_MENU_HEIGHT) {
                EditorLayer *activeLayer = editor_state_get_active_layer(ui->editorState);
                if (!activeLayer) {
                    fprintf(stderr, "Drop failed: no layer selected. Select a layer first.\n");
                } else {
                    // Create a sprite from the atlas sub-region
                    Rectangle srcRect = {
                        (float)ui->dragTextureAtlas->positionX,
                        (float)ui->dragTextureAtlas->positionY,
                        (float)ui->dragTextureAtlas->width,
                        (float)ui->dragTextureAtlas->height
                    };
                    Sprite *sprite = sprite_create(ui->engine->atlasTexture, mousePos);
                    Animation *anim = animation_create(0, 0.0f, (Vector2){0, 0});
                    animation_add_frame(anim, srcRect);
                    sprite_add_animation(sprite, anim);
                    sprite->activeAnimationId = 0;
                    Entity *entity = entity_create(sprite);

                    // Find the index of the active parallax layer
                    int layerIdx = -1;
                    for (int i = 0; i < dynamic_array_size(ui->engine->parallax->layers); i++) {
                        if (dynamic_array_get(ui->engine->parallax->layers, i) == activeLayer->layer) {
                            layerIdx = i;
                            break;
                        }
                    }
                    if (layerIdx >= 0) {
                        jalan_engine_add_entity(ui->engine, entity, layerIdx);
                        editor_state_add_entity(ui->editorState, entity);
                    }
                }
            }
            ui->dragging = false;
            ui->dragTextureAtlas = NULL;
        }
    }

    // Handle entity selection and movement in the scene area
    if (!ui->dragging) {
        Vector2 mousePos = GetMousePosition();
        bool inScene = mousePos.x > LEFT_PANEL_WIDTH && mousePos.y > TOP_MENU_HEIGHT;

        // Handle ongoing resize
        if (ui->resizing) {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                EditorEntity *sel = ui->editorState->selectedEntity;
                if (sel) {
                    float dx = mousePos.x - ui->resizeStartMouse.x;
                    float dy = mousePos.y - ui->resizeStartMouse.y;
                    float newW = ui->resizeBaseWidth * ui->resizeStartScale.x + dx;
                    float newH = ui->resizeBaseHeight * ui->resizeStartScale.y + dy;
                    if (newW > 4.0f) sel->entity->sprite->scale.x = newW / ui->resizeBaseWidth;
                    if (newH > 4.0f) sel->entity->sprite->scale.y = newH / ui->resizeBaseHeight;
                }
            } else {
                ui->resizing = false;
            }
        } else if (ui->movingEntity) {
            // Move the entity while mouse is held
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                ui->movingEntity->entity->sprite->position.x = mousePos.x - ui->moveOffset.x;
                ui->movingEntity->entity->sprite->position.y = mousePos.y - ui->moveOffset.y;
            } else {
                // Mouse released: stop moving
                ui->movingEntity->isDragged = false;
                ui->movingEntity = NULL;
            }
        } else if (inScene && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            // Check if clicking the resize handle of the selected entity
            EditorEntity *sel = ui->editorState->selectedEntity;
            bool startedResize = false;
            if (sel) {
                Sprite *sprite = sel->entity->sprite;
                float fw = (float)sprite->texture->width;
                float fh = (float)sprite->texture->height;
                if (sprite->activeAnimationId >= 0) {
                    for (int idx = 0; idx < (int)dynamic_array_size(sprite->animations); idx++) {
                        Animation *anim = (Animation *)dynamic_array_get(sprite->animations, idx);
                        if (anim->id == sprite->activeAnimationId && dynamic_array_size(anim->frames) > 0) {
                            Rectangle *frame = (Rectangle *)dynamic_array_get(anim->frames, anim->currentFrameIndex);
                            fw = frame->width;
                            fh = frame->height;
                            break;
                        }
                    }
                }
                float scaledW = fw * sprite->scale.x;
                float scaledH = fh * sprite->scale.y;
                // Resize handle: 10x10 square at bottom-right corner (matches render position)
                Rectangle handleRect = {
                    sprite->position.x + scaledW - 4,
                    sprite->position.y + scaledH - 4,
                    10, 10
                };
                if (CheckCollisionPointRec(mousePos, handleRect)) {
                    ui->resizing = true;
                    ui->resizeStartMouse = mousePos;
                    ui->resizeStartScale = sprite->scale;
                    ui->resizeBaseWidth = fw;
                    ui->resizeBaseHeight = fh;
                    startedResize = true;
                }
            }

            if (!startedResize) {
                // Try to pick an entity under cursor in the active layer
                EditorLayer *activeLayer = editor_state_get_active_layer(ui->editorState);
                if (activeLayer) {
                    EditorEntity *hit = editor_state_get_entity_under_cursor(ui->editorState, mousePos);
                    if (hit && hit->entity->parallaxLayer == activeLayer->layer) {
                        // Select this entity
                        editor_state_set_active_entity(ui->editorState, hit);
                        // Start moving
                        ui->movingEntity = hit;
                        hit->isDragged = true;
                        ui->moveOffset.x = mousePos.x - hit->entity->sprite->position.x;
                        ui->moveOffset.y = mousePos.y - hit->entity->sprite->position.y;
                    } else {
                        // Clicked empty space: deselect
                        if (ui->editorState->selectedEntity) {
                            ui->editorState->selectedEntity->selected = false;
                            ui->editorState->selectedEntity = NULL;
                        }
                    }
                }
            }
        }
    }
}

void ui_render(UI *ui) {
    if (!ui) return;
    DrawNuklear(ui->ctx);

    // Draw selection highlight around selected entity
    EditorEntity *sel = ui->editorState->selectedEntity;
    if (sel) {
        Sprite *sprite = sel->entity->sprite;
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
        Rectangle selRect = {
            sprite->position.x - 2,
            sprite->position.y - 2,
            w * sprite->scale.x + 4,
            h * sprite->scale.y + 4
        };
        DrawRectangleLinesEx(selRect, 2.0f, YELLOW);

        // Draw resize handle at bottom-right corner
        Rectangle handleRect = {
            sprite->position.x + w * sprite->scale.x - 4,
            sprite->position.y + h * sprite->scale.y - 4,
            10, 10
        };
        DrawRectangleRec(handleRect, YELLOW);
    }

    // Draw drag preview on top of everything
    if (ui->dragging && ui->dragTextureAtlas) {
        Vector2 mousePos = GetMousePosition();
        Rectangle srcRect = {
            (float)ui->dragTextureAtlas->positionX,
            (float)ui->dragTextureAtlas->positionY,
            (float)ui->dragTextureAtlas->width,
            (float)ui->dragTextureAtlas->height
        };
        Rectangle destRect = {
            mousePos.x - 16, mousePos.y - 16,
            32, 32
        };
        DrawTexturePro(*ui->engine->atlasTexture, srcRect, destRect, (Vector2){0, 0}, 0.0f, WHITE);
    }
}

void ui_destroy(UI *ui) {
    if (!ui) return;
    UnloadNuklear(ui->ctx);
    free(ui);
}