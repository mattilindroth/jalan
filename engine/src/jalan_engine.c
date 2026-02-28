#include <stdlib.h>
#include <stdio.h>
#include "jalan_engine.h"

JalanEngine *jalan_engine_init(int window_width, int window_height, const char* window_title) {
    JalanEngine *engine = (JalanEngine *)malloc(sizeof(JalanEngine));
    if (!engine) {
        fprintf(stderr, "Failed to allocate memory for JalanEngine\n");
        return NULL;
    }
    
    engine->window_width = window_width;
    engine->window_height = window_height;
    engine->window_title = window_title;
    engine->textures = dynamic_array_create(sizeof(Texture2D));
    engine->sprites = dynamic_array_create(sizeof(Sprite *));

    InitWindow(window_width, window_height, window_title);
    SetTargetFPS(60);

    return engine;
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

void jalan_engine_add_sprite(JalanEngine *engine, Sprite *sprite) {
    if (!engine) {
        fprintf(stderr, "JalanEngine is not initialized\n");
        return;
    }
    
    if (!sprite) {
        fprintf(stderr, "Sprite is NULL\n");
        return;
    }
    
    // Assuming you have a dynamic array for sprites in the engine
    dynamic_array_push(engine->sprites, sprite);
}

void jalan_engine_render(JalanEngine *engine) {
    if (!engine) {
        fprintf(stderr, "JalanEngine is not initialized\n");
        return;
    }
    
    // Example rendering code (this should be expanded based on actual game needs)

    // Render sprites
    for (size_t i = 0; i < dynamic_array_size(engine->sprites); i++) {
        Sprite *sprite = (Sprite *)dynamic_array_get(engine->sprites, i);
        sprite_render(sprite);
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

    for(size_t i = 0; i < dynamic_array_size(engine->sprites); i++) {
        Sprite *sprite = (Sprite *)dynamic_array_get(engine->sprites, i);
        sprite_destroy(sprite);
    }
    
    //Close window and OpenGL context
    CloseWindow();

    dynamic_array_destroy(engine->textures);
    free(engine);
    
    return NULL;
}