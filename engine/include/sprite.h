#ifndef SPRITE_HH
#define SPRITE_HH

#include <raylib.h>
#include "dynamic_array.h"

typedef struct Animation {
    DynamicArray *frames; //Array of Rectangles
    int currentFrameIndex;
    int id;
    float frameSpeed;   // frames per second
    int currentFrame;
    float timer;
    Vector2 offset;  // Position offset for this animation
} Animation;

Animation *animation_create(int id, float frameSpeed, Vector2 offset);
void animation_add_frame(Animation *animation, Rectangle frame);
Animation *animation_destroy(Animation *animation);

typedef struct Sprite {
    Texture2D *texture;
    Vector2 position;
    Vector2 origin;
    float rotation;
    Vector2 scale;
    bool flipX;
    bool flipY;
    Color tint;
    DynamicArray *animations; // Array of Animation
    int activeAnimationId; // ID of the currently active animation
} Sprite;

Sprite *sprite_create(Texture2D* texture, Vector2 position) ;

void sprite_update(Sprite* sprite, float deltaTime);

void sprite_set_flip_x(Sprite* sprite, bool flip);

void sprite_set_flip_y(Sprite* sprite, bool flip);

void sprite_set_rotation(Sprite* sprite, float rotation);

void sprite_set_scale(Sprite* sprite, Vector2 scale);

void sprite_add_animation(Sprite* sprite, Animation *animation);

void sprite_render(Sprite *sprite);

Sprite* sprite_destroy(Sprite* sprite);

#endif // SPRITE_HH