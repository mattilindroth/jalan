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
    PLAYER_SNEAKING,
    PLAYER_JUMPING,
    PLAYER_FALLING,
    PLAYER_HIDING,
    PLAYER_LIFTING,
    PLAYER_PUSHING,
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

// Rock structures
typedef struct Rock {
    Vector2 position;
    bool collected;
    bool visible;
    Rectangle boundingBox;
} Rock;

typedef struct ThrownRock {
    Vector2 position;
    Vector2 velocity;
    bool active;
    bool hasLanded;
} ThrownRock;

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
    bool isPushing;
    Block *pushedBlock;     // Which block is being pushed
    float breathTimer;      // How long player has been holding breath while hiding
    bool hasRock;           // Does player have a rock?
} Player;

//Structure to represent an enemy
typedef struct Enemy {
    Vector2 position;
    Vector2 speed;
    Rectangle boundingBox;
    enum EnemyType type;
    bool onGround;
    bool isEliminated;
// Sound investigation state
    bool isInvestigating;      // Is the enemy investigating a sound?
    float investigateTimer;    // Time remaining to investigate
    float patrolCenterX;       // X position to patrol around
    float patrolDirection;     // Current patrol direction (1 or -1)
    float originalSpeedX;      // Original speed to restore after investigation
} Enemy;

/************************
** Function declarations
*************************/

int checkCollisionWithGameArea(Rectangle *object);

int checkPlayerCollisionWithEnemy(Player *player, Enemy *enemy);

int updateCamera();

int resetPlayer();

int handleInput();

int updateGame();

int renderFrame();

int updateSoundWaves();

int emitSoundWave(Vector2 origin, float intensity);

int renderSoundWaves();

/**************************
** Game object declarations
***************************/

Camera2D camera = {0};

RenderTexture2D fogMask;

Player player;

Rectangle gameArea = {0, 0, GAME_AREA_WIDTH, GAME_AREA_HEIGHT};

Rectangle waterArea = {0};

Rectangle exitGateway = {0};

bool gameWon = false;

bool playerDead = false;
float deathFadeTimer = 0.0f;
float deathFadeDuration = 1.0f;

BackgroundTree trees[10] = {0};

BackgroundGrave graves[10] = {0};

Enemy ghost;

Enemy zombie;

Block blocks[5] = {0};

Block *liftableBlock = NULL;

float jumpCooldown = 0.0f;

// Lightning system
float lightningTimer = 0.0f;
float nextLightningTime = 0.0f;
bool isLightning = false;
Vector2 lightningStart = {0};
Vector2 lightningEnd = {0};

// Rain system
#define MAX_RAINDROPS 200
typedef struct {
    Vector2 position;
    Vector2 velocity;
} Raindrop;

Raindrop raindrops[MAX_RAINDROPS];

// Sound wave system - visual representation of noise
#define MAX_SOUND_WAVES 10
#define SOUND_WAVE_MAX_RADIUS 550.0f
#define SOUND_WAVE_EXPAND_SPEED 7.0f
#define SOUND_WAVE_EMIT_INTERVAL 0.15f
#define BREATH_HOLD_MAX 10.0f           // Max seconds player can hold breath
#define BREATH_SOUND_INTERVAL 0.4f      // Interval for involuntary sounds when out of breath

typedef struct {
    Vector2 origin;      // Center of the wave
    float radius;        // Current radius
    float maxRadius;     // Maximum radius before fading out completely
    bool active;         // Is this wave active?
    float intensity;     // Initial intensity (based on movement speed)
} SoundWave;

SoundWave soundWaves[MAX_SOUND_WAVES];
float soundWaveEmitTimer = 0.0f;
float breathSoundTimer = 0.0f;  // Timer for involuntary breath sounds

// Rock system
#define MAX_ROCKS 3
Rock rocks[MAX_ROCKS];
ThrownRock thrownRock = {0};

