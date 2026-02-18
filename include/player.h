#ifndef PLAYER_HH
#define PLAYER_HH

#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>

#define GRAVITY 0.5f    
#define PLAYER_SNEAK_SPEED 1.5f
#define PLAYER_RUNNING_SPEED 3.0f
#define MAX_FALL_SPEED 10.0f

enum PlayerState {
    PLAYER_NORMAL,
    PLAYER_SNEAK,
    PLAYER_RUNNING,
    PLAYER_AIR, 
    PLAYER_HIDDEN
};

typedef struct Player {
    int id;
    Vector2 position;
    Vector2 speed;
    enum PlayerState state;
    Rectangle boundingBox;
    // Add other player-related properties here
} Player;

Player* createPlayer(int id, Vector2 initialPosition, int width, int height);

enum PlayerState getPlayerState(Player* player);

void setPlayerState(Player* player, enum PlayerState state);    

void renderPlayer(Player* player);

Vector2 getPlayerPosition(Player* player);

void setPlayerPosition(Player* player, Vector2 position);

Vector2 getPlayerSpeed(Player* player);

void setPlayerSpeed(Player* player, Vector2 speed);

void updatePlayer(Player* player);

void destroyPlayer(Player* player);

#endif // PLAYER_HH