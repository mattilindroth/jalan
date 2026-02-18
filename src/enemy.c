
#include "enemy.h"
#include <math.h>

//Create a new enemy object
Enemy* createEnemy(int id, int sceneId, Vector2 position, Vector2 patrolPointA, Vector2 patrolPointB, float speed, bool doesPatrol) {
    Enemy* enemy = (Enemy*)malloc(sizeof(Enemy));
    if (enemy != NULL) {
        enemy->id = id;
        enemy->sceneId = sceneId;
        enemy->position = position;
        enemy->state = ENEMY_PATROL;
        enemy->doesPatrol = doesPatrol;
        enemy->patrolPointA = patrolPointA;
        enemy->patrolPointB = patrolPointB;
        enemy->lastKnownPlayerPosition = position;
        enemy->searchOrigin = position;
        enemy->lookAtDirection = (Vector2){0.0f, 0.0f};
        enemy->speed = speed;
    }
    return enemy;
}

//Update enemy behavior based on its state
void updateEnemy(Enemy* enemy, Vector2 playerPosition) {
    switch (enemy->state) {
        case ENEMY_PATROL:
            if (enemy->doesPatrol) {
                // If lookAtDirection is zero, initialize it toward patrolPointB
                if (enemy->lookAtDirection.x == 0.0f && enemy->lookAtDirection.y == 0.0f) {
                    float dx = enemy->patrolPointB.x - enemy->patrolPointA.x;
                    float dy = enemy->patrolPointB.y - enemy->patrolPointA.y;
                    float len = sqrtf(dx * dx + dy * dy);
                    if (len > 0.0f) {
                        enemy->lookAtDirection.x = dx / len;
                        enemy->lookAtDirection.y = dy / len;
                    } else {
                        enemy->lookAtDirection = (Vector2){1.0f, 0.0f};
                    }
                }

                // Pick the patrol point we're heading toward (most aligned with lookAtDirection)
                float dotA = (enemy->patrolPointA.x - enemy->position.x) * enemy->lookAtDirection.x
                           + (enemy->patrolPointA.y - enemy->position.y) * enemy->lookAtDirection.y;
                float dotB = (enemy->patrolPointB.x - enemy->position.x) * enemy->lookAtDirection.x
                           + (enemy->patrolPointB.y - enemy->position.y) * enemy->lookAtDirection.y;
                Vector2 target = (dotA > dotB) ? enemy->patrolPointA : enemy->patrolPointB;

                float dx = target.x - enemy->position.x;
                float dy = target.y - enemy->position.y;
                float dist = sqrtf(dx * dx + dy * dy);

                if (dist < 4.0f) {
                    // Reached the target point — flip direction
                    enemy->lookAtDirection.x = -enemy->lookAtDirection.x;
                    enemy->lookAtDirection.y = -enemy->lookAtDirection.y;
                } else {
                    float step = enemy->speed * GetFrameTime();
                    enemy->position.x += (dx / dist) * step;
                    enemy->position.y += (dy / dist) * step;
                }
            }
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
    const float radius = 20.0f;

    // Body
    DrawCircleV(enemy->position, radius, RED);
    DrawCircleLines(enemy->position.x, enemy->position.y, radius, MAROON);

    // Face direction indicator: white eye sclera + dark pupil offset toward lookAtDirection
    if (enemy->lookAtDirection.x != 0.0f || enemy->lookAtDirection.y != 0.0f) {
        // Normalise (should already be normalised, but be safe)
        float len = sqrtf(enemy->lookAtDirection.x * enemy->lookAtDirection.x
                        + enemy->lookAtDirection.y * enemy->lookAtDirection.y);
        float nx = enemy->lookAtDirection.x / len;
        float ny = enemy->lookAtDirection.y / len;

        // Eye centre sits partway between the body centre and the edge
        Vector2 eyeCenter = {
            enemy->position.x + nx * (radius * 0.5f),
            enemy->position.y + ny * (radius * 0.5f)
        };
        // Pupil is offset a little further in the look direction
        Vector2 pupilCenter = {
            eyeCenter.x + nx * 3.0f,
            eyeCenter.y + ny * 3.0f
        };

        DrawCircleV(eyeCenter, 7.0f, WHITE);
        DrawCircleV(pupilCenter, 4.0f, BLACK);
    }
}

//Destroy an enemy object
void destroyEnemy(Enemy* enemy) {
    if (enemy != NULL) {
        free(enemy);
    }
}