//#include <stdio.h>
#include "raylib.h"
#include "jalan_engine.h"

//Todo. These should be configurable in the editor, not harad coded.
#define PARALLAX_LAYER_STATIC_BACKGROUND 0
#define PARALLAX_LAYER_ROLLING_BACKGROUND 1
#define PARALLAX_LAYER_GAME_ENTITIES 2
#define PARALLAX_LAYER_ROLLING_FOREGROUND 3 
#define PARALLAX_LAYER_STATIC_FOREGROUND 4

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 800

/*************
** Structures
**************/

/************************
** Function declarations
*************************/

int updateCamera();

int resetPlayer();

int handleInput();

int updateGame();

int renderFrame();


/**************************
** Game object declarations
***************************/

Camera2D camera = {0};

int main(int argc, char *argv[]) {
    
    JalanEngine *engine;

    engine = jalan_engine_init(SCREEN_WIDTH, SCREEN_HEIGHT, "Jalannallas - the final nightmare V0.2 Alpha");

    //Texture2D wallTexture = LoadTexture("../resources/wooden_wall.png");

    Texture2D *wallTexture = jalan_engine_load_texture(engine, "../resources/wooden_wall.png");

    Sprite *wallSprite = sprite_create(wallTexture, (Vector2){100, 100});

    Entity *wallEntity = entity_create(wallSprite);

    jalan_engine_add_entity(engine, wallEntity, PARALLAX_LAYER_GAME_ENTITIES);
    
    while (!WindowShouldClose()) {       

        // handleInput();

        // updateGame();
        jalan_engine_update(engine);
        // updateCamera();

        BeginDrawing();
            ClearBackground(RAYWHITE);
            jalan_engine_render(engine);
        EndDrawing();
    }

    // Close window and OpenGL context
    jalan_engine_destroy(engine);
    
    return 0;
}

int updateCamera() {
    // Center the camera on the player, but move it 20 pixels down to simulate underground view
    camera.offset = (Vector2){SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
    camera.zoom = 1.0f;

    // Clamp camera to keep game area visible
    float cameraLeft = camera.target.x - camera.offset.x / camera.zoom;
    float cameraRight = camera.target.x + camera.offset.x / camera.zoom;
    float cameraTop = camera.target.y - camera.offset.y / camera.zoom;
    float cameraBottom = camera.target.y + camera.offset.y / camera.zoom;
   
    return 0;
}


int handleInput() {
    
   
    return 0;
}


