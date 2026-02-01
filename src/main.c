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

    //Create player and the enemy
    // Player *player = createPlayer(1, (Vector2){100.0f, 850.0f}, 80, 80);
    // Enemy *enemy = createEnemy(1, 1, (Vector2){200.0f, 850.0f}, (Vector2){150.0f, 150.0f}, (Vector2){250.0f, 250.0f}, 100.0f, true);

    //Create obstacles and add them to the game
    // Obstacle *bedPost = createObstacle(1, 1, (Rectangle){50.0f, 900.0f, 10.0f, 100.0f}, DARKBROWN);
    // addObstacleToGame(game, bedPost);
    // Obstacle *bed = createObstacle(2, 1, (Rectangle){60.0f, 950.0f, 300.0f, 40.0f}, GREEN);
    // addObstacleToGame(game, bed);
    // Obstacle *bedPost2 = createObstacle(3, 1, (Rectangle){360.0f, 900.0f, 10.0f, 100.0f}, DARKBROWN);
    // addObstacleToGame(game, bedPost2);

    //Create room background 
    // BackgroundObject *roomBackground = createBackgroundObject(1, 1, (Rectangle){0.0f, 0.0f, 1920.0f, 1080.0f}, LIGHTGRAY);
    // addBackgroundObjectToGame(game, roomBackground);
    //Create background window objects
    // BackgroundObject *windowBackground = createBackgroundObject(2, 1, (Rectangle){600.0f, 550.0f, 200.0f, 150.0f}, DARKBLUE);
    // addBackgroundObjectToGame(game, windowBackground);
    // BackgroundObject *windowBackground2 = createBackgroundObject(3, 1, (Rectangle){1000.0f, 550.0f, 200.0f, 150.0f}, DARKBLUE);
    // addBackgroundObjectToGame(game, windowBackground2);

    //Create window bars
    // BackgroundObject *windowBar1 = createBackgroundObject(4, 1, (Rectangle){610.0f, 550.0f, 10.0f, 150.0f}, DARKGRAY);
    // addBackgroundObjectToGame(game, windowBar1);
    // BackgroundObject *windowBar2 = createBackgroundObject(4, 1, (Rectangle){700.0f, 550.0f, 10.0f, 150.0f}, DARKGRAY);
    // addBackgroundObjectToGame(game, windowBar2);
    // BackgroundObject *windowBar3 = createBackgroundObject(4, 1, (Rectangle){790.0f, 550.0f, 10.0f, 150.0f}, DARKGRAY);
    // addBackgroundObjectToGame(game, windowBar3);

    // BackgroundObject *windowBar4 = createBackgroundObject(4, 1, (Rectangle){1010.0f, 550.0f, 10.0f, 150.0f}, DARKGRAY);
    // addBackgroundObjectToGame(game, windowBar4);
    // BackgroundObject *windowBar5 = createBackgroundObject(4, 1, (Rectangle){1100.0f, 550.0f, 10.0f, 150.0f}, DARKGRAY);
    // addBackgroundObjectToGame(game, windowBar5);
    // BackgroundObject *windowBar6 = createBackgroundObject(4, 1, (Rectangle){1190.0f, 550.0f, 10.0f, 150.0f}, DARKGRAY);
    // addBackgroundObjectToGame(game, windowBar6);
        
    // game->player = player;
    // game->enemy = enemy;

    // Floor belongs to Level One (scene id 1)
    // Obstacle *floorObstacle = createObstacle(4, 1, (Rectangle){0.0f,1000.0f, 1920.0f, 100.0f}, GRAY);
    // addObstacleToGame(game, floorObstacle);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Jalanallas - The nightmare");
    SetTargetFPS(60);
    ToggleFullscreen();

    while (!WindowShouldClose()) {
        
        handleInput(game);
        updateGame(game);
        updateCamera(game);
        renderGame(game);
        
    }

    CloseWindow();

    destroyGame(game);
    destroyScene(level_one_scene);
    return 0;
}