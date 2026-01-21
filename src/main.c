#include <stdio.h>
#include <math.h>
#include "raylib.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 800
#define GAME_AREA_WIDTH (SCREEN_WIDTH * 3)
#define GAME_AREA_HEIGHT (SCREEN_HEIGHT * 2)
#define GRAVITY 0.5f

/*************
** Structures
**************/
enum PlayerState {
    PLAYER_IDLE,
    PLAYER_RUNNING,
    PLAYER_JUMPING,
    PLAYER_FALLING,
    PLAYER_HIDING,
    PLAYER_LIFTING,
};

enum CollisionState {
    COLLISION_NONE,
    COLLISION_WALL,
    COLLISION_GROUND,
    COLLISION_CEILING,
};

enum EnemyType {
    ENEMY_TYPE_GHOST,
    ENEMY_TYPE_ZOMBIE,
    ENEMY_TYPE_SKELETON,
};

typedef struct Block {
    Rectangle boundingBox;
    Vector2 speed;
} Block;

enum BackgroundItemType {
    BACKGROUND_TREE,
    BACKGROUND_GRAVE,
};

typedef struct BackgroundTree {
    Rectangle boundingBox;
    enum BackgroundItemType type;
} BackgroundTree;

typedef struct BackgroundGrave {
    Rectangle boundingBox;
    enum BackgroundItemType type;
} BackgroundGrave;

typedef struct BackgroundItem {
    Rectangle boundingBox;
    enum BackgroundItemType type;
} BackgroundItem;

//Structure to represent the player
typedef struct Player {
    Vector2 position;
    Vector2 speed;
    Rectangle boundingBox;
    enum PlayerState state;
    bool onGround;
    bool canHide;
    bool canLift;
    bool isLifting;
} Player;

//Structure to represent an enemy
typedef struct Enemy {
    Vector2 position;
    Vector2 speed;
    Rectangle boundingBox;
    enum EnemyType type;
    bool onGround;
    bool isEliminated;
    bool isFlattened; // For skeleton - when stomped
    float flattenedTimer; // Timer for skeleton respawn
} Enemy;

/************************
** Function declarations
*************************/

int checkCollisionWithGameArea(Rectangle *object);

int checkPlayerCollisionWithEnemy(Player *player, Enemy *enemy);

int updateCamera();

int handleInput();

int updateGame();

int renderFrame();

/**************************
** Game object declarations
***************************/

Camera2D camera = {0};

Player player;

Rectangle gameArea = {0, 0, GAME_AREA_WIDTH, GAME_AREA_HEIGHT};

Rectangle waterArea = {0};

Rectangle exitGateway = {0};

bool gameWon = false;

BackgroundTree trees[10] = {0};

BackgroundGrave graves[10] = {0};

Enemy ghost;

Enemy zombie;

Enemy skeleton;

Block blocks[5] = {0};

Block *liftableBlock = NULL;

float jumpCooldown = 0.0f;

