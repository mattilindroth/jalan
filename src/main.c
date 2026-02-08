#include <stdio.h>
#include "raylib.h"
#include "scene.h"
#include "game.h"
#include "level_loader.h"
// #include "player.h"
// #include "enemy.h"
// #include "background_object.h"


#define SCREEN_WIDTH 1920//1600
#define SCREEN_HEIGHT 1200 //900

#define ROOM_COUNT 6
#define WORLD_WIDTH (SCREEN_WIDTH * ROOM_COUNT)
#define WORLD_HEIGHT (SCREEN_HEIGHT * 2)

Game *game;
Scene *level_one_scene;

int main() {

    level_one_scene = createScene(1, "Level One");

    // Start directly in the playable level for now
    game = createGame(level_one_scene, STATE_PLAYING);

    if(loadLevel("levels.json", game) == NULL) {
        printf("Failed to load level!\n");
        return -1;
    }

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Jalanallas - The nightmare");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);  // Disable default ESC key exit behavior
    ToggleFullscreen();

    // Initialize camera properly now that window exists
    game->camera.offset = (Vector2){GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};

    while (!WindowShouldClose()) {
        
        int inputResult = handleInput(game);
        if (inputResult == -1) {
            break;  // Exit game loop
        }
        
        updateGame(game);
        updateCamera(game);
        renderGame(game);
        
    }

    CloseWindow();

    destroyGame(game);
    destroyScene(level_one_scene);
    return 0;
}