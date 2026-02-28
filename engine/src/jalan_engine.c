#include <stdlib.h>
#include <stdio.h>
#include "jalan_engine.h"
#include "dynamic_array.h"

JalanEngine *jalan_engine_init(int window_width, int window_height, const char* window_title) {
    JalanEngine *engine = (JalanEngine *)malloc(sizeof(JalanEngine));
    if (!engine) {
        fprintf(stderr, "Failed to allocate memory for JalanEngine\n");
        return NULL;
    }
    
    engine->window_width = window_width;
    engine->window_height = window_height;
    engine->window_title = window_title;
    engine->textures = dynamic_array_create_default();
    engine->entities = dynamic_array_create_default();
    engine->assetLoader = asset_loader_create();
    engine->parallax = parallax_create();

    InitWindow(window_width, window_height, window_title);
    SetTargetFPS(60);

    return engine;
}

void jalan_engine_add_parallax_layer(JalanEngine *engine, int layerIndex, float speed){
    if (!engine) {
        fprintf(stderr, "JalanEngine is not initialized\n");
        return;
    }
    parallax_add_layer(engine->parallax, layerIndex, speed);
}

Texture2D* jalan_engine_load_texture(JalanEngine *engine, const char* file_path) {
    if (!engine) {
        fprintf(stderr, "JalanEngine is not initialized\n");
        return NULL;
    }

    Texture2D *texture = (Texture2D *)malloc(sizeof(Texture2D));
    if (!texture) {
        fprintf(stderr, "Failed to allocate memory for Texture2D\n");
        return NULL;
    }

    *texture = LoadTexture(file_path);
    if (texture->id == 0) {
        fprintf(stderr, "Failed to load texture: %s\n", file_path);
        free(texture);
        return NULL;
    }
    
    dynamic_array_push(engine->textures, texture);
    return texture;
}

void jalan_engine_add_entity(JalanEngine *engine, Entity *entity, int parallaxLayerIndex) {
    if (!engine) {
        fprintf(stderr, "JalanEngine is not initialized\n");
        return;
    }
    
    if (!entity) {
        fprintf(stderr, "Entity is NULL\n");
        return;
    }

    ParallaxLayer *layer = NULL;
    // Add entity to the specified parallax layer
    if (parallaxLayerIndex >= 0 && parallaxLayerIndex < dynamic_array_size(engine->parallax->layers)) {
        ParallaxLayer *layer = (ParallaxLayer *)dynamic_array_get(engine->parallax->layers, parallaxLayerIndex);
        dynamic_array_push(layer->entities, entity);
        entity->parallaxLayer = layer;
    } else {
        fprintf(stderr, "Invalid parallax layer index: %d\n", parallaxLayerIndex);
        return;
    }

    // Assuming you have a dynamic array for entities in the engine
    dynamic_array_push(engine->entities, entity);
}

void jalan_engine_render(JalanEngine *engine) {
    if (!engine) {
        fprintf(stderr, "JalanEngine is not initialized\n");
        return;
    }
    

    //Render parallax layers
    for(size_t i = 0; i < dynamic_array_size(engine->parallax->layers); i++) {
        ParallaxLayer *layer = (ParallaxLayer *)dynamic_array_get(engine->parallax->layers, i);

        // Render entity sprites
        for (size_t i = 0; i < dynamic_array_size(layer->entities); i++) {
            Entity *entity = (Entity *)dynamic_array_get(layer->entities, i);
            sprite_render(entity->sprite);
        }
    }
}

void jalan_engine_update(JalanEngine *engine) {
    if (!engine) {
        fprintf(stderr, "JalanEngine is not initialized\n");
        return;
    }
    
    // Update game logic here (this should be expanded based on actual game needs)
}

JalanEngine *jalan_engine_destroy(JalanEngine *engine) {
    if (!engine) {
        fprintf(stderr, "JalanEngine is not initialized\n");
        return NULL;
    }

    // Unload textures
    for (size_t i = 0; i < dynamic_array_size(engine->textures); i++) {
        Texture2D *texture = (Texture2D *)dynamic_array_get(engine->textures, i);
        UnloadTexture(*texture);
        free(texture);
    }

    // Unload entities
    for (size_t i = 0; i < dynamic_array_size(engine->entities); i++) {
        Entity *entity = (Entity *)dynamic_array_get(engine->entities, i);
        entity_destroy(entity);
    }

    //Close window and OpenGL context
    CloseWindow();

    dynamic_array_destroy(engine->textures);
    dynamic_array_destroy(engine->entities);
    asset_loader_destroy(engine->assetLoader);
    parallax_destroy(engine->parallax);
    free(engine);
    
    return NULL;
}