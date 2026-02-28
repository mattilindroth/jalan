#include "sprite.h"
#include "dynamic_array.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

Animation *animation_create(int id, float frameSpeed, Vector2 offset) {
    Animation *animation = (Animation *)malloc(sizeof(Animation));
    if (!animation) {
        fprintf(stderr, "Failed to allocate memory for Animation\n");
        return NULL;
    }
    animation->id = id;
    animation->frameSpeed = frameSpeed;
    animation->currentFrameIndex = 0;
    animation->timer = 0.0f;
    animation->offset = offset;
    animation->frames = dynamic_array_create_default();
    return animation;
}

void animation_add_frame(Animation *animation, Rectangle frame) {
    if (!animation) return;
    dynamic_array_push(animation->frames, &frame);
}

Animation *animation_destroy(Animation *animation) {
    if (!animation) return NULL;
    dynamic_array_destroy(animation->frames);
    free(animation);
    return NULL;
}

Sprite *sprite_create(Texture2D* texture, Vector2 position) {
    Sprite *sprite = (Sprite *)malloc(sizeof(Sprite));
    if (!sprite) {
        fprintf(stderr, "Failed to allocate memory for Sprite\n");
        return NULL;
    }
    sprite->texture = texture;
    sprite->position = position;
    sprite->origin = (Vector2){0, 0};
    sprite->rotation = 0.0f;
    sprite->scale = (Vector2){1.0f, 1.0f};
    sprite->flipX = false;
    sprite->flipY = false;
    sprite->tint = WHITE;
    sprite->animations = dynamic_array_create_default();
    sprite->activeAnimationId = -1; // No active animation by default
    return sprite;
}

void sprite_update(Sprite* sprite, float deltaTime) {
    if (!sprite) return;
    
    // Update active animation frame
    Animation *anim = NULL;
    if (sprite->activeAnimationId >= 0) {
        for(int index = 0; index < dynamic_array_size(sprite->animations); index++) {
            anim = (Animation*)dynamic_array_get(sprite->animations, index);
            if(anim->id == sprite->activeAnimationId) {
                break;
            }
        }
        if (anim && dynamic_array_size(anim->frames) > 0) {
            anim->timer += deltaTime;
            int frameCount = (int)dynamic_array_size(anim->frames);
            int frameDuration = (int)(1000.0f / anim->frameSpeed); // Duration of each frame in ms
            
            if (anim->timer >= frameDuration) {
                anim->currentFrameIndex = (anim->currentFrameIndex + 1) % frameCount;
                anim->timer = 0.0f;
            }
        }
    }
}

void sprite_set_flip_x(Sprite* sprite, bool flip) {
    if (!sprite) return;
    sprite->flipX = flip;
}

void sprite_set_flip_y(Sprite* sprite, bool flip) {
    if (!sprite) return;
    sprite->flipY = flip;
}

void sprite_set_rotation(Sprite* sprite, float rotation) {
    if (!sprite) return;
    sprite->rotation = rotation;
}

void sprite_set_scale(Sprite* sprite, Vector2 scale) {
    if (!sprite) return;
    sprite->scale = scale;
}

void sprite_add_animation(Sprite* sprite, Animation *animation) {
    if (!sprite || !animation) return;
    dynamic_array_push(sprite->animations, animation);
}

void sprite_render(Sprite *sprite) {
    if (!sprite) return;
    
    // Determine source rectangle based on active animation frame
    Rectangle sourceRect = {0};
    Animation *anim = NULL;
    if (sprite->activeAnimationId >= 0) {
        //Find the right animation
        for(int index = 0; index < dynamic_array_size(sprite->animations); index++) {
            anim = (Animation*)dynamic_array_get(sprite->animations, index);
            if(anim->id == sprite->activeAnimationId) {
                break;
            }
        }
        
        if (anim && dynamic_array_size(anim->frames) > 0) {
            sourceRect = *(Rectangle*)dynamic_array_get(anim->frames, anim->currentFrameIndex);
        }
    } else {
        // If no animation, use entire texture
        sourceRect = (Rectangle){0, 0, sprite->texture->width, sprite->texture->height};
    }
    
    // Calculate destination rectangle
    Rectangle destRect = {
        sprite->position.x,
        sprite->position.y,
        sourceRect.width * sprite->scale.x,
        sourceRect.height * sprite->scale.y
    };
    
    // Use the sprite's own origin (defaults to {0,0}, i.e. top-left)
    Vector2 origin = sprite->origin;

    // Apply flipping by negating the source rect dimensions
    if (sprite->flipX) sourceRect.width  *= -1;
    if (sprite->flipY) sourceRect.height *= -1;
    
    DrawTexturePro(*sprite->texture, sourceRect, destRect, origin, sprite->rotation, sprite->tint);
}

Sprite* sprite_destroy(Sprite* sprite) {
    if (!sprite) return NULL;
    for(size_t i = 0; i < dynamic_array_size(sprite->animations); i++) {
        Animation *anim = (Animation *)dynamic_array_get(sprite->animations, i);
        animation_destroy(anim);
    }
    dynamic_array_destroy(sprite->animations);
    free(sprite);
    return NULL;
}