int main(int argc, char *argv[]) {
    
    // Initialize the window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Jalannallas - the final nightmare V0.1 Alpha");

    SetTargetFPS(60);
    //Initialize player. he/she is on ground at start and not hiding.
    player = (Player){{2, SCREEN_HEIGHT * 2}, {0, 0}, {2, SCREEN_HEIGHT * 2, 50, 50}, PLAYER_IDLE, true, false, false, false};

    ghost = (Enemy){{GAME_AREA_WIDTH - 100, GAME_AREA_HEIGHT - 100}, {-7, 0}, {GAME_AREA_WIDTH - 100, GAME_AREA_HEIGHT - 100, 40, 60}, ENEMY_TYPE_GHOST, false, false, false, 0};

    zombie = (Enemy){{GAME_AREA_WIDTH - 300, GAME_AREA_HEIGHT - 70}, {-3, 0}, {GAME_AREA_WIDTH - 300, GAME_AREA_HEIGHT - 70, 40, 60}, ENEMY_TYPE_ZOMBIE, false, false, false, 0};

    skeleton = (Enemy){{GAME_AREA_WIDTH - 500, GAME_AREA_HEIGHT - 60}, {-2, 0}, {GAME_AREA_WIDTH - 500, GAME_AREA_HEIGHT - 60, 40, 60}, ENEMY_TYPE_SKELETON, false, false, false, 0};

    //Initialize exit gateway at far right, requiring 4+ boxes to reach (4*50=200 pixels high)
    exitGateway = (Rectangle){GAME_AREA_WIDTH - 80, GAME_AREA_HEIGHT - 300, 60, 80};

    //Initialize water area in the middle of the map (300px wide, at ground level)
    waterArea = (Rectangle){(GAME_AREA_WIDTH - 300) / 2, GAME_AREA_HEIGHT - 20, 300, 20};

    //Initialize blocks (5 cube-like blocks, spaced apart to avoid overlapping and water)
    for(int i = 0; i < 5; i++) {
        int x = (GAME_AREA_WIDTH / 6) * (i + 1) - 25; // Space them evenly across the game area
        // If block would be in water area, move it to the left
        if (x > waterArea.x - 60 && x < waterArea.x + waterArea.width + 10) {
            x = waterArea.x - 80; // Place it to the left of water area
        }
        blocks[i].boundingBox = (Rectangle){x, GAME_AREA_HEIGHT - 50, 50, 50}; // 50x50 cube-like blocks
        blocks[i].speed = (Vector2){0, 0};
    }

    //Initiate trees (avoid water area)
    for(int i = 0; i < 10; i++) {
        int x;
        int attempts = 0;
        do {
            x = GetRandomValue(0, GAME_AREA_WIDTH - 50);
            attempts++;
        } while ((x > waterArea.x - 60 && x < waterArea.x + waterArea.width + 10) && attempts < 10);
        
        // If we couldn't find a good position, place it far from water
        if (attempts >= 10) {
            x = (i < 5) ? GetRandomValue(0, waterArea.x - 100) : GetRandomValue(waterArea.x + waterArea.width + 50, GAME_AREA_WIDTH - 50);
        }
        
        trees[i].boundingBox = (Rectangle){x, GAME_AREA_HEIGHT - 150, 50, 150};
        trees[i].type = BACKGROUND_TREE;
    }

    //Initiate graves (avoid water area)
    for(int i = 0; i < 10; i++) {
        int x;
        int attempts = 0;
        do {
            x = GetRandomValue(0, GAME_AREA_WIDTH - 40);
            attempts++;
        } while ((x > waterArea.x - 50 && x < waterArea.x + waterArea.width + 10) && attempts < 10);
        
        // If we couldn't find a good position, place it far from water
        if (attempts >= 10) {
            x = (i < 5) ? GetRandomValue(0, waterArea.x - 80) : GetRandomValue(waterArea.x + waterArea.width + 40, GAME_AREA_WIDTH - 40);
        }
        
        graves[i].boundingBox = (Rectangle){x, GAME_AREA_HEIGHT - 30, 40, 30};
        graves[i].type = BACKGROUND_GRAVE;
    }

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

int renderTreesAndGraves() {
    // Draw trees
    for(int i = 0; i < 10; i++) {
        DrawRectangleRec(trees[i].boundingBox, DARKGREEN);
        DrawCircle(trees[i].boundingBox.x + trees[i].boundingBox.width / 2, trees[i].boundingBox.y, 40, DARKGREEN);
    }

    // Draw graves
    for(int i = 0; i < 10; i++) {
        DrawRectangleRec(graves[i].boundingBox, GRAY);
        DrawText("RIP", graves[i].boundingBox.x + 5, graves[i].boundingBox.y + 5, 10, BLACK);
    }

    return 0;
}

int renderFrame() {
    // Begin camera mode to apply camera transformations
    BeginMode2D(camera);
    
    //Draw the game area (keep original gray background)
    DrawRectangleRec(gameArea, LIGHTGRAY);
    
    //Draw brown underground area below the game area
    Rectangle undergroundArea = {gameArea.x, gameArea.y + gameArea.height, gameArea.width, 50};
    DrawRectangleRec(undergroundArea, BROWN);

    if(player.state == PLAYER_HIDING) {
        //Draw the player as a red square. Draw player before trees and graves to appear behind
        DrawRectangle(player.boundingBox.x, player.boundingBox.y, player.boundingBox.width, player.boundingBox.height, RED);

        renderTreesAndGraves();
    } else {
        
        renderTreesAndGraves();

        //Draw the player as a red square. Draw player after trees and graves to appear in front
        DrawRectangle(player.boundingBox.x, player.boundingBox.y, player.boundingBox.width, player.boundingBox.height, RED);
    }

    //Draw water area
    DrawRectangleRec(waterArea, BLUE);

    
    
    //Draw the ghost as a blue square
    DrawRectangle(ghost.boundingBox.x, ghost.boundingBox.y, ghost.boundingBox.width, ghost.boundingBox.height, BLUE);

    //Draw the zombie as a grayish green square
    DrawRectangle(zombie.boundingBox.x, zombie.boundingBox.y, zombie.boundingBox.width, zombie.boundingBox.height, GREEN);

    //Draw the skeleton as a white square
    if(skeleton.isFlattened) {
        // Draw skeleton as flat white rectangle when stomped
        DrawRectangle(skeleton.boundingBox.x, skeleton.boundingBox.y + 50, skeleton.boundingBox.width, 10, WHITE);
    } else {
        // Draw skeleton as normal white square
        DrawRectangle(skeleton.boundingBox.x, skeleton.boundingBox.y, skeleton.boundingBox.width, skeleton.boundingBox.height, WHITE);
    }

    //Draw blocks
    for(int i = 0; i < 5; i++) {
        DrawRectangleRec(blocks[i].boundingBox, BROWN);
        DrawText("BOX", blocks[i].boundingBox.x + 10, blocks[i].boundingBox.y + 20, 10, BLACK);
    }

    // Draw exit gateway
    DrawEllipse(exitGateway.x + exitGateway.width/2, exitGateway.y + exitGateway.height/2, 
                exitGateway.width/2, exitGateway.height/2, BLACK);
    DrawText("EXIT", exitGateway.x + 15, exitGateway.y + 35, 12, WHITE);   

    // End camera mode
    EndMode2D();
    
    // Draw win message if game is won
    if(gameWon) {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 180}); // Semi-transparent overlay
        DrawText("CONGRATULATIONS!", SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 - 60, 30, GREEN);
        DrawText("YOU ESCAPED!", SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 - 20, 24, GREEN);
        DrawText("Press ESC to quit", SCREEN_WIDTH/2 - 70, SCREEN_HEIGHT/2 + 20, 16, WHITE);
    }
    
    return 0;
}

