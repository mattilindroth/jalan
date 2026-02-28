#include <stdio.h>
#include <stdlib.h>
#include "parallax.h"

Parallax *parallax_create() {
    Parallax *parallax = (Parallax *)malloc(sizeof(Parallax));
    if (!parallax) {
        fprintf(stderr, "Failed to allocate memory for Parallax\n");
        return NULL;
    }
    parallax->layers = dynamic_array_create_default();
    return parallax;
}

void parallax_add_layer(Parallax *parallax, int id, float speed) {
    if (!parallax) return;
    
    ParallaxLayer *layer = (ParallaxLayer *)malloc(sizeof(ParallaxLayer));
    if (!layer) {
        fprintf(stderr, "Failed to allocate memory for ParallaxLayer\n");
        return;
    }
    layer->id = id;
    layer->speed = speed;
    layer->entities = dynamic_array_create_default();
    
    dynamic_array_push(parallax->layers, layer);
}

void parallax_add_entity_to_layer(Parallax *parallax, int layerId, Entity *entity) {
    if (!parallax || !entity) return;
    
    for (int i = 0; i < dynamic_array_size(parallax->layers); i++) {
        ParallaxLayer *layer = (ParallaxLayer *)dynamic_array_get(parallax->layers, i);
        if (layer->id == layerId) {
            dynamic_array_push(layer->entities, entity);
            break;
        }
    }
}

int parallax_get_layer_count(Parallax *parallax) {
    if (!parallax) return 0;
    return (int)dynamic_array_size(parallax->layers);
}

ParallaxLayer* parallax_get_layer_by_id(Parallax *parallax, int layerId) {
    if (!parallax) return NULL;
    
    for (int i = 0; i < dynamic_array_size(parallax->layers); i++) {
        ParallaxLayer *layer = (ParallaxLayer *)dynamic_array_get(parallax->layers, i);
        if (layer->id == layerId) {
            return layer;
        }
    }
    return NULL; // Not found
}

ParallaxLayer* parallax_get_layer_by_index(Parallax *parallax, int index) {
    if (!parallax) return NULL;
    if (index < 0 || index >= dynamic_array_size(parallax->layers)) return NULL;
    return (ParallaxLayer *)dynamic_array_get(parallax->layers, index);
}

Parallax *parallax_destroy(Parallax *parallax) {
    if (!parallax) return NULL;
    
    for (int i = 0; i < dynamic_array_size(parallax->layers); i++) {
        ParallaxLayer *layer = (ParallaxLayer *)dynamic_array_get(parallax->layers, i);
        dynamic_array_destroy(layer->entities);
        free(layer);
    }
    dynamic_array_destroy(parallax->layers);
    free(parallax);
    return NULL;
}