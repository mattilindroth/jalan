#ifndef GAME_H
#define GAME_H

#include "scene.h"
#include <stdlib.h>
#include <raylib.h>
#include "dynamic_array.h"
#include "obstacle.h"
#include "player.h"
#include "enemy.h"
#include "background_object.h"
#include "light.h"
#include "grid.h"

#define LIGHTING_MAX_RADIUS 600.0f

enum GameState {
    STATE_MENU,
    STATE_PLAYING,
    STATE_STORY,
    STATE_PAUSED,
    STATE_GAMEOVER,
    STATE_EDITOR
};

typedef struct Game {
    Scene* currentScene;
    enum GameState state;
    DynamicArray* obstacles;
    DynamicArray* backgroundObjects;
    DynamicArray* lights;
    DynamicArray* enemies;

    //Perhaps make a render- specific module with camera there later on ?
    Camera2D camera;

    //These could be some kind of entities later on.    
    Player *player;
    
    // Editor-related properties
    Grid *editorGrid;
    Obstacle *selectedObstacle;       // Currently selected obstacle
    BackgroundObject *selectedBgObj;  // Currently selected background object
    Light *selectedLight;             // Currently selected light
    Enemy *selectedEnemy;             // Currently selected enemy
    int editorMode;                   // 0=obstacles, 1=background objects, 2=lights, 3=enemies
    int currentColorIndex;            // Index for color cycling
    int dragMode;                     // 0=none, 1=move, 2=resize
    Vector2 dragStartPos;             // Mouse position when drag started
    Vector2 dragStartObjPos;          // Object position when drag started
    Vector2 dragStartObjSize;         // Object size when drag started
    // Add other game-related properties here
} Game;

Game* createGame(Scene* initialScene, enum GameState initialState);

void addObstacleToGame(Game* game, Obstacle* obstacle);

void addBackgroundObjectToGame(Game* game, BackgroundObject* backgroundObject);

void addLightToGame(Game* game, Light* light);

void addEnemyToGame(Game* game, Enemy* enemy);

int updateGame(Game* game);

int handleInput(Game* game);

int updateCamera(Game* game);

int renderGame(Game* game);

// Editor helper functions
Obstacle* findObstacleAtPosition(Game* game, Vector2 worldPos);
Obstacle* createObstacleAtPosition(Game* game, Vector2 worldPos);
BackgroundObject* findBackgroundObjectAtPosition(Game* game, Vector2 worldPos);
BackgroundObject* createBackgroundObjectAtPosition(Game* game, Vector2 worldPos);
Light* findLightAtPosition(Game* game, Vector2 worldPos);
Light* createLightAtPosition(Game* game, Vector2 worldPos);
Enemy* findEnemyAtPosition(Game* game, Vector2 worldPos);
Enemy* createEnemyAtPosition(Game* game, Vector2 worldPos);
bool isInResizeCorner(Rectangle rect, Vector2 worldPos, float cornerSize);
void removeObstacleFromGame(Game* game, Obstacle* obstacle);
void removeBackgroundObjectFromGame(Game* game, BackgroundObject* bgObj);
void removeLightFromGame(Game* game, Light* light);
void removeEnemyFromGame(Game* game, Enemy* enemy);
Color getEditorColor(int colorIndex);
int getEditorColorCount(void);

void destroyGame(Game* game);

#endif // GAME_H
