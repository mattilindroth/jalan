#ifndef JALAN_ENGINE_H
#define JALAN_ENGINE_H

#include <raylib.h>
#include "dynamic_array.h"
#include "sprite.h"

typedef struct JalanEngine {
    // Window
    int window_width;
    int window_height;
    const char* window_title;
    DynamicArray *textures;
    DynamicArray *sprites;
} JalanEngine;

JalanEngine *jalan_engine_init(int window_width, int window_height, const char* window_title);

Texture2D* jalan_engine_load_texture(JalanEngine *engine, const char* file_path);

void jalan_engine_add_sprite(JalanEngine *engine, Sprite *sprite);

void jalan_engine_render(JalanEngine *engine);

void jalan_engine_update(JalanEngine *engine);

JalanEngine *jalan_engine_destroy(JalanEngine *engine);


#endif // JALAN_ENGINE_H