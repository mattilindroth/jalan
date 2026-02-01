#ifndef ENEMY_HH
#define ENEMY_HH

#include "raylib.h"
#include <stdlib.h>
#include <stdbool.h>

enum EnemyState {
    ENEMY_PATROL,
    ENEMY_INVESTIGATE,
    ENEMY_SEARCH,
    ENEMY_CHASE,
    ENEMY_RETURN,
};

typedef struct Enemy {
    int id;
    int sceneId;
    Vector2 position;
    enum EnemyState state;
    Vector2 patrolPointA;
    Vector2 patrolPointB;
    float speed;

    // Add other enemy-related properties here
} Enemy;

#endif // ENEMY_HH

//Create a new enemy object
Enemy* createEnemy(int id, int sceneId, Vector2 position, Vector2 patrolPointA, Vector2 patrolPointB, float speed, bool hasLight);

void renderEnemy(Enemy* enemy);

//Update enemy behavior based on its state
void updateEnemy(Enemy* enemy, Vector2 playerPosition);

//Destroy an enemy object
void destroyEnemy(Enemy* enemy);