int main(int argc, char *argv[]) {
    
    // Initialize the window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Jalannallas - the final nightmare V0.2 Alpha");

    SetTargetFPS(60);
    
    // Create a render texture for fog mask
    fogMask = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
    //Initialize player. he/she is on ground at start and not hiding.
    player = (Player){{2, SCREEN_HEIGHT * 2}, {0, 0}, {2, SCREEN_HEIGHT * 2, 50, 50}, PLAYER_IDLE, true, false, false, false, 0.0f, false};

    ghost = (Enemy){{GAME_AREA_WIDTH - 100, GAME_AREA_HEIGHT - 100}, {-6.3f, 0}, {GAME_AREA_WIDTH - 100, GAME_AREA_HEIGHT - 100, 40, 60}, ENEMY_TYPE_GHOST, false, false, false, 0, 0, 1.0f, -6.3f};

    zombie = (Enemy){{GAME_AREA_WIDTH - 300, GAME_AREA_HEIGHT - 70}, {-2.7f, 0}, {GAME_AREA_WIDTH - 300, GAME_AREA_HEIGHT - 70, 40, 60}, ENEMY_TYPE_ZOMBIE, false, false, false, 0, 0, 1.0f, -2.7f};

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
    // Initialize lightning system
    nextLightningTime = GetRandomValue(2, 5); // First lightning in 2-5 seconds (60% more frequent)

    // Initialize rain system
    for(int i = 0; i < MAX_RAINDROPS; i++) {
        raindrops[i].position = (Vector2){
            GetRandomValue(0, GAME_AREA_WIDTH), 
            GetRandomValue(-GAME_AREA_HEIGHT, 0)
        };
        raindrops[i].velocity = (Vector2){
            GetRandomValue(-2, 2),  // Wind effect (-2 to 2 pixels per frame)
            GetRandomValue(3, 6)    // Falling speed (3 to 6 pixels per frame)
        };
    }
    
    // Initialize sound wave system
    for(int i = 0; i < MAX_SOUND_WAVES; i++) {
        soundWaves[i].active = false;
        soundWaves[i].radius = 0;
        soundWaves[i].maxRadius = SOUND_WAVE_MAX_RADIUS;
    }
    
    // Initialize rock system
    // Rock 1: at the beginning
    rocks[0] = (Rock){{100, GAME_AREA_HEIGHT - 25}, false, true, {100, GAME_AREA_HEIGHT - 25, 15, 15}};
    
    // Rock 2: after the water area  
    rocks[1] = (Rock){{waterArea.x + waterArea.width + 100, GAME_AREA_HEIGHT - 25}, false, true, {waterArea.x + waterArea.width + 100, GAME_AREA_HEIGHT - 25, 15, 15}};
    
    // Rock 3: at the very end
    rocks[2] = (Rock){{GAME_AREA_WIDTH - 150, GAME_AREA_HEIGHT - 25}, false, true, {GAME_AREA_WIDTH - 150, GAME_AREA_HEIGHT - 25, 15, 15}};
    
    // Initialize thrown rock as inactive
    thrownRock = (ThrownRock){{0, 0}, {0, 0}, false, false};
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
    UnloadRenderTexture(fogMask);
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
    
    //Draw the zombie as a grayish green square
    DrawRectangle(zombie.boundingBox.x, zombie.boundingBox.y, zombie.boundingBox.width, zombie.boundingBox.height, GREEN);

    //Draw blocks
    for(int i = 0; i < 5; i++) {
        DrawRectangleRec(blocks[i].boundingBox, BROWN);
        DrawText("BOX", blocks[i].boundingBox.x + 10, blocks[i].boundingBox.y + 20, 10, BLACK);
    }
    
    // Draw rocks
    for(int i = 0; i < MAX_ROCKS; i++) {
        if(rocks[i].visible && !rocks[i].collected) {
            DrawCircle((int)(rocks[i].position.x + 7), (int)(rocks[i].position.y + 7), 7, GRAY);
            DrawCircle((int)(rocks[i].position.x + 7), (int)(rocks[i].position.y + 7), 5, DARKGRAY);
        }
    }
    
    // Draw thrown rock if active
    if(thrownRock.active) {
        DrawCircle((int)(thrownRock.position.x), (int)(thrownRock.position.y), 4, GRAY);
    }

    // Draw exit gateway
    DrawEllipse(exitGateway.x + exitGateway.width/2, exitGateway.y + exitGateway.height/2, 
                exitGateway.width/2, exitGateway.height/2, BLACK);
    DrawText("EXIT", exitGateway.x + 15, exitGateway.y + 35, 12, WHITE);   

    //Draw rain (affected by fog of war and lightning)
    for(int i = 0; i < MAX_RAINDROPS; i++) {
        DrawRectangle((int)raindrops[i].position.x, (int)raindrops[i].position.y, 5, 16, DARKBLUE);
    }

    // End camera mode
    EndMode2D();
    
    if(!isLightning) {
        // Normal darkness - apply fog mask
        // Calculate player position in screen coordinates
        Vector2 playerScreenPos = GetWorldToScreen2D(
            (Vector2){player.boundingBox.x + player.boundingBox.width/2, 
                      player.boundingBox.y + player.boundingBox.height/2}, 
            camera);
    
    // Create fog mask
    BeginTextureMode(fogMask);
        ClearBackground(BLACK); // Complete darkness background
        
        // Draw a gradient circle - from "just before dark" to complete darkness
        DrawCircleGradient((int)playerScreenPos.x, (int)playerScreenPos.y, 500, 
                          (Color){80, 80, 80, 255},  // Dark gray at center - "just before dark"
                          BLACK);                    // Complete darkness at edges
    EndTextureMode();
    
    // Apply the fog mask to darken areas outside visibility
    BeginBlendMode(BLEND_MULTIPLIED);
        DrawTextureRec(fogMask.texture, (Rectangle){0, 0, SCREEN_WIDTH, -SCREEN_HEIGHT}, (Vector2){0, 0}, WHITE);
    EndBlendMode();
    
    //Draw sound waves on top of the mask, to make them more visible to let the player know consequences of noise (action)
    // Draw sound waves emanating from player (even when hiding, though player is still)
    renderSoundWaves();

    // Draw glowing ghost after fog mask - it glows in the dark
    Vector2 ghostScreenPos = GetWorldToScreen2D(
        (Vector2){ghost.boundingBox.x + ghost.boundingBox.width/2, 
                  ghost.boundingBox.y + ghost.boundingBox.height/2}, 
        camera);
    
    // Draw ghost with glow effect using additive blending
    BeginBlendMode(BLEND_ADDITIVE);
        // Outer glow
        DrawCircleGradient((int)ghostScreenPos.x, (int)ghostScreenPos.y, 30, 
                          (Color){0, 50, 100, 100}, (Color){0, 20, 40, 0});
        // Inner glow  
        DrawCircleGradient((int)ghostScreenPos.x, (int)ghostScreenPos.y, 15, 
                          (Color){50, 100, 150, 150}, (Color){0, 50, 100, 50});
    EndBlendMode();
    
    // Draw the ghost body itself with bright color
    DrawCircle((int)ghostScreenPos.x, (int)ghostScreenPos.y, 8, (Color){100, 150, 255, 200});
    } else {
        // Lightning is active - everything is visible, ghost is invisible
        // Draw lightning bolt
        DrawLineEx(lightningStart, lightningEnd, 3, WHITE);
        
        // Add some jagged branches to make it look more like lightning
        for(int i = 0; i < 3; i++) {
            Vector2 branch1 = {lightningStart.x + GetRandomValue(-20, 20), lightningStart.y + GetRandomValue(100, 200)};
            Vector2 branch2 = {branch1.x + GetRandomValue(-30, 30), branch1.y + GetRandomValue(50, 100)};
            DrawLineEx(branch1, branch2, 2, WHITE);
        }
        
        // Flash effect - brighten the entire screen slightly
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){255, 255, 255, 30});
    }
    
    // Draw UI elements (rock status and controls)
    if (!playerDead && !gameWon) {
        // Rock status
        if (player.hasRock) {
            DrawText("ROCK: READY", 10, 10, 20, YELLOW);
            if (player.state == PLAYER_HIDING) {
                DrawText("Press R to throw rock", 10, 35, 16, WHITE);
            }
        } else {
            DrawText("ROCK: NONE", 10, 10, 20, GRAY);
        }
        
        // Basic controls
        DrawText("WASD/Arrows: Move | Space: Hide | Ctrl: Lift", 10, SCREEN_HEIGHT - 25, 14, WHITE);
    }
    
    // Draw death screen if player is dead
    if(playerDead) {
        // Calculate fade progress (0.0 to 1.0)
        float fadeProgress = deathFadeTimer / deathFadeDuration;
        if(fadeProgress > 1.0f) fadeProgress = 1.0f;
        
        // Fade to white overlay
        unsigned char fadeAlpha = (unsigned char)(fadeProgress * 255);
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){255, 255, 255, fadeAlpha});
        
        // Show death message after fade is complete
        if(fadeProgress >= 1.0f) {
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE); // Full white background
            DrawText("YOU DIED", SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 - 60, 40, BLACK);
            DrawText("Press ESC to quit", SCREEN_WIDTH/2 - 90, SCREEN_HEIGHT/2, 20, BLACK);
            DrawText("Press any other key to try again", SCREEN_WIDTH/2 - 140, SCREEN_HEIGHT/2 + 30, 20, BLACK);
        }
    }
    
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
    // Check if ghost hears any sound waves
    for(int i = 0; i < MAX_SOUND_WAVES; i++) {
        if(soundWaves[i].active) {
            // Check if sound wave reaches the ghost
            float distToGhost = sqrtf(
                powf(ghost.boundingBox.x + ghost.boundingBox.width/2 - soundWaves[i].origin.x, 2) +
                powf(ghost.boundingBox.y + ghost.boundingBox.height/2 - soundWaves[i].origin.y, 2)
            );
            if(distToGhost <= soundWaves[i].radius && distToGhost >= soundWaves[i].radius - 20) {
                // Ghost heard the sound! Start investigating
                ghost.isInvestigating = true;
                ghost.investigateTimer = 10.0f;
                ghost.patrolCenterX = soundWaves[i].origin.x;
                // Move towards the sound
                if(ghost.boundingBox.x < ghost.patrolCenterX) {
                    ghost.patrolDirection = 1.0f;
                } else {
                    ghost.patrolDirection = -1.0f;
                }
            }
        }
    }
    
    if(ghost.isInvestigating) {
        // Investigating mode - patrol around the sound origin
        ghost.investigateTimer -= GetFrameTime();
        
        if(ghost.investigateTimer <= 0) {
            // Done investigating, return to normal patrol
            ghost.isInvestigating = false;
            ghost.speed.x = ghost.originalSpeedX;
        } else {
            // Patrol range expands over time: starts at 25px, expands to 200px as time runs out
            // At 10s remaining: 25px range, at 0s: 200px range
            float timeProgress = 1.0f - (ghost.investigateTimer / 10.0f); // 0 at start, 1 at end
            float patrolRange = 25.0f + (175.0f * timeProgress); // 25 -> 200
            
            float distFromCenter = ghost.boundingBox.x - ghost.patrolCenterX;
            if(distFromCenter > patrolRange) {
                ghost.patrolDirection = -1.0f;
            } else if(distFromCenter < -patrolRange) {
                ghost.patrolDirection = 1.0f;
            }
            ghost.speed.x = fabsf(ghost.originalSpeedX) * ghost.patrolDirection;
        }
    }
    
    ghost.boundingBox.x += ghost.speed.x;

    ghost.boundingBox.y = ghost.position.y + (40 * sin(ghost.boundingBox.x / 90)) ; // Slight vertical movement for floating effect

    if(ghost.boundingBox.x < gameArea.x || (ghost.boundingBox.x + ghost.boundingBox.width) > (gameArea.x + gameArea.width)) {
        ghost.speed.x *= -1; // Reverse direction on wall collision
        ghost.patrolDirection *= -1; // Also reverse patrol direction
    }
    
    // Check if zombie hears any sound waves
    for(int i = 0; i < MAX_SOUND_WAVES; i++) {
        if(soundWaves[i].active) {
            // Check if sound wave reaches the zombie
            float distToZombie = sqrtf(
                powf(zombie.boundingBox.x + zombie.boundingBox.width/2 - soundWaves[i].origin.x, 2) +
                powf(zombie.boundingBox.y + zombie.boundingBox.height/2 - soundWaves[i].origin.y, 2)
            );
            if(distToZombie <= soundWaves[i].radius && distToZombie >= soundWaves[i].radius - 20) {
                // Zombie heard the sound! Start investigating
                zombie.isInvestigating = true;
                zombie.investigateTimer = 10.0f;
                zombie.patrolCenterX = soundWaves[i].origin.x;
                // Move towards the sound
                if(zombie.boundingBox.x < zombie.patrolCenterX) {
                    zombie.patrolDirection = 1.0f;
                } else {
                    zombie.patrolDirection = -1.0f;
                }
            }
        }
    }
    
    if(zombie.isInvestigating) {
        // Investigating mode - patrol around the sound origin
        zombie.investigateTimer -= GetFrameTime();
        
        if(zombie.investigateTimer <= 0) {
            // Done investigating, return to normal patrol
            zombie.isInvestigating = false;
            zombie.speed.x = zombie.originalSpeedX;
        } else {
            // Patrol range expands over time: starts at 25px, expands to 200px as time runs out
            // At 10s remaining: 25px range, at 0s: 200px range
            float timeProgress = 1.0f - (zombie.investigateTimer / 10.0f); // 0 at start, 1 at end
            float patrolRange = 25.0f + (175.0f * timeProgress); // 25 -> 200
            
            float distFromCenter = zombie.boundingBox.x - zombie.patrolCenterX;
            if(distFromCenter > patrolRange) {
                zombie.patrolDirection = -1.0f;
            } else if(distFromCenter < -patrolRange) {
                zombie.patrolDirection = 1.0f;
            }
            zombie.speed.x = fabsf(zombie.originalSpeedX) * zombie.patrolDirection;
        }
    }

    zombie.boundingBox.x += zombie.speed.x;
    
    //zombie.boundingBox.y += zombie.speed.y;
    if(zombie.boundingBox.x < gameArea.x || (zombie.boundingBox.x + zombie.boundingBox.width) > (gameArea.x + gameArea.width)) {
        zombie.speed.x *= -1; // Reverse direction on wall collision
        zombie.patrolDirection *= -1; // Also reverse patrol direction
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
    
    // Rock collection logic
    if (!player.hasRock) {
        for (int i = 0; i < MAX_ROCKS; i++) {
            if (rocks[i].visible && !rocks[i].collected) {
                if (CheckCollisionRecs(player.boundingBox, rocks[i].boundingBox)) {
                    rocks[i].collected = true;
                    rocks[i].visible = false;
                    player.hasRock = true;
                    break; // Only collect one rock at a time
                }
            }
        }
    }
    
    // Update thrown rock physics
    if (thrownRock.active) {
        // Apply gravity to thrown rock
        thrownRock.velocity.y += GRAVITY;
        
        // Update position
        thrownRock.position.x += thrownRock.velocity.x;
        thrownRock.position.y += thrownRock.velocity.y;
        
        // Check if rock hits ground
        if (thrownRock.position.y >= GAME_AREA_HEIGHT - 5) {
            if (!thrownRock.hasLanded) {
                // Rock just landed - emit sound wave
                emitSoundWave(thrownRock.position, 15.0f); // Strong sound intensity
                thrownRock.hasLanded = true;
            }
            thrownRock.active = false; // Rock is no longer active
        }
        
        // Check if rock goes out of bounds horizontally
        if (thrownRock.position.x < 0 || thrownRock.position.x > GAME_AREA_WIDTH) {
            thrownRock.active = false;
        }
    }

    if(player.state == PLAYER_LIFTING && liftableBlock != NULL) {
        // Move the block with the player
        liftableBlock->boundingBox.x = player.boundingBox.x + (player.boundingBox.width - liftableBlock->boundingBox.width) / 2;
        liftableBlock->boundingBox.y = player.boundingBox.y - liftableBlock->boundingBox.height; // Position block above player
    }

    if(player.state != PLAYER_HIDING) { 
        if (checkPlayerCollisionWithEnemy(&player, &ghost)) {
            // Trigger death sequence instead of immediate reset
            playerDead = true;
            deathFadeTimer = 0.0f;
        }
        enum CollisionState collisionWithZombie = checkPlayerCollisionWithEnemy(&player, &zombie);
        switch (collisionWithZombie) {
            case COLLISION_CEILING:
            case COLLISION_WALL:
                // Trigger death sequence instead of immediate reset
                playerDead = true;
                deathFadeTimer = 0.0f;
                break;
            default:
                break;
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

    // Update rain system
    for(int i = 0; i < MAX_RAINDROPS; i++) {
        // Update raindrop position
        raindrops[i].position.x += raindrops[i].velocity.x;
        raindrops[i].position.y += raindrops[i].velocity.y;
        
        // Reset raindrop if it goes below ground or off screen
        if(raindrops[i].position.y > GAME_AREA_HEIGHT || 
           raindrops[i].position.x < -50 || raindrops[i].position.x > GAME_AREA_WIDTH + 50) {
            raindrops[i].position = (Vector2){
                GetRandomValue(-50, GAME_AREA_WIDTH + 50), 
                GetRandomValue(-100, -10)
            };
            raindrops[i].velocity = (Vector2){
                GetRandomValue(-2, 2),  // Wind effect
                GetRandomValue(3, 6)    // Falling speed
            };
        }
    }

    // Update lightning system
    if(isLightning) {
        lightningTimer -= GetFrameTime();
        if(lightningTimer <= 0) {
            isLightning = false;
            nextLightningTime = GetRandomValue(3, 9); // Next lightning in 3-9 seconds (60% more frequent)
        }
    } else {
        nextLightningTime -= GetFrameTime();
        if(nextLightningTime <= 0) {
            // Start lightning
            isLightning = true;
            lightningTimer = 0.5f; // 0.5 second duration
            
            // Random lightning position in screen coordinates
            lightningStart = (Vector2){GetRandomValue(0, SCREEN_WIDTH), 0};
            lightningEnd = (Vector2){lightningStart.x + GetRandomValue(-50, 50), SCREEN_HEIGHT};
            
            // Lightning thunder causes enemies to lose interest - the loud sound distracts them
            // They stop investigating but continue in their current direction
            ghost.isInvestigating = false;
            // Keep current direction, just restore normal speed magnitude
            if(ghost.speed.x > 0) {
                ghost.speed.x = fabsf(ghost.originalSpeedX);
            } else {
                ghost.speed.x = -fabsf(ghost.originalSpeedX);
            }
            zombie.isInvestigating = false;
            if(zombie.speed.x > 0) {
                zombie.speed.x = fabsf(zombie.originalSpeedX);
            } else {
                zombie.speed.x = -fabsf(zombie.originalSpeedX);
            }
        }
    }

    // Update death fade timer
    if(playerDead && deathFadeTimer < deathFadeDuration) {
        deathFadeTimer += GetFrameTime();
    }

     // Update sound wave system
    updateSoundWaves();

    return 0;
}

int handleInput() {
    
    // If player is dead, handle death screen input
    if(playerDead && deathFadeTimer >= deathFadeDuration) {
        if(IsKeyPressed(KEY_ESCAPE)) {
            // Exit the game
            CloseWindow();
        } else if(GetKeyPressed() != 0) {
            // Any other key pressed - restart
            resetPlayer();
            playerDead = false;
            deathFadeTimer = 0.0f;
        }
        return 0; // Don't process other input while dead
    }
    
    // If game is won, only allow ESC to quit
    if(gameWon) {
        if(IsKeyPressed(KEY_ESCAPE)) {
            // Exit the game
            CloseWindow();
        }
        return 0;
    }
    
    // If player is hiding, don't allow movement - only allow unhiding with SPACE and throwing with R
    if(player.state == PLAYER_HIDING) {
        if (IsKeyPressed(KEY_SPACE)) {
            player.state = PLAYER_IDLE;
        }
        
        // Rock throwing when hiding
        if (IsKeyPressed(KEY_R) && player.hasRock && !thrownRock.active) {
            // Throw rock 200px to the right from player position
            thrownRock.position = (Vector2){player.boundingBox.x + player.boundingBox.width, player.boundingBox.y + player.boundingBox.height / 2};
            thrownRock.velocity = (Vector2){8.0f, -2.0f}; // Horizontal: 8px/frame ≈ 200px total, slight upward arc
            thrownRock.active = true;
            thrownRock.hasLanded = false;
            player.hasRock = false; // Player no longer has rock
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
        
        if(player.onGround) {
            // W key or Up arrow is being held down - only allow jumping when on ground
            // Check if player is in water for reduced jump power
            bool playerInWater = CheckCollisionRecs(player.boundingBox, waterArea);
            float jumpPower = playerInWater ? -6.0f : -9.0f; // Normal jump power, reduced only in water
            player.speed.y = jumpPower;
            if(player.state != PLAYER_LIFTING) {
                player.state = PLAYER_JUMPING;
            }
            player.onGround = false; // Immediately set onGround to false
            jumpCooldown = 0.3f; // 300ms cooldown to prevent continuous jumping
            printf("Jump executed!\n");
        } else {
            printf("Jump blocked - not on ground\n");
        }
    }
    
    // Handle movement input after jump
    if(player.onGround || player.state == PLAYER_LIFTING) {
        // Check if player is in water for slower movement
        bool playerInWater = CheckCollisionRecs(player.boundingBox, waterArea);
        bool isSneaking = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
        
        // Base speed, reduced in water, further reduced when sneaking
        float baseSpeed = playerInWater ? 2.25f : 4.5f;
        float moveSpeed = isSneaking ? baseSpeed * 0.4f : baseSpeed; // 40% speed when sneaking
        
        // Reset pushing state
        player.isPushing = false;
        player.pushedBlock = NULL;
        
        if(IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
            // Check for block collision before moving left
            Rectangle nextPlayerPos = player.boundingBox;
            nextPlayerPos.x -= moveSpeed;
            
            bool canMove = true;
            for(int i = 0; i < 5; i++) {
                if(CheckCollisionRecs(nextPlayerPos, blocks[i].boundingBox)) {
                    // Check if we can push the block (not lifting this block and block can move)
                    if(&blocks[i] != liftableBlock || !player.isLifting) {
                        Rectangle nextBlockPos = blocks[i].boundingBox;
                        nextBlockPos.x -= moveSpeed * 0.3f; // Blocks move slower when pushed
                        
                        // Check if block can move (not hitting game boundaries or other blocks)
                        bool blockCanMove = (nextBlockPos.x >= gameArea.x);
                        
                        // Check collision with other blocks
                        for(int j = 0; j < 5; j++) {
                            if(i != j && CheckCollisionRecs(nextBlockPos, blocks[j].boundingBox)) {
                                blockCanMove = false;
                                break;
                            }
                        }
                        
                        if(blockCanMove) {
                            // Push the block
                            blocks[i].boundingBox.x = nextBlockPos.x;
                            player.speed.x = -moveSpeed * 0.3f; // Slow pushing speed
                            player.isPushing = true;
                            player.pushedBlock = &blocks[i];
                            if(player.state != PLAYER_LIFTING) {
                                player.state = PLAYER_PUSHING;
                            }
                        } else {
                            // Block can't move, player can't move
                            player.speed.x = 0;
                            canMove = false;
                        }
                    } else {
                        // Can't push lifted block, stop movement
                        player.speed.x = 0;
                        canMove = false;
                    }
                    break;
                }
            }
            
            if(canMove && !player.isPushing) {
                // Normal left movement
                player.speed.x = -moveSpeed;
                if(player.state != PLAYER_LIFTING) {
                    player.state = isSneaking ? PLAYER_SNEAKING : PLAYER_RUNNING;
                }
            }
        } else 
        if(IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
            // Check for block collision before moving right
            Rectangle nextPlayerPos = player.boundingBox;
            nextPlayerPos.x += moveSpeed;
            
            bool canMove = true;
            for(int i = 0; i < 5; i++) {
                if(CheckCollisionRecs(nextPlayerPos, blocks[i].boundingBox)) {
                    // Check if we can push the block (not lifting this block and block can move)
                    if(&blocks[i] != liftableBlock || !player.isLifting) {
                        Rectangle nextBlockPos = blocks[i].boundingBox;
                        nextBlockPos.x += moveSpeed * 0.3f; // Blocks move slower when pushed
                        
                        // Check if block can move (not hitting game boundaries or other blocks)
                        bool blockCanMove = (nextBlockPos.x + nextBlockPos.width <= gameArea.x + gameArea.width);
                        
                        // Check collision with other blocks
                        for(int j = 0; j < 5; j++) {
                            if(i != j && CheckCollisionRecs(nextBlockPos, blocks[j].boundingBox)) {
                                blockCanMove = false;
                                break;
                            }
                        }
                        
                        if(blockCanMove) {
                            // Push the block
                            blocks[i].boundingBox.x = nextBlockPos.x;
                            player.speed.x = moveSpeed * 0.3f; // Slow pushing speed
                            player.isPushing = true;
                            player.pushedBlock = &blocks[i];
                            if(player.state != PLAYER_LIFTING) {
                                player.state = PLAYER_PUSHING;
                            }
                        } else {
                            // Block can't move, player can't move
                            player.speed.x = 0;
                            canMove = false;
                        }
                    } else {
                        // Can't push lifted block, stop movement
                        player.speed.x = 0;
                        canMove = false;
                    }
                    break;
                }
            }
            
            if(canMove && !player.isPushing) {
                // Normal right movement
                player.speed.x = moveSpeed;
                if(player.state != PLAYER_LIFTING) {
                    player.state = isSneaking ? PLAYER_SNEAKING : PLAYER_RUNNING;
                }
            }
        } else {
            player.speed.x = 0;
            if(player.state != PLAYER_LIFTING) {
                player.state = PLAYER_IDLE;
            }
        }
    } else {
        // Handle air movement (while jumping/falling) - also affected by water
        bool playerInWater = CheckCollisionRecs(player.boundingBox, waterArea);
        float airMoveSpeed = playerInWater ? 1.5f : 3.0f; // Reduced air control, even less in water
        
        if(IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
            player.speed.x = -airMoveSpeed;
        } else if(IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
            player.speed.x = airMoveSpeed;
        } else {
            player.speed.x *= 0.9f; // Gradually reduce horizontal speed when no input
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

int resetPlayer() {
    // Reset player to starting position and state
    player.boundingBox.x = 2;
    player.boundingBox.y = SCREEN_HEIGHT * 2;
    player.speed.x = 0;
    player.speed.y = 0;
    player.onGround = true;
    player.state = PLAYER_IDLE;
    player.isLifting = false;
    player.canHide = false;
    player.canLift = false;
    player.hasRock = false; // Reset rock status
    
    // Clear any lifted block
    if(liftableBlock != NULL && player.isLifting) {
        liftableBlock->speed.y = -3; // Release block
    }
    liftableBlock = NULL;
    
    // Reset jump cooldown
    jumpCooldown = 0.0f;
    
    // Reset thrown rock
    thrownRock.active = false;
    thrownRock.hasLanded = false;
    
    // Reset all rocks to their original state
    for (int i = 0; i < MAX_ROCKS; i++) {
        rocks[i].collected = false;
        rocks[i].visible = true;
    }
    
    return 0;
}

/************************
** Sound Wave Functions
*************************/

// Emit a new sound wave from the given origin with given intensity
int emitSoundWave(Vector2 origin, float intensity) {
    // Find an inactive wave slot
    for(int i = 0; i < MAX_SOUND_WAVES; i++) {
        if(!soundWaves[i].active) {
            soundWaves[i].origin = origin;
            soundWaves[i].radius = 5.0f; // Start with small radius
            soundWaves[i].maxRadius = SOUND_WAVE_MAX_RADIUS * intensity; // Louder sounds travel further
            soundWaves[i].intensity = intensity;
            soundWaves[i].active = true;
            return 0;
        }
    }
    return -1; // No available slot
}

// Update all active sound waves
int updateSoundWaves() {
    // Expand and fade out active waves
    for(int i = 0; i < MAX_SOUND_WAVES; i++) {
        if(soundWaves[i].active) {
            // Expand the wave
            soundWaves[i].radius += SOUND_WAVE_EXPAND_SPEED;
            
            // Deactivate if reached max radius
            if(soundWaves[i].radius >= soundWaves[i].maxRadius) {
                soundWaves[i].active = false;
            }
        }
    }
    
    // Emit sound waves based on player movement
    if(player.state == PLAYER_RUNNING || player.state == PLAYER_SNEAKING || player.state == PLAYER_JUMPING || player.state == PLAYER_FALLING || player.state == PLAYER_LIFTING) {
        soundWaveEmitTimer -= GetFrameTime();
        
        if(soundWaveEmitTimer <= 0) {
            // Calculate movement intensity based on speed
            float speedMagnitude = sqrtf(player.speed.x * player.speed.x + player.speed.y * player.speed.y);
            float intensity = speedMagnitude / 10.0f; // Normalize to 0-1 range approximately
            if(intensity > 1.0f) intensity = 1.0f;
            if(intensity < 0.3f) intensity = 0.3f; // Minimum intensity for visibility
            
            // Sneaking significantly reduces sound
            if(player.state == PLAYER_SNEAKING) {
                intensity *= 0.2f; // 80% quieter when sneaking
            }
            
            // Lifting makes more noise - increase intensity
            if(player.state == PLAYER_LIFTING && intensity < 0.6f) {
                intensity = 0.6f;
            }
            
            // Emit from player center
            Vector2 playerCenter = {
                player.boundingBox.x + player.boundingBox.width / 2,
                player.boundingBox.y + player.boundingBox.height / 2
            };
            emitSoundWave(playerCenter, intensity);
            
            // Reset timer - faster movement = more frequent waves
            soundWaveEmitTimer = SOUND_WAVE_EMIT_INTERVAL;
        }
    } else if(player.state == PLAYER_HIDING) {
        // Player is hiding - update breath timer
        player.breathTimer += GetFrameTime();
        
        // After holding breath for max time, start emitting involuntary sounds
        if(player.breathTimer > BREATH_HOLD_MAX) {
            breathSoundTimer -= GetFrameTime();
            
            if(breathSoundTimer <= 0) {
                // Emit small involuntary sound (gasping/heartbeat)
                Vector2 playerCenter = {
                    player.boundingBox.x + player.boundingBox.width / 2,
                    player.boundingBox.y + player.boundingBox.height / 2
                };
                // Small intensity - quiet gasping sounds
                float intensity = 0.25f;
                emitSoundWave(playerCenter, intensity);
                
                breathSoundTimer = BREATH_SOUND_INTERVAL;
            }
        }
        // Reset movement sound timer
        soundWaveEmitTimer = 0;
    } else {
        // Reset timers when not moving and not hiding
        soundWaveEmitTimer = 0;
        player.breathTimer = 0;  // Reset breath when not hiding
        breathSoundTimer = 0;
    }
    
    return 0;
}

// Render all active sound waves
// Note: This function draws in screen coordinates when called outside BeginMode2D
int renderSoundWaves() {
    for(int i = 0; i < MAX_SOUND_WAVES; i++) {
        if(soundWaves[i].active) {
            // Convert world coordinates to screen coordinates
            Vector2 screenPos = GetWorldToScreen2D(soundWaves[i].origin, camera);
            
            // Calculate fade based on distance from origin (radius)
            // At radius 0: full intensity (white)
            // At max radius: fully faded (transparent)
            float fadeProgress = soundWaves[i].radius / soundWaves[i].maxRadius;
            
            // Use a slower fade curve (squared) so waves stay brighter longer
            float fadeFactor = (1.0f - fadeProgress) * (1.0f - fadeProgress * 0.5f);
            
            // Intensity affects the starting brightness
            // Higher intensity = whiter/louder, fades to dark/quiet
            unsigned char brightness = (unsigned char)(fadeFactor * 255);
            unsigned char alpha = (unsigned char)(fadeFactor * 220 * soundWaves[i].intensity);
            
            Color waveColor = {brightness, brightness, brightness, alpha};
            
            // Draw the wave as a ring (circle outline)
            // Use multiple circles with decreasing alpha for a softer look
            DrawCircleLines((int)screenPos.x, (int)screenPos.y, 
                           soundWaves[i].radius, waveColor);
            
            // Draw a slightly smaller ring for thickness effect
            if(soundWaves[i].radius > 3) {
                Color innerColor = {brightness, brightness, brightness, (unsigned char)(alpha * 0.5f)};
                DrawCircleLines((int)screenPos.x, (int)screenPos.y, 
                               soundWaves[i].radius - 2, innerColor);
            }
        }
    }
    return 0;
}