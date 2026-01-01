/**
 * @file sprite.h
 * @brief Sprite and animation system for 2D graphics rendering
 */

#ifndef SPRITE_H
#define SPRITE_H

#include <stdbool.h>
#include "raylib.h"

/**
 * @struct Animation
 * @brief Represents an animated sequence of frames
 * 
 * Contains frame data and timing information for sprite animations.
 * Each animation consists of multiple rectangular frames that cycle
 * through at a specified duration per frame.
 */
typedef struct {
    Rectangle* frames;
    int frame_count;
    float frame_duration; // Duration of each frame in seconds
    int current_frame ;
    float timer ;
} Animation;


/**
 * @brief Initialize an existing Animation structure
 * @param anim Pointer to the Animation structure to initialize
 * @param frames Array of Rectangle structures defining frame boundaries
 * @param frame_count Number of frames in the animation
 * @param frame_duration Duration of each frame in seconds
 */
void animation_init(Animation* anim, Rectangle* frames, int frame_count, float frame_duration);

/**
 * @brief Create and allocate a new Animation structure
 * @param frames Array of Rectangle structures defining frame boundaries
 * @param frame_count Number of frames in the animation
 * @param frame_duration Duration of each frame in seconds
 * @return Pointer to newly allocated Animation structure, or NULL on failure
 */
Animation *animation_create(Rectangle* frames, int frame_count, float frame_duration);

/**
 * @brief Destroy and deallocate an Animation structure
 * @param anim Pointer to the Animation structure to destroy
 */
void animation_destroy(Animation* anim);

/**
 * @struct Sprite
 * @brief Represents a 2D sprite with position, transformation, and animation
 * 
 * Contains all necessary data for rendering a 2D sprite including
 * position, scale, rotation, and associated animation data.
 */
typedef struct {
    Vector2 position;
    Vector2 scale;
    float rotation;
    bool flip_x;
    bool flip_y;
    Texture texture;
    Animation* animation;
    int active_animation_index;
} Sprite;

/**
 * @brief Create and allocate a new Sprite structure
 * @param position Initial position of the sprite
 * @param scale Scaling factors for the sprite
 * @param rotation Initial rotation angle of the sprite in degrees
 * @param animation Pointer to the Animation structure associated with the sprite
 * @return Pointer to newly allocated Sprite structure, or NULL on failure
 */
Sprite *sprite_create(Vector2 position, Vector2 scale, float rotation, Animation* animation);

/**
 * @brief Update the sprite's animation based on elapsed time
 * @param sprite Pointer to the Sprite structure to update
 * @param deltaTime Time elapsed since the last update in seconds
 */
void sprite_update(Sprite* sprite, float deltaTime);

/**
 * @brief Draw the sprite to the screen
 * @param sprite Pointer to the Sprite structure to draw
 */
void sprite_draw(Sprite* sprite);

/**
 * @brief Initialize an existing Sprite structure
 * @param sprite Pointer to the Sprite structure to initialize
 * @param position Initial position of the sprite
 * @param scale Scaling factors for the sprite
 * @param rotation Initial rotation angle of the sprite in degrees
 * @param animation Pointer to the Animation structure associated with the sprite
 */
void sprite_init(Sprite* sprite, Vector2 position, Vector2 scale, float rotation, Animation* animation);

/**
 * @brief Destroy and deallocate a Sprite structure
 * @param sprite Pointer to the Sprite structure to destroy
 */
void sprite_destroy(Sprite* sprite);

#endif // SPRITE_H