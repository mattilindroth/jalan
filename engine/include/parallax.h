#ifndef PARALLAX_HH
#define PARALLAX_HH

#include "dynamic_array.h"
#include "entity.h"

typedef struct ParallaxLayer {
    int id;
    float speed;
    DynamicArray *entities; // Array of Entity pointers
} ParallaxLayer;

typedef struct Parallax {
    DynamicArray *layers; // Array of ParallaxLayer pointers
} Parallax;

Parallax *parallax_create();

void parallax_add_layer(Parallax *parallax, int id, float speed);

void parallax_add_entity_to_layer(Parallax *parallax, int layerId, Entity *entity);

int parallax_get_layer_count(Parallax *parallax);

ParallaxLayer* parallax_get_layer_by_id(Parallax *parallax, int layerId);

ParallaxLayer* parallax_get_layer_by_index(Parallax *parallax, int index);

Parallax *parallax_destroy(Parallax *parallax);


#endif // PARALLAX_HH