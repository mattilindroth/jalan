#ifndef JALAN_ENGINE_H
#define JALAN_ENGINE_H

#include <raylib.h>
#include "dynamic_array.h"
#include "parallax.h"
#include "asset_loader.h"
#include "entity.h"

typedef struct JalanEngine {
    // Window
    int window_width;
    int window_height;
    const char* window_title;
    Texture2D *textures;
    Texture2D *atlasTexture;
    Texture2D *sprites;
    DynamicArray *entities;
    AssetLoader *assetLoader;
    Parallax *parallax;
} JalanEngine;

JalanEngine *jalan_engine_init(int window_width, int window_height, const char* window_title);

Texture2D* jalan_engine_load_texture(JalanEngine *engine, const char* file_path);

Texture2D *jalan_engine_load_sprites(JalanEngine *engine, const char* file_path);

ParallaxLayer *jalan_engine_get_entity_parallax_layer(JalanEngine *engine, Entity *entity);

void jalan_engine_add_parallax_layer(JalanEngine *engine, int layerIndex, float speed);

void jalan_engine_add_entity(JalanEngine *engine, Entity *entity, int parallaxLayerIndex);

void jalan_engine_render(JalanEngine *engine);

void jalan_engine_update(JalanEngine *engine);

JalanEngine *jalan_engine_destroy(JalanEngine *engine);


#endif // JALAN_ENGINE_H