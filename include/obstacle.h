#ifndef OBSTACLE_HH
#define OBSTACLE_HH

#include "raylib.h"

typedef struct Obstacle {
    int id;
    int sceneId;
    Rectangle rectangle;
    Color color;
    // Add other obstacle-related properties here
} Obstacle;

enum CollisionType {
    COLLISION_NONE,
    COLLISION_LEFT,
    COLLISION_RIGHT,
    COLLISION_TOP,
    COLLISION_BOTTOM
};

//Create a new obstacle object
Obstacle* createObstacle(int id, int sceneId, Rectangle rectangle, Color color);

void renderObstacle(Obstacle* obstacle);

//Check collision between a rectangle and an obstacle
enum CollisionType checkCollisionWithObstacle(Rectangle rect, Obstacle* obstacle);

//Destroy an obstacle object
void destroyObstacle(Obstacle* obstacle);

#endif // OBSTACLE_HH