int updateCamera() {
    // Center the camera on the player, but move it 20 pixels down to simulate underground view
    camera.target = (Vector2){player.boundingBox.x + player.boundingBox.width / 2, player.boundingBox.y + player.boundingBox.height / 2 + 20};
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

    // Clamp vertical position (allow 20 pixels underground view)
    if (cameraTop < gameArea.y) {
        camera.target.y = gameArea.y + camera.offset.y / camera.zoom;
    }
    if (cameraBottom > gameArea.y + gameArea.height + 20) {
        camera.target.y = gameArea.y + gameArea.height + 20 - camera.offset.y / camera.zoom;
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

int checkPlayerCollisionWithEnemy(Player *player, Enemy *enemy) {
    if (CheckCollisionRecs(player->boundingBox, enemy->boundingBox)) {
        // Check if player is coming from above
        // Player's bottom edge should be above or at the enemy's top edge from previous frame
        float playerBottom = player->boundingBox.y + player->boundingBox.height;
        float enemyTop = enemy->boundingBox.y;
        
        // If player is falling (positive y speed) and landed on top of enemy
        if (player->speed.y > 0 && playerBottom <= enemyTop + 10) {
            return COLLISION_CEILING; // Player jumped on enemy
        } else {
            return COLLISION_WALL; // Side collision
        }
    }
    return COLLISION_NONE; // No collision
}

int updateGame() {

    //Move ghost
    ghost.boundingBox.x += ghost.speed.x;
    //ghost.boundingBox.y += ghost.speed.y;

    ghost.boundingBox.y = ghost.position.y + (40 * sin(ghost.boundingBox.x / 90)) ; // Slight vertical movement for floating effect

    if(ghost.boundingBox.x < gameArea.x || (ghost.boundingBox.x + ghost.boundingBox.width) > (gameArea.x + gameArea.width)) {
        ghost.speed.x *= -1; // Reverse direction on wall collision
    }

    //Move zombie
    if(zombie.isEliminated) {
        zombie.boundingBox.x = zombie.position.x; // Return the zombie to original start place
        zombie.boundingBox.y = zombie.position.y;
    }
    zombie.boundingBox.x += zombie.speed.x;
    
    //zombie.boundingBox.y += zombie.speed.y;
    if(zombie.boundingBox.x < gameArea.x || (zombie.boundingBox.x + zombie.boundingBox.width) > (gameArea.x + gameArea.width)) {
        zombie.speed.x *= -1; // Reverse direction on wall collision
    }

    //Move skeleton
    if(skeleton.isFlattened) {
        // Skeleton is flattened, count down timer
        skeleton.flattenedTimer -= GetFrameTime();
        if(skeleton.flattenedTimer <= 0) {
            // Respawn skeleton
            skeleton.isFlattened = false;
            skeleton.boundingBox.height = 60; // Restore normal height
        }
    } else {
        // Normal skeleton movement
        skeleton.boundingBox.x += skeleton.speed.x;
        if(skeleton.boundingBox.x < gameArea.x || (skeleton.boundingBox.x + skeleton.boundingBox.width) > (gameArea.x + gameArea.width)) {
            skeleton.speed.x *= -1; // Reverse direction on wall collision
        }
    }

    //Apply gravity to boxes
    for(int i = 0; i < 5; i++) {
        if(!(&blocks[i] == liftableBlock && player.isLifting)) { // Don't apply gravity to lifted block
            // Apply gravity like the player
            if((blocks[i].boundingBox.y + blocks[i].boundingBox.height) < (gameArea.y + gameArea.height)) {
                if(blocks[i].speed.y > 10) {
                    blocks[i].speed.y = 10; // Terminal velocity
                }
                blocks[i].speed.y += GRAVITY;
                
                // Check for collision with other blocks (stacking)
                bool hitBlock = false;
                for(int j = 0; j < 5; j++) {
                    if(i != j && !(&blocks[j] == liftableBlock && player.isLifting)) {
                        Rectangle nextPos = blocks[i].boundingBox;
                        nextPos.y += blocks[i].speed.y;
                        if(CheckCollisionRecs(nextPos, blocks[j].boundingBox)) {
                            // Land on top of another block
                            blocks[i].boundingBox.y = blocks[j].boundingBox.y - blocks[i].boundingBox.height;
                            blocks[i].speed.y = 0;
                            hitBlock = true;
                            break;
                        }
                    }
                }
                
                if(!hitBlock) {
                    blocks[i].boundingBox.y += blocks[i].speed.y;
                }
            } else {
                // Block is on ground, stop falling
                blocks[i].speed.y = 0;
                blocks[i].boundingBox.y = gameArea.y + gameArea.height - blocks[i].boundingBox.height;
            }
        } else {
            // Reset velocity for lifted blocks
            blocks[i].speed.y = 0;
        }
    }

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

    if(player.state == PLAYER_LIFTING && liftableBlock != NULL) {
        // Move the block with the player
        liftableBlock->boundingBox.x = player.boundingBox.x + (player.boundingBox.width - liftableBlock->boundingBox.width) / 2;
        liftableBlock->boundingBox.y = player.boundingBox.y - liftableBlock->boundingBox.height; // Position block above player
    }

    if(player.state != PLAYER_HIDING) { 
        if (checkPlayerCollisionWithEnemy(&player, &ghost)) {
            // Simple response: reset player position to start
            player.boundingBox.x = SCREEN_WIDTH / 2;
            player.boundingBox.y = SCREEN_HEIGHT * 2;
            player.speed.y = 0;
            player.onGround = true;
            player.state = PLAYER_IDLE;
        }
        enum CollisionState collisionWithZombie = checkPlayerCollisionWithEnemy(&player, &zombie);
        switch (collisionWithZombie) {
            case COLLISION_CEILING:
                // Player jumped on zombie - eliminate zombie
                // For simplicity, just move zombie off-screen
                zombie.boundingBox.x = zombie.position.x;
                zombie.boundingBox.y = zombie.position.y;
                //zombie.isEliminated = true;

                //Make the player bounce up a bit
                player.speed.y = -8;
                player.onGround = false;
                break;
            case COLLISION_WALL:
                // Simple response: reset player position to start
                player.boundingBox.x = SCREEN_WIDTH / 2;
                player.boundingBox.y = SCREEN_HEIGHT * 2;
                player.speed.y = 0;
                player.onGround = true;
                player.state = PLAYER_IDLE;
                break;
            default:
                break;
        }
        
        // Check collision with skeleton
        if(!skeleton.isFlattened) { // Only check collision if skeleton is not flattened
            enum CollisionState collisionWithSkeleton = checkPlayerCollisionWithEnemy(&player, &skeleton);
            switch (collisionWithSkeleton) {
                case COLLISION_CEILING:
                    // Player jumped on skeleton - flatten it
                    skeleton.isFlattened = true;
                    skeleton.flattenedTimer = 3.0f; // 3 seconds
                    skeleton.boundingBox.height = 10; // Make it flat
                    
                    //Make the player bounce up a bit
                    player.speed.y = -8;
                    player.onGround = false;
                    break;
                case COLLISION_WALL:
                    // Simple response: reset player position to start
                    player.boundingBox.x = SCREEN_WIDTH / 2;
                    player.boundingBox.y = SCREEN_HEIGHT * 2;
                    player.speed.y = 0;
                    player.onGround = true;
                    player.state = PLAYER_IDLE;
                    break;
                default:
                    break;
            }
        }
    }

    // Check collision with blocks (player can stand on them)
    bool playerOnBlock = false;
    if(player.speed.y >= 0) { // Only check when falling or stationary
        for(int i = 0; i < 5; i++) {
            // Allow collision with all blocks, but don't position lifted blocks
            Rectangle nextPlayerPos = player.boundingBox;
            nextPlayerPos.y += player.speed.y;
            
            if(CheckCollisionRecs(nextPlayerPos, blocks[i].boundingBox)) {
                // Check if player is coming from above
                float playerBottom = player.boundingBox.y + player.boundingBox.height;
                float blockTop = blocks[i].boundingBox.y;
                
                if(playerBottom <= blockTop + 5) { // Allow small overlap for landing
                    // Player landed on top of block (but don't move lifted blocks)
                    if(&blocks[i] != liftableBlock || !player.isLifting) {
                        player.boundingBox.y = blocks[i].boundingBox.y - player.boundingBox.height;
                    }
                    player.speed.y = 0;
                    player.onGround = true;
                    playerOnBlock = true;
                    
                    if(player.state == PLAYER_JUMPING || player.state == PLAYER_FALLING) {
                        // Only change to IDLE if not lifting
                        if(!player.isLifting) {
                            player.state = PLAYER_IDLE;
                        } else {
                            player.state = PLAYER_LIFTING;
                        }
                    }
                    break;
                }
            }
        }
    }

    // Check if player reached the exit
    if(CheckCollisionRecs(player.boundingBox, exitGateway)) {
        gameWon = true;
    }

    int collisionType = checkCollisionWithGameArea(&player.boundingBox);
    
    if(collisionType == COLLISION_GROUND && !playerOnBlock) {
        // Ground collision detected - player is definitely on ground
        if(player.speed.y >= 0) { // Only stop if falling or stationary
            player.speed.y = 0;
        }
        player.onGround = true;
        if(player.state == PLAYER_JUMPING || player.state == PLAYER_FALLING) {
            // Only change to IDLE if not lifting
            if(!player.isLifting) {
                player.state = PLAYER_IDLE;
            } else {
                player.state = PLAYER_LIFTING;
            }
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
        // No ground collision - check if we're still touching ground or blocks
        if(!playerOnBlock) {
            // Only set onGround to false if we're definitely in the air and not on a block
            if(player.speed.y < 0 || (player.boundingBox.y + player.boundingBox.height) < (gameArea.y + gameArea.height - 1)) {
                player.onGround = false;
            }
            if(player.speed.y > 0 && player.state != PLAYER_JUMPING) {
                player.state = PLAYER_FALLING;
            }
        }
    }

    //Check if there is a block near the player to allow lifting
    // Safety check: if player.isLifting is true but liftableBlock is NULL, reset the lifting state
    if(player.isLifting && liftableBlock == NULL) {
        player.isLifting = false;
    }
    
    if(player.isLifting) {
        // Player is already lifting - keep current liftableBlock and don't change canLift
        player.canLift = false; // Can't lift another while already lifting
    } else {
        // Player is not lifting - check for new blocks to lift
        player.canLift = false; // Reset canLift first
        liftableBlock = NULL;
        if(player.state != PLAYER_HIDING) {
            for(int i = 0; i < 5; i++) {
                // Check if player is touching the block (either overlapping or standing on top)
                bool touching = CheckCollisionRecs(player.boundingBox, blocks[i].boundingBox);
                
                // Also check if player is standing on top of the block
                if(!touching) {
                    float playerBottom = player.boundingBox.y + player.boundingBox.height;
                    float blockTop = blocks[i].boundingBox.y;
                    float playerLeft = player.boundingBox.x;
                    float playerRight = player.boundingBox.x + player.boundingBox.width;
                    float blockLeft = blocks[i].boundingBox.x;
                    float blockRight = blocks[i].boundingBox.x + blocks[i].boundingBox.width;
                    
                    // Check if player is standing on this block (within 5 pixels and horizontally overlapping)
                    if(playerBottom >= blockTop && playerBottom <= blockTop + 5 && 
                       playerRight > blockLeft && playerLeft < blockRight) {
                        touching = true;
                    }
                }
                
                if(touching) {
                    player.canLift = true;
                    liftableBlock = &blocks[i];
                    break;
                }
            }
        } else {
            liftableBlock = NULL; // Can't lift while hiding
            player.canLift = false;
        }
    }

    //Check if there is a tree or grave near the player to allow hiding
    if(player.state != PLAYER_HIDING) {
        player.canHide = false; // Reset canHide first
        for(int i = 0; i < 10; i++) {
            if(CheckCollisionRecs(player.boundingBox, trees[i].boundingBox) || CheckCollisionRecs(player.boundingBox, graves[i].boundingBox)) {
                player.canHide = true;
                break;
            }
        }
    } else {
        player.canHide = false; // Can't hide while already hiding
    }

    return 0;
}

int handleInput() {
    
    // If game is won, only allow ESC to quit
    if(gameWon) {
        if(IsKeyPressed(KEY_ESCAPE)) {
            // Exit the game
            CloseWindow();
        }
        return 0;
    }
    
    // If player is hiding, don't allow movement - only allow unhiding with SPACE
    if(player.state == PLAYER_HIDING) {
        if (IsKeyPressed(KEY_SPACE)) {
            player.state = PLAYER_IDLE;
        }
        return 0; // Exit early to prevent movement while hiding
    }
    
    // Handle jumping FIRST, before movement, to ensure it's not affected by position changes
    // Update jump cooldown
    if(jumpCooldown > 0) {
        jumpCooldown -= GetFrameTime();
    }
    
    if ((IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) && jumpCooldown <= 0) {
        // Debug output for jump attempts
        printf("Jump attempt - onGround: %s, isLifting: %s, state: %d\n", 
               player.onGround ? "true" : "false", 
               player.isLifting ? "true" : "false", 
               player.state);
        
        if(player.onGround || player.state == PLAYER_LIFTING) {
            // W key or Up arrow is being held down
            player.speed.y = -10; // Jumping action
            if(player.state != PLAYER_LIFTING) {
                player.state = PLAYER_JUMPING;
            }
            player.onGround = false; // Immediately set onGround to false
            jumpCooldown = 0.3f; // 300ms cooldown to prevent continuous jumping
            printf("Jump executed!\n");
        } else {
            printf("Jump blocked - not on ground and not lifting\n");
        }
    }
    
    // Handle movement input after jump
    if(player.onGround || player.state == PLAYER_LIFTING) {
        // Check if player is in water for slower movement
        bool playerInWater = CheckCollisionRecs(player.boundingBox, waterArea);
        float moveSpeed = playerInWater ? 2.5f : 5.0f; // Slower movement in water
        
        if(IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
            // A key or Left arrow is being held down
            player.speed.x = -moveSpeed;
            if(player.state != PLAYER_LIFTING) {
                player.state = PLAYER_RUNNING;
            }
        } else 
        if(IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
            // D key or Right arrow is being held down
            player.speed.x = moveSpeed;
            if(player.state != PLAYER_LIFTING) {
                player.state = PLAYER_RUNNING;
            }
        } else {
            player.speed.x = 0;
            if(player.state != PLAYER_LIFTING) {
                player.state = PLAYER_IDLE;
            }
        }

    }

    if (IsKeyPressed(KEY_SPACE)) {
        if(player.state != PLAYER_HIDING && player.canHide) {
            player.state = PLAYER_HIDING;
            player.speed.x = 0; // Stop horizontal movement when hiding
            // Clear lifting state when hiding
            if(player.isLifting && liftableBlock != NULL) {
                liftableBlock->speed.y = -3; // Release with upward velocity
            }
            player.isLifting = false;
            liftableBlock = NULL;
        } else if(player.state == PLAYER_HIDING) {
            player.state = PLAYER_IDLE;
        }
    }

    if(IsKeyDown(KEY_LEFT_CONTROL)) {
        // Control key is down
        // Implement lift action here if needed
        if(player.canLift) {
            // Lift action code
            player.state = PLAYER_LIFTING;
            player.isLifting = true;
        }
    } else if(player.state == PLAYER_LIFTING) {
        // Release the block when control key is released
        if(liftableBlock != NULL) {
            // Give the block some upward velocity when released
            liftableBlock->speed.y = -3; // Small upward velocity
        }
        player.state = PLAYER_IDLE;
        player.isLifting = false;
    }
    // if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
    //     // Left mouse button is being held down
    // }

    // if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
    //     // Right mouse button was just pressed (single press)
    // }
    return 0;
}