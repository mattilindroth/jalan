#include "game.h"

Game* createGame(Scene* initialScene, enum GameState initialState) {
    Game* game = (Game*)malloc(sizeof(Game));
    if (game != NULL) {
        game->currentScene = initialScene;
        game->state = initialState;
    }
    return game;
}

int updateGame(Game* game) {
    // Implement game update logic here
    return 0;
}

int handleInput(Game* game) {
    // Implement input handling logic here
    return 0;
}

int updateCamera(Game* game) {
    // Implement camera update logic here
    return 0;
}

int renderGame(Game* game) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Hello, World!", 190, 200, 20, LIGHTGRAY);
    EndDrawing();
    return 0;
}

void destroyGame(Game* game) {
    if (game != NULL) {
        free(game);
    }
}