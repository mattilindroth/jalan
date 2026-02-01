#include "game.h"

Game* createGame(Scene* initialScene, enum GameState initialState) {
    Game* game = (Game*)malloc(sizeof(Game));
    if (game != NULL) {
        game->currentScene = initialScene;
        game->state = initialState;
        game->obstacles = dynamic_array_create_default();
        game->backgroundObjects = dynamic_array_create_default();
        game->player = NULL;
        game->enemy = NULL;
    }
    return game;
}

void addObstacleToGame(Game* game, Obstacle* obstacle) {
    if(game != NULL && obstacle != NULL) {
        dynamic_array_push(game->obstacles, obstacle);
    }
}

void addBackgroundObjectToGame(Game* game, BackgroundObject* backgroundObject) {
    if(game != NULL && backgroundObject != NULL) {
        dynamic_array_push(game->backgroundObjects, backgroundObject);
    }
}

void resolveCollisions(Game* game) {
    if (!game || !game->player || !game->obstacles) return;

    int sceneId = game->currentScene->id;
    int onGround = 0;

    for (int i = 0; i < game->obstacles->size; i++) {
        Obstacle* obs = (Obstacle*)dynamic_array_get(game->obstacles, i);
        
        // Filter obstacles by scene
        if (obs->sceneId != sceneId) continue;

        // Update playerRect with current position for accurate collision detection
        Rectangle playerRect = { 
            game->player->position.x, 
            game->player->position.y, 
            game->player->boundingBox.width, 
            game->player->boundingBox.height 
        };

        enum CollisionType col = checkCollisionWithObstacle(playerRect, obs);
        
        if (col != COLLISION_NONE) {
            // Resolve collision based on the side of impact
            // We adjust position to "undo" the overlap and stop velocity
            if (col == COLLISION_TOP) {
                // Landed on TOP
                game->player->position.y = obs->rectangle.y - game->player->boundingBox.height;
                game->player->speed.y = 0;
                onGround = 1;
            } else if (col == COLLISION_BOTTOM) {
                // Hit BOTTOM (Ceiling)
                game->player->position.y = obs->rectangle.y + obs->rectangle.height;
                game->player->speed.y = 0;
            } else if (col == COLLISION_LEFT) {
                game->player->position.x = obs->rectangle.x - game->player->boundingBox.width;
                game->player->speed.x = 0;
            } else if (col == COLLISION_RIGHT) {
                game->player->position.x = obs->rectangle.x + obs->rectangle.width;
                game->player->speed.x = 0;
            }
        }
    }

    // Update state based on ground contact
    if (onGround) {
        if (game->player->state == PLAYER_AIR) {
            game->player->state = PLAYER_NORMAL;
        }
    } else {
        if (game->player->state == PLAYER_NORMAL) {
            game->player->state = PLAYER_AIR;
        }
    }
}

int updateGame(Game* game) {
    // Implement game update logic here
    
    int sceneId = game->currentScene->id;

    updatePlayer(game->player); 

    updateEnemy(game->enemy, getPlayerPosition(game->player));

    resolveCollisions(game);

    return 0;
}

int handleInput(Game* game) {
    // Implement input handling logic here
    if(game->player->state != PLAYER_AIR) {
        game->player->speed.y = 0; // Reset vertical speed if on ground

        if(IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
            if(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
                game->player->speed.x = PLAYER_SNEAK_SPEED;
                game->player->state = PLAYER_SNEAK;
            } else {
                game->player->speed.x = PLAYER_RUNNING_SPEED;
                game->player->state = PLAYER_RUNNING;            
            }
        } else if(IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
            if(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
                game->player->speed.x = -PLAYER_SNEAK_SPEED;
                game->player->state = PLAYER_SNEAK;            
            } else {
                game->player->speed.x = -PLAYER_RUNNING_SPEED;
                game->player->state = PLAYER_RUNNING;
            }
        } else {
            game->player->speed.x = 0;
        }
    }
    if(IsKeyPressed(KEY_SPACE)) {
        // Jump only if on ground
        if(game->player->state != PLAYER_AIR) {
            game->player->speed.y = -10.0f; // Jump impulse
            game->player->state = PLAYER_AIR;
        }
    }

    return 0;
}

int updateCamera(Game* game) {
    // Implement camera update logic here
    return 0;
}

int renderGame(Game* game) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    //DrawText("Hello, World!", 190, 200, 20, LIGHTGRAY);

    //Render backgroud objects
    for(int i = 0; i < game->backgroundObjects->size; i++) {
        BackgroundObject* bgObj = (BackgroundObject*)dynamic_array_get(game->backgroundObjects, i);
        if(bgObj->sceneId == game->currentScene->id) {
            renderBackgroundObject(bgObj);
        }
    }

    //Render player
    renderPlayer(game->player);  

    //Render obstacles
    for(int i = 0; i < game->obstacles->size; i++) {
        Obstacle* obs = (Obstacle*)dynamic_array_get(game->obstacles, i);
        if(obs->sceneId == game->currentScene->id) {
            DrawRectangleRec(obs->rectangle, obs->color);
        }
    }   

    EndDrawing();
    return 0;
}

void destroyGame(Game* game) {
    if (game != NULL) {
        if (game->player != NULL) {
            destroyPlayer(game->player);
        }
        if (game->enemy != NULL) {
            destroyEnemy(game->enemy);
        }
        if (game->obstacles != NULL) {
            for (int i = 0; i < game->obstacles->size; i++) {
                Obstacle* obs = (Obstacle*)dynamic_array_get(game->obstacles, i);
                destroyObstacle(obs);
            }
            dynamic_array_destroy(game->obstacles);
        }
        if (game->backgroundObjects != NULL) {
            for (int i = 0; i < game->backgroundObjects->size; i++) {
                BackgroundObject* bgObj = (BackgroundObject*)dynamic_array_get(game->backgroundObjects, i);
                destroyBackgroundObject(bgObj);
            }
            dynamic_array_destroy(game->backgroundObjects);
        }
    }
}