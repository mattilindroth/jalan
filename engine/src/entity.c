#include <stdlib.h>
#include <stdio.h>
#include "entity.h"

Entity *entity_create(Sprite *sprite) {
    Entity *entity = (Entity *)malloc(sizeof(Entity));
    if (!entity) {
        fprintf(stderr, "Failed to allocate memory for Entity\n");
        return NULL;
    }
    entity->sprite = sprite;
    entity->collider = NULL;
    entity->mover = NULL;
    entity->parallaxLayer = NULL;
    return entity;
}

Entity *entity_destroy(Entity *entity) {
    if (!entity) return NULL;
    
    // Clean up traits
    if (entity->collider) {
        free(entity->collider);
    }
    if (entity->mover) {
        free(entity->mover);
    }
    
    // Note: Sprite is not owned by Entity, so we don't free it here
    
    free(entity);
    return NULL;
}

void entity_update(Entity *entity, float deltaTime) {
    if (!entity) return;
    
    // Update traits
    if (entity->mover) {
        // Example movement logic (this should be expanded based on actual game needs)
        entity->sprite->position.x += entity->mover->speed * deltaTime;
    }
    
    // Update sprite animation
    sprite_update(entity->sprite, deltaTime);
}

void entity_add_trait_collision(Entity *entity, EntityCollisionCallback callback) {
    if (!entity) return;
    
    if (!entity->collider) {
        entity->collider = (Collider *)malloc(sizeof(Collider));
        if (!entity->collider) {
            fprintf(stderr, "Failed to allocate memory for EntityCollider\n");
            return;
        }
    }
    
    entity->collider->onCollision = callback;
}

void entity_add_trait_mover(Entity *entity, float speed) {
    if (!entity) return;
    
    if (!entity->mover) {
        entity->mover = (Mover *)malloc(sizeof(Mover));
        if (!entity->mover) {
            fprintf(stderr, "Failed to allocate memory for EntityMover\n");
            return;
        }
    }
    
    entity->mover->speed = speed;
}