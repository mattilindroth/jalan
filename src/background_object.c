#include "background_object.h"

BackgroundObject *createBackgroundObject(int id, int sceneId, Rectangle bounds, Color color) {
    BackgroundObject *bgObject = (BackgroundObject *)malloc(sizeof(BackgroundObject));
    if (bgObject != NULL) {
        bgObject->id = id;
        bgObject->sceneId = sceneId;
        bgObject->bounds = bounds;
        bgObject->color = color;
    }
    return bgObject;
}

void renderBackgroundObject(BackgroundObject *bgObject) {
    if (bgObject != NULL) {
        DrawRectangleRec(bgObject->bounds, bgObject->color);
    }
}

void destroyBackgroundObject(BackgroundObject *bgObject) {
    if (bgObject != NULL) {
        free(bgObject);
    }
}