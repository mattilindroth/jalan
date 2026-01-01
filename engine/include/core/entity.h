#pragma once
#include "raylib.h"
#include "render/sprite.h"

typedef struct {
    Rectangle bounds;
    Sprite sprite;
    int layer_id;
    bool collidable;
} Entity;
