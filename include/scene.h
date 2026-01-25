#include <stdio.h>
#include <stdlib.h>

#ifndef SCENE_H
#define SCENE_H

typedef struct Scene {
    // Add scene-related properties here
    int id;
    const char* name;
} Scene;

Scene* createScene(int id, const char* name);

void destroyScene(Scene* scene);

#endif // SCENE_H