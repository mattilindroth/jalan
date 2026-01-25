#include "scene.h"

Scene* createScene(int id, const char* name) {
    Scene* scene = (Scene*)malloc(sizeof(Scene));
    if (scene != NULL) {
        scene->id = id;
        scene->name = name;
    }
    return scene;
}

void destroyScene(Scene* scene) {
    if (scene != NULL) {
        free(scene);
    }
}