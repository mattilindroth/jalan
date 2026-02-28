
#include <stdio.h>
#include "asset_loader.h"
#include "entity.h"
#include "raylib.h"
#include "nuklear.h"
#include "jalan_engine.h"

#include "ui.h"

#define PARALLAX_LAYER_STATIC_BACKGROUND 0
#define PARALLAX_LAYER_ROLLING_BACKGROUND 1
#define PARALLAX_LAYER_GAME_ENTITIES 2
#define PARALLAX_LAYER_ROLLING_FOREGROUND 3 
#define PARALLAX_LAYER_STATIC_FOREGROUND 4

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 1200

int main(int argc, char *argv[]) {
    JalanEngine *engine;

    // Initialize the game engine (Creates window and context)
    engine = jalan_engine_init(SCREEN_WIDTH, SCREEN_HEIGHT, "Jalannallas - editor V0.2 Alpha");

    //Init nuklear
    UI *ui = ui_create(SCREEN_WIDTH, SCREEN_HEIGHT, 18);

    AssetLoader *assetLoader = asset_loader_create();

    printf("Loading texture atlas...\n");
    asset_loader_load_texture_atlas(assetLoader, "../resources/texture.json");
    printf("Texture atlas loaded.\n");
    printf("Loaded %zu texture atlases\n", dynamic_array_size(assetLoader->textureAtlases));
    for(int i = 0; i < dynamic_array_size(assetLoader->textureAtlases); i++) {
        TextureAtlas *atlas = (TextureAtlas *)dynamic_array_get(assetLoader->textureAtlases, i);
        printf("Atlas %d: %s at (%d, %d) size (%d x %d)\n", i, atlas->name, atlas->positionX, atlas->positionY, atlas->width, atlas->height);
    }

    Texture2D *wallTexture = jalan_engine_load_texture(engine, "../resources/wooden_wall.png");
    Texture2D *backgroundTexture = jalan_engine_load_texture(engine, "../resources/night_sky.png");
    jalan_engine_add_parallax_layer(engine, PARALLAX_LAYER_STATIC_BACKGROUND, 0);
    jalan_engine_add_parallax_layer(engine, PARALLAX_LAYER_ROLLING_BACKGROUND, 0.2f);
    jalan_engine_add_parallax_layer(engine, PARALLAX_LAYER_GAME_ENTITIES, 1.0f);
    jalan_engine_add_parallax_layer(engine, PARALLAX_LAYER_ROLLING_FOREGROUND, 1.5f);
    jalan_engine_add_parallax_layer(engine, PARALLAX_LAYER_STATIC_FOREGROUND, 0);

    Sprite *wallSprite = sprite_create(wallTexture, (Vector2){300, 300});
    Sprite *backgroundSprite = sprite_create(backgroundTexture, (Vector2){LEFT_PANEL_WIDTH, TOP_MENU_HEIGHT});
    Entity *wallEntity = entity_create(wallSprite);
    Entity *backgroundEntity = entity_create(backgroundSprite);

    jalan_engine_add_entity(engine, backgroundEntity, PARALLAX_LAYER_STATIC_BACKGROUND);
    jalan_engine_add_entity(engine, wallEntity, PARALLAX_LAYER_GAME_ENTITIES);
    
    while (!WindowShouldClose()) {       

        ui_update(ui);

        BeginDrawing();
            ClearBackground(RAYWHITE);
            jalan_engine_render(engine);
            ui_render(ui);
        EndDrawing();

        // jalan_engine_render handles BeginDrawing/ClearBackground/EndDrawing internally
        
    }

    fprintf(stderr, "Exiting game loop, cleaning up resources...\n");

    ui_destroy(ui);

    fprintf(stderr, "UI resources cleaned up.\n");
    // Close window and OpenGL context
    jalan_engine_destroy(engine);
}