
#include <stdio.h>
#include "raylib.h"
#include "nuklear.h"
#include "jalan_engine.h"

#include "ui.h"

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 1200

int main(int argc, char *argv[]) {
    JalanEngine *engine;

    // Initialize the game engine (Creates window and context)
    engine = jalan_engine_init(SCREEN_WIDTH, SCREEN_HEIGHT, "Jalannallas - editor V0.2 Alpha");

    //Init nuklear
    UI *ui = ui_create(SCREEN_WIDTH, SCREEN_HEIGHT, 18);

    Texture2D *wallTexture = jalan_engine_load_texture(engine, "../resources/wooden_wall.png");

    Sprite *wallSprite = sprite_create(wallTexture, (Vector2){300, 300});

    jalan_engine_add_sprite(engine, wallSprite);
    
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