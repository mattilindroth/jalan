#ifndef GAME_H
#define GAME_H

#include "scene.h"
#include <stdlib.h>
#include "raylib.h"
#include "dynamic_array.h"
#include "obstacle.h"
#include "player.h"
#include "enemy.h"
#include "background_object.h"

enum GameState {
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAMEOVER
};

typedef struct Game {
    Scene* currentScene;
    enum GameState state;
    DynamicArray* obstacles;
    DynamicArray* backgroundObjects;

    //Perhaps make a render- specific module with camera there later on ?
    Camera2D camera;

    //These could be some kind of entities later on.    
    Player *player;
    Enemy *enemy;
    // Add other game-related properties here
} Game;

Game* createGame(Scene* initialScene, enum GameState initialState);

void addObstacleToGame(Game* game, Obstacle* obstacle);

void addBackgroundObjectToGame(Game* game, BackgroundObject* backgroundObject);

int updateGame(Game* game);

int handleInput(Game* game);

int updateCamera(Game* game);

int renderGame(Game* game);

void destroyGame(Game* game);

#endif // GAME_H
