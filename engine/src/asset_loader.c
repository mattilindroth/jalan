#include "asset_loader.h"
#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Passes back the parsed cJSON tree via double pointer. Caller owns it and must cJSON_Delete() it.
int load_json_file(const char *filePath, cJSON **json) {
    FILE *file = fopen(filePath, "r");
    if (!file) {
        return 0;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *data = malloc(length + 1);
    if (!data) {
        fclose(file);
        return 0;
    }

    fread(data, 1, length, file);
    data[length] = '\0';
    fclose(file);

    *json = cJSON_Parse(data);
    free(data);

    if (!*json) {
        return 0;
    }
    return 1;
}

// Parses a single cJSON frame item (one child of the "frames" object).
// The item's key is the filename e.g. "wooden_wall.png"; we strip the extension for the name.
TextureAtlas* parse_texture_atlas(cJSON *item) {
    if (!item) return NULL;

    cJSON *frameRect = cJSON_GetObjectItem(item, "frame");
    cJSON *rotated   = cJSON_GetObjectItem(item, "rotated");
    cJSON *trimmed   = cJSON_GetObjectItem(item, "trimmed");

    if (!frameRect) return NULL;

    TextureAtlas *atlas = (TextureAtlas *)malloc(sizeof(TextureAtlas));
    if (!atlas) return NULL;

    // item->string is the JSON key, e.g. "wooden_wall.png"
    const char *key = item->string;
    const char *dot = strrchr(key, '.');
    size_t nameLen = dot ? (size_t)(dot - key) : strlen(key);
    atlas->name = (char *)malloc(nameLen + 1);
    if (atlas->name) {
        memcpy(atlas->name, key, nameLen);
        atlas->name[nameLen] = '\0';
    }

    atlas->positionX = cJSON_GetObjectItem(frameRect, "x")->valueint;
    atlas->positionY = cJSON_GetObjectItem(frameRect, "y")->valueint;
    atlas->width     = cJSON_GetObjectItem(frameRect, "w")->valueint;
    atlas->height    = cJSON_GetObjectItem(frameRect, "h")->valueint;
    atlas->rotated   = rotated ? cJSON_IsTrue(rotated) : false;
    atlas->trimmed   = trimmed ? cJSON_IsTrue(trimmed) : false;

    return atlas;
}

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
    cJSON *json = NULL;
    if (!load_json_file(filePath, &json)) {
        fprintf(stderr, "Failed to load JSON file: %s\n", filePath);
        return 0;
    }

    cJSON *frames = cJSON_GetObjectItem(json, "frames");
    if (!frames) {
        fprintf(stderr, "No 'frames' object found in: %s\n", filePath);
        cJSON_Delete(json);
        return 0;
    }

    // "frames" is a JSON object — iterate its children with cJSON_ArrayForEach
    cJSON *item = NULL;
    cJSON_ArrayForEach(item, frames) {
        TextureAtlas *atlas = parse_texture_atlas(item);
        if (atlas) {
            dynamic_array_push(loader->textureAtlases, atlas);
        }
    }

    cJSON_Delete(json);
    return 1;
}

int asset_loader_load_sprite_atlas(AssetLoader *loader, const char *filePath) {
    fprintf(stderr, "asset_loader_load_sprite_atlas not implemented yet\n");
    return 0; // Not implemented yet
}


AssetLoader* asset_loader_destroy(AssetLoader *loader) {
    if (!loader) return NULL;

    // Free texture atlases
    for (size_t i = 0; i < loader->textureAtlases->size; i++) {
        TextureAtlas *atlas = (TextureAtlas *)dynamic_array_get(loader->textureAtlases, i);
        if (atlas) {
            free(atlas->name);
            free(atlas);
        }
    }
    dynamic_array_destroy(loader->textureAtlases);

    // Free sprite atlases
    for (size_t i = 0; i < loader->spriteAtlases->size; i++) {
        SpriteAtlas *spriteAtlas = (SpriteAtlas *)dynamic_array_get(loader->spriteAtlases, i);
        if (spriteAtlas) {
            free(spriteAtlas->name);
            // Note: We don't free spriteAtlas->textureAtlas here since it's shared with textureAtlases
            free(spriteAtlas);
        }
    }
    dynamic_array_destroy(loader->spriteAtlases);

    free(loader);
    return NULL;
}