#include "player.h"

Player* createPlayer(int id, Vector2 initialPosition, int width, int height) {
    Player* player = (Player*)malloc(sizeof(Player));
    if (player != NULL) {
        player->id = id;
        player->position = initialPosition;
        player->speed = (Vector2){0, 0};
        player->state = PLAYER_NORMAL;
        player->boundingBox = (Rectangle){initialPosition.x, initialPosition.y, width, height};
    }
    return player;
}

enum PlayerState getPlayerState(Player* player) {
    return player->state;
}

void setPlayerState(Player* player, enum PlayerState state) {
    player->state = state;
}

Vector2 getPlayerPosition(Player* player) {
    return player->position;
}

void setPlayerPosition(Player* player, Vector2 position) {
    player->position = position;
    player->boundingBox.x = position.x;
    player->boundingBox.y = position.y;
}

Vector2 getPlayerSpeed(Player* player) {
    return player->speed;
}

void renderPlayer(Player* player) {
    Rectangle playerRect = {player->position.x, player->position.y, player->boundingBox.width, player->boundingBox.height};   
    DrawRectangleRec(playerRect, BLUE);
    //DrawRectangleRec(player->boundingBox, BLUE);
}

void setPlayerSpeed(Player* player, Vector2 speed) {
    player->speed = speed;
}

void updatePlayer(Player* player) {
    if(player->state == PLAYER_AIR) {
        player->speed.y += GRAVITY;
    }

    if(player->speed.y > MAX_FALL_SPEED) {
        player->speed.y = MAX_FALL_SPEED;
    }

    player->position.x += player->speed.x;
    player->position.y += player->speed.y;
    player->boundingBox.x = player->position.x;
    player->boundingBox.y = player->position.y;
}

void destroyPlayer(Player* player) {
    free(player);
}