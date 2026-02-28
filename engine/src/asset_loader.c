#include "asset_loader.h"
#include <stdio.h>
#include <stdlib.h>

AssetLoader *asset_loader_create() {
    AssetLoader *loader = (AssetLoader *)malloc(sizeof(AssetLoader));
    if (!loader) {
        fprintf(stderr, "Failed to allocate memory for AssetLoader\n");
        return NULL;
    }
    loader->textureAtlases = dynamic_array_create(sizeof(TextureAtlas));
    loader->spriteAtlases = dynamic_array_create(sizeof(SpriteAtlas));
    return loader;
}

int asset_loader_load_texture_atlas(AssetLoader *loader, const char *filePath) {
    
}

int asset_loader_load_sprite_atlas(AssetLoader *loader, const char *filePath);

AssetLoader* asset_loader_destroy(AssetLoader *loader);