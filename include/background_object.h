#ifndef BACKGROUND_OBJECT_H
#define BACKGROUND_OBJECT_H

#include <stdlib.h>
#include "raylib.h"

typedef struct BackgroundObject {
    int id;
    int sceneId;
    Rectangle bounds;
    Color color;
} BackgroundObject;

BackgroundObject *createBackgroundObject(int id, int sceneId, Rectangle bounds, Color color);

void renderBackgroundObject(BackgroundObject *bgObject);

void destroyBackgroundObject(BackgroundObject *bgObject);

#endif // BACKGROUND_OBJECT_H