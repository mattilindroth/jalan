#include "scene.h"
#include <stdlib.h>
#include "raylib.h"
#include "dynamic_array.h"

#ifndef GAME_H
#define GAME_H

enum GameState {
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAMEOVER
};

typedef struct Game {
    Scene* currentScene;
    enum GameState state;
    dynamic_array* rainDrops;
    // Add other game-related properties here
} Game;

Game* createGame(Scene* initialScene, enum GameState initialState);

int updateGame(Game* game);

int handleInput(Game* game);

int updateCamera(Game* game);

int renderGame(Game* game);

void destroyGame(Game* game);

#endif // GAME_H
