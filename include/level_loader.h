#include <stdlib.h>
#include "cJSON.h"
#include "game.h"
#include "player.h"
#include "enemy.h"
#include "obstacle.h"
#include "background_object.h"
#include "light.h"
#include "cJSON.h"

#ifndef LEVEL_LOADER_H
#define LEVEL_LOADER_H

Game* loadLevel(const char* filename, Game *game);

Game* saveLevel(const char* filename, Game *game);

#endif // LEVEL_LOADER_H
