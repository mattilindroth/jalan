#ifndef ENTITY_HH
#define ENTITY_HH

#include "raylib.h"
#include "sprite.h"

typedef struct ParallaxLayer ParallaxLayer;
typedef struct Entity Entity;

typedef void (*EntityCollisionCallback)(Entity *self, Entity *other);
//typedef void (*EntityMoveCallback)(Entity *self, float deltaTime);

//This needs thinking. I do not know what inputs a player might have, e.g. e to pick up stuff or
//space to jump or f to attack. think how to handle this.

typedef struct Mover {
    Vector2 position;
    Vector2 velocity;
    float speed;
} Mover;

typedef struct Collider {
    Rectangle *bounds;
    EntityCollisionCallback onCollision;
} Collider;

typedef struct Entity {
    Mover *mover;
    Collider *collider;
    //EntityMoveCallback onMove;
    Sprite *sprite;
    ParallaxLayer *parallaxLayer; // Pointer to the parallax layer this entity belongs to
} Entity;

Entity *entity_create(Sprite *sprite);

Entity *entity_destroy(Entity *entity);

void entity_update(Entity *entity, float deltaTime);

void entity_add_trait_collision(Entity *entity, EntityCollisionCallback callback);

void entity_add_trait_mover(Entity *entity, float speed);

#endif // ENTITY_HH