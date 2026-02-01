#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "obstacle.h"

//Create a new obstacle object
Obstacle* createObstacle(int id, int sceneId, Rectangle rectangle, Color color) {
    Obstacle* obstacle = (Obstacle*)malloc(sizeof(Obstacle));
    if (obstacle == NULL) {
        fprintf(stderr, "Failed to allocate memory for Obstacle\n");
        return NULL;
    }
    obstacle->id = id;
    obstacle->sceneId = sceneId;
    obstacle->rectangle = rectangle;
    obstacle->color = color;
    return obstacle;
}

void renderObstacle(Obstacle* obstacle) {
    if (obstacle != NULL) {
        DrawRectangleRec(obstacle->rectangle, obstacle->color);
    }
}

//Check collision between a rectangle and an obstacle
enum CollisionType checkCollisionWithObstacle(Rectangle rect, Obstacle* obstacle) {
    if (obstacle == NULL) {
        return COLLISION_NONE;
    }

    if (CheckCollisionRecs(rect, obstacle->rectangle)) {
        float rectCenterX = rect.x + rect.width / 2;
        float rectCenterY = rect.y + rect.height / 2;
        float obsCenterX = obstacle->rectangle.x + obstacle->rectangle.width / 2;
        float obsCenterY = obstacle->rectangle.y + obstacle->rectangle.height / 2;

        float dx = rectCenterX - obsCenterX;
        float dy = rectCenterY - obsCenterY;

        float width = (rect.width + obstacle->rectangle.width) / 2;
        float height = (rect.height + obstacle->rectangle.height) / 2;

        float crossWidth = width * dy;
        float crossHeight = height * dx;

        if (fabsf(dx) <= width && fabsf(dy) <= height) {
            if (crossWidth > crossHeight) {
                return (crossWidth > -crossHeight) ? COLLISION_BOTTOM : COLLISION_LEFT;
            } else {
                return (crossWidth > -crossHeight) ? COLLISION_RIGHT : COLLISION_TOP;
            }
        }
    }
    return COLLISION_NONE;
}

//Destroy an obstacle object
void destroyObstacle(Obstacle* obstacle) {
    if (obstacle != NULL) {
        free(obstacle);
    }
}