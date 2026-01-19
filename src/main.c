#include <stdio.h>

#include "raylib.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 800
#define GRAVITY 0.5f

/*************
** Structures
**************/
enum PlayerState {
    PLAYER_IDLE,
    PLAYER_RUNNING,
    PLAYER_JUMPING,
    PLAYER_FALLING,
};

enum CollisionState {
    COLLISION_NONE,
    COLLISION_WALL,
    COLLISION_GROUND,
    COLLISION_CEILING,
};

//Structure to represent the player
typedef struct Player {
    Vector2 position;
    Vector2 speed;
    Rectangle boundingBox;
    enum PlayerState state;
    bool onGround;
} Player;

/************************
** Function declarations
*************************/

int checkCollisionWithGameArea(Rectangle *object);

int updateCamera();

int handleInput();

int updateGame();

int renderFrame();

/**************************
** Game object declarations
***************************/

Camera2D camera = {0};

Player player;

Rectangle gameArea = {0, 0, SCREEN_WIDTH * 3, SCREEN_HEIGHT * 2};

int main(int argc, char *argv[]) {
    
    // Initialize the window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Jalannallas - the final nightmare V0.1 Alpha");

    SetTargetFPS(60);
    
    player = (Player){{SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2}, {0, 0}, {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 50, 50}, PLAYER_IDLE, true};

    // Main game loop
    while (!WindowShouldClose()) {       

        handleInput();

        updateGame();
        
        updateCamera();

        //Draw
        BeginDrawing();

        ClearBackground(RAYWHITE);

        renderFrame();

        // End drawing
        EndDrawing();

    }

    // Close window and OpenGL context
    CloseWindow();

    return 0;
}

int renderFrame() {
    // Begin camera mode to apply camera transformations
    BeginMode2D(camera);
    
    //Draw the game area
    DrawRectangleRec(gameArea, LIGHTGRAY);

    //Draw the player as a red square
    DrawRectangle(player.boundingBox.x, player.boundingBox.y, player.boundingBox.width, player.boundingBox.height, RED);
    
    // End camera mode
    EndMode2D();
    
    return 0;
}

int updateCamera() {
    // Center the camera on the player
    camera.target = (Vector2){player.boundingBox.x + player.boundingBox.width / 2, player.boundingBox.y + player.boundingBox.height / 2};
    camera.offset = (Vector2){SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
    camera.zoom = 1.0f;

    // Clamp camera to keep game area visible
    float cameraLeft = camera.target.x - camera.offset.x / camera.zoom;
    float cameraRight = camera.target.x + camera.offset.x / camera.zoom;
    float cameraTop = camera.target.y - camera.offset.y / camera.zoom;
    float cameraBottom = camera.target.y + camera.offset.y / camera.zoom;

    // Clamp horizontal position
    if (cameraLeft < gameArea.x) {
        camera.target.x = gameArea.x + camera.offset.x / camera.zoom;
    }
    if (cameraRight > gameArea.x + gameArea.width) {
        camera.target.x = gameArea.x + gameArea.width - camera.offset.x / camera.zoom;
    }

    // Clamp vertical position
    if (cameraTop < gameArea.y) {
        camera.target.y = gameArea.y + camera.offset.y / camera.zoom;
    }
    if (cameraBottom > gameArea.y + gameArea.height) {
        camera.target.y = gameArea.y + gameArea.height - camera.offset.y / camera.zoom;
    }

    
    return 0;
}

int checkCollisionWithGameArea(Rectangle *object) {
    int collision = COLLISION_NONE;
    
    // Check horizontal collisions (walls)
    if (object->x < gameArea.x) {
        object->x = gameArea.x;
        collision = COLLISION_WALL;
    }
    if ((object->x + object->width) > gameArea.x + gameArea.width) {
        object->x = gameArea.x + gameArea.width - object->width;
        collision = COLLISION_WALL;
    }
    
    // Check vertical collisions
    if (object->y < gameArea.y) {
        object->y = gameArea.y;
        collision = COLLISION_CEILING;
    }
    
    // Check ground collision specifically
    if ((object->y + object->height) > gameArea.y + gameArea.height) {
        object->y = gameArea.y + gameArea.height - object->height;
        return COLLISION_GROUND;
    }
    
    return collision;
}

int updateGame() {
    // Apply gravity if not on ground
    if(!player.onGround) {
        if(player.speed.y > 10) {
            player.speed.y = 10; // Terminal velocity
        }
        player.speed.y += GRAVITY; // Apply gravity to vertical speed
    }
    
    // Update player position based on speed
    player.boundingBox.x += player.speed.x;
    player.boundingBox.y += player.speed.y;

    int collisionType = checkCollisionWithGameArea(&player.boundingBox);
    
    if(collisionType == COLLISION_GROUND) {
        // Ground collision detected - player is definitely on ground
        if(player.speed.y >= 0) { // Only stop if falling or stationary
            player.speed.y = 0;
        }
        player.onGround = true;
        if(player.state == PLAYER_JUMPING || player.state == PLAYER_FALLING) {
            player.state = PLAYER_IDLE; // Reset state to idle if landing
        }
    } else if(collisionType == COLLISION_WALL) {
        // Wall collision detected - don't affect ground state

        // Don't change onGround state for wall collisions
        if(player.speed.y > 0 && player.state != PLAYER_JUMPING) {
            player.state = PLAYER_FALLING;
        }
    } else if(collisionType == COLLISION_CEILING) {
        // Ceiling collision detected - stop upward movement
        if(player.speed.y < 0) {
            player.speed.y = 0;

    }
    } else {
        // No collision - check if we're still touching ground from previous frame
        // Only set onGround to false if we're definitely in the air
        if(player.speed.y < 0 || (player.boundingBox.y + player.boundingBox.height) < (gameArea.y + gameArea.height - 1)) {
            player.onGround = false;
        }
        if(player.speed.y > 0 && player.state != PLAYER_JUMPING) {
            player.state = PLAYER_FALLING;
        }
    }

    return 0;
}

int handleInput() {
    
    if(player.onGround) {
        if(IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
            // A key or Left arrow is being held down
            player.speed.x = -5;
            player.state = PLAYER_RUNNING;
        } else 
        if(IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
            // D key or Right arrow is being held down
            player.speed.x = 5;
            player.state = PLAYER_RUNNING;
        } else {
            player.speed.x = 0;
            player.state = PLAYER_IDLE;
        }

        // Change to IsKeyPressed for single jump input
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
            // W key or Up arrow was just pressed (single press)
            player.speed.y = -10; // Jumping action
            player.state = PLAYER_JUMPING;
            player.onGround = false; // Immediately set onGround to false
        }
    }

    if (IsKeyPressed(KEY_SPACE)) {
        // Space key was just pressed (single press)
    }

    // if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
    //     // Left mouse button is being held down
    // }

    // if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
    //     // Right mouse button was just pressed (single press)
    // }
    return 0;
}