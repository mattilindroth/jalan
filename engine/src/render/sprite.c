#include <stdlib.h>
#include <math.h>

#include "render/sprite.h"
#include "raylib.h"

void animation_init(Animation* anim, Rectangle* frames, int frame_count, float frame_duration) {
    if (!anim || !frames || frame_count <= 0 || frame_duration <= 0.0f) {
        return;
    }
    
    anim->frames = frames;
    anim->frame_count = frame_count;
    anim->frame_duration = frame_duration;
    anim->current_frame = 0;
    anim->timer = 0.0f;
}


Animation *animation_create(Rectangle* frames, int frame_count, float frame_duration) {
    if (!frames || frame_count <= 0 || frame_duration <= 0.0f) {
        return NULL;
    }
    
    Animation* anim = malloc(sizeof(Animation));
    if (!anim) {
        return NULL;
    }
    
    anim->frames = malloc(frame_count * sizeof(Rectangle));
    if (!anim->frames) {
        free(anim);
        return NULL;
    }
    
    for (int i = 0; i < frame_count; i++) {
        anim->frames[i] = frames[i];
    }
    
    anim->frame_count = frame_count;
    anim->frame_duration = frame_duration;
    anim->current_frame = 0;
    anim->timer = 0.0f;
    
    return anim;
}

/**
 * @brief Destroy and deallocate an Animation structure
 * @param anim Pointer to the Animation structure to destroy
 */
void animation_destroy(Animation* anim) {
    if (!anim) return;
    
    free(anim->frames);
    free(anim); 
}

Sprite *sprite_create(Vector2 position, Vector2 scale, float rotation, Animation* animation) {
    Sprite* sprite = malloc(sizeof(Sprite));
    if (!sprite) {
        return NULL;
    }
    
    sprite->position = position;
    sprite->scale = scale;
    sprite->rotation = rotation;
    sprite->flip_x = false;
    sprite->flip_y = false;
    sprite->animation = animation;
    sprite->active_animation_index = 0;
    
    return sprite;
}


void sprite_update(Sprite* sprite, float deltaTime) {
    if (!sprite || !sprite->animation) return;
    
    Animation* anim = sprite->animation;
    anim->timer += deltaTime;
    
    if (anim->timer >= anim->frame_duration) {
        anim->current_frame = (anim->current_frame + 1) % anim->frame_count;
        anim->timer = 0.0f;
    }
}


void sprite_draw(Sprite* sprite) {
if (!sprite || !sprite->animation) return;
    Animation *anim = sprite->animation;
    Rectangle source = anim->frames[anim->current_frame];
    
    // Flip horizontally by using negative width in source rectangle
    if (sprite->flip_x) {
        source.width = -source.width;
    }
    if (sprite->flip_y) {
        source.height = -source.height;
    }
    
    Rectangle dest = {
        sprite->position.x,
        sprite->position.y,
        abs(source.width) * sprite->scale.x,
        source.height * sprite->scale.y
    };
    DrawTexturePro(sprite->texture, source, dest, (Vector2){0, 0}, 0.0f, WHITE);
}


void sprite_init(Sprite* sprite, Vector2 position, Vector2 scale, float rotation, Animation* animation) {
    if (!sprite) return;
    
    sprite->position = position;
    sprite->scale = scale;
    sprite->rotation = rotation;
    sprite->flip_x = false;
    sprite->flip_y = false;
    sprite->animation = animation;
    sprite->active_animation_index = 0;
}


void sprite_destroy(Sprite* sprite) {
    if (!sprite) return;
 
    if (sprite->animation) {
        animation_destroy(sprite->animation);
    }
    
    // Note: Animation is not destroyed here; manage its lifecycle separately
    free(sprite);
}