#ifndef ASSET_LOADER_HH
#define ASSET_LOADER_HH

#include <stdbool.h>

#include "dynamic_array.h"

//Todo. Probably needs some struct to hold information about textures and sprites and where to find them in the atlas.png.


typedef struct TextureAtlas {
    char *name;
    int positionX;
    int positionY;
    int width;
    int height;
    bool rotated;
    bool trimmed;
} TextureAtlas;

typedef struct SpriteAtlas {
    char *name;
    TextureAtlas *textureAtlas; // Pointer to the TextureAtlas struct
} SpriteAtlas;

typedef struct AssetLoader {
    DynamicArray *textureAtlases; // Array of TextureAtlas structs
    DynamicArray *spriteAtlases;  // Array of SpriteAtlas structs
} AssetLoader;

AssetLoader *asset_loader_create();

int asset_loader_load_texture_atlas(AssetLoader *loader, const char *filePath);

int asset_loader_load_sprite_atlas(AssetLoader *loader, const char *filePath);

AssetLoader* asset_loader_destroy(AssetLoader *loader);
//Todo load assets like textures, sprites, sounds, music, fonts, etc. from files 


#endif // ASSET_LOADER_HH