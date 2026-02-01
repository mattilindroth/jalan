
#include "enemy.h"

//Create a new enemy object
Enemy* createEnemy(int id, int sceneId, Vector2 position, Vector2 patrolPointA, Vector2 patrolPointB, float speed, bool hasLight) {
    Enemy* enemy = (Enemy*)malloc(sizeof(Enemy));
    if (enemy != NULL) {
        enemy->id = id;
        enemy->sceneId = sceneId;
        enemy->position = position;
        enemy->state = ENEMY_PATROL;
        enemy->patrolPointA = patrolPointA;
        enemy->patrolPointB = patrolPointB;
        enemy->speed = speed;
    }
    return enemy;
}

//Update enemy behavior based on its state
void updateEnemy(Enemy* enemy, Vector2 playerPosition) {
    switch (enemy->state) {
        case ENEMY_PATROL:
            // Implement patrol logic here
            break;
        case ENEMY_INVESTIGATE:
            // Implement investigate logic here
            break;
        case ENEMY_SEARCH:
            // Implement search logic here
            break;
        case ENEMY_CHASE:
            // Implement chase logic here
            break;
        case ENEMY_RETURN:
            // Implement return logic here
            break;
        default:
            break;
    }
}

void renderEnemy(Enemy* enemy) {
    // Placeholder rendering logic
    DrawCircleV(enemy->position, 20.0f, RED);
}

//Destroy an enemy object
void destroyEnemy(Enemy* enemy) {
    if (enemy != NULL) {
        free(enemy);
    }
}