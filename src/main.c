#include <stdio.h>
#include "raylib.h"
#include "scene.h"
#include "game.h"

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900

Game *game;
Scene *main_menu_scene;
Scene *level_one_scene;

int main() {

    main_menu_scene = createScene(0, "Main Menu");
    
    level_one_scene = createScene(1, "Level One");

    game = createGame(main_menu_scene, STATE_MENU);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Jalanallas - The nightmare");

    while (!WindowShouldClose()) {
        
        handleInput(game);
        updateGame(game);
        updateCamera(game);
        renderGame(game);
    }

    CloseWindow();

    destroyGame(game);
    destroyScene(main_menu_scene);
    destroyScene(level_one_scene);
    return 0;
}