#include "level_loader.h"


/**********************************************************
 *** Helper function to load a Color from a cJSON object
 **********************************************************/

Color loadColor(cJSON *colorJson) {
    Color color = {
        cJSON_GetObjectItem(colorJson, "r")->valueint,
        cJSON_GetObjectItem(colorJson, "g")->valueint,
        cJSON_GetObjectItem(colorJson, "b")->valueint,
        cJSON_GetObjectItem(colorJson, "a")->valueint
    };
    return color;
}

Obstacle *loadObstacle(cJSON *obstacleJson) {
    int id = cJSON_GetObjectItem(obstacleJson, "id")->valueint;
    int sceneId = cJSON_GetObjectItem(obstacleJson, "sceneId")->valueint;

    cJSON *rectJson = cJSON_GetObjectItem(obstacleJson, "rectangle");
    Rectangle rect = {
        cJSON_GetObjectItem(rectJson, "x")->valuedouble,
        cJSON_GetObjectItem(rectJson, "y")->valuedouble,
        cJSON_GetObjectItem(rectJson, "width")->valuedouble,
        cJSON_GetObjectItem(rectJson, "height")->valuedouble
    };

    cJSON *colorJson = cJSON_GetObjectItem(obstacleJson, "color");
    Color color = loadColor(colorJson);

    return createObstacle(id, sceneId, rect, color);
}

BackgroundObject *loadBackgroundObject(cJSON *bgObjectJson) {
    int id = cJSON_GetObjectItem(bgObjectJson, "id")->valueint;
    int sceneId = cJSON_GetObjectItem(bgObjectJson, "sceneId")->valueint;

    cJSON *boundsJson = cJSON_GetObjectItem(bgObjectJson, "bounds");
    Rectangle bounds = {
        cJSON_GetObjectItem(boundsJson, "x")->valuedouble,
        cJSON_GetObjectItem(boundsJson, "y")->valuedouble,
        cJSON_GetObjectItem(boundsJson, "width")->valuedouble,
        cJSON_GetObjectItem(boundsJson, "height")->valuedouble
    };

    cJSON *colorJson = cJSON_GetObjectItem(bgObjectJson, "color");
    Color color = loadColor(colorJson);

    return createBackgroundObject(id, sceneId, bounds, color);
}

Light *loadLight(cJSON *lightJson) {
    cJSON *posJson = cJSON_GetObjectItem(lightJson, "position");
    Vector2 position = {
        cJSON_GetObjectItem(posJson, "x")->valuedouble,
        cJSON_GetObjectItem(posJson, "y")->valuedouble
    };

    float radius = cJSON_GetObjectItem(lightJson, "radius")->valuedouble;
    float lightingRadius = cJSON_GetObjectItem(lightJson, "lightingRadius")->valuedouble;
    
    cJSON *colorJson = cJSON_GetObjectItem(lightJson, "color");
    Color color = loadColor(colorJson);
    
    cJSON *flickerItem = cJSON_GetObjectItem(lightJson, "flicker");
    bool flicker = flickerItem ? cJSON_IsTrue(flickerItem) : false;
    
    cJSON *breakableItem = cJSON_GetObjectItem(lightJson, "breakable");
    bool breakable = breakableItem ? cJSON_IsTrue(breakableItem) : false;
    
    cJSON *isOnItem = cJSON_GetObjectItem(lightJson, "isOn");
    bool isOn = isOnItem ? cJSON_IsTrue(isOnItem) : true;  // Default to true if not specified

    Light* light = createLight(position, radius, lightingRadius, color, flicker, breakable, isOn);
    if (light) {
        light->isOn = true;  // Default lights to on when loaded
        light->flickerTimer = 0.0f;  // Reset flicker timer
    }
    
    return light;
}

Enemy *loadEnemy(cJSON *enemyJson) {
    int id = cJSON_GetObjectItem(enemyJson, "id")->valueint;
    int sceneId = cJSON_GetObjectItem(enemyJson, "sceneId")->valueint;

    cJSON *posJson = cJSON_GetObjectItem(enemyJson, "position");
    Vector2 position = {
        cJSON_GetObjectItem(posJson, "x")->valueint,
        cJSON_GetObjectItem(posJson, "y")->valueint
    };

    cJSON *patrolAJson = cJSON_GetObjectItem(enemyJson, "patrolPointA");
    Vector2 patrolA = {
        cJSON_GetObjectItem(patrolAJson, "x")->valueint,
        cJSON_GetObjectItem(patrolAJson, "y")->valueint
    };

    cJSON *patrolBJson = cJSON_GetObjectItem(enemyJson, "patrolPointB");
    Vector2 patrolB = {
        cJSON_GetObjectItem(patrolBJson, "x")->valueint,
        cJSON_GetObjectItem(patrolBJson, "y")->valueint
    };

    float speed = cJSON_GetObjectItem(enemyJson, "speed")->valuedouble;
    bool hasLight = cJSON_IsTrue(cJSON_GetObjectItem(enemyJson, "hasLight"));

    return createEnemy(id, sceneId, position, patrolA, patrolB, speed, hasLight);
}


Game* loadLevel(const char* filename, Game *game) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *data = malloc(length + 1);
    if (!data) {
        fclose(file);
        return NULL;
    }

    fread(data, 1, length, file);
    data[length] = '\0';
    fclose(file);

    cJSON *json = cJSON_Parse(data);
    free(data);

    if (!json) {
        return NULL;
    }

    cJSON *levels = cJSON_GetObjectItem(json, "Levels");
    if (!cJSON_IsArray(levels)) {
        cJSON_Delete(json);
        return NULL;
    }
    
    int levelCount = cJSON_GetArraySize(levels);
    
    if (levelCount > 0) {
        cJSON *level = cJSON_GetArrayItem(levels, 0);
        
        // Load player
        cJSON *playerJson = cJSON_GetObjectItem(level, "player");
        if (playerJson) {
            int playerId = cJSON_GetObjectItem(playerJson, "id")->valueint;
            int playerX = cJSON_GetObjectItem(playerJson, "x")->valueint;
            int playerY = cJSON_GetObjectItem(playerJson, "y")->valueint;
            
            Player *player = createPlayer(playerId, (Vector2){playerX, playerY}, 80, 80);
            game->player = player;
        }
        
        // Load enemies
        cJSON *enemiesArray = cJSON_GetObjectItem(level, "enemies");
        if (cJSON_IsArray(enemiesArray)) {
            int enemyCount = cJSON_GetArraySize(enemiesArray);
            for (int e = 0; e < enemyCount; e++) {
                cJSON *enemyJson = cJSON_GetArrayItem(enemiesArray, e);
                
                Enemy *enemy = loadEnemy(enemyJson);
                game->enemy = enemy;
            }
        }
        
        // Load obstacles
        cJSON *obstaclesArray = cJSON_GetObjectItem(level, "obstacles");
        if (cJSON_IsArray(obstaclesArray)) {
            int obstacleCount = cJSON_GetArraySize(obstaclesArray);
            for (int o = 0; o < obstacleCount; o++) {
                cJSON *obstacleJson = cJSON_GetArrayItem(obstaclesArray, o);
                
                Obstacle *obstacle = loadObstacle(obstacleJson);
                addObstacleToGame(game, obstacle);
            }
        }
        
        // Load background objects
        cJSON *bgObjectsArray = cJSON_GetObjectItem(level, "backgroundObjects");
        if (cJSON_IsArray(bgObjectsArray)) {
            int bgObjectCount = cJSON_GetArraySize(bgObjectsArray);
            for (int b = 0; b < bgObjectCount; b++) {
                cJSON *bgObjectJson = cJSON_GetArrayItem(bgObjectsArray, b);
                
                BackgroundObject *bgObject = loadBackgroundObject(bgObjectJson);
                addBackgroundObjectToGame(game, bgObject);
            }
        }
        
        // Load lights
        cJSON *lightsArray = cJSON_GetObjectItem(level, "lights");
        if (cJSON_IsArray(lightsArray)) {
            int lightCount = cJSON_GetArraySize(lightsArray);
            for (int l = 0; l < lightCount; l++) {
                cJSON *lightJson = cJSON_GetArrayItem(lightsArray, l);
                
                Light *light = loadLight(lightJson);
                addLightToGame(game, light);
            }
        }
    }

    cJSON_Delete(json);
    return game;
}


Game* saveLevel(const char* filename, Game *game) {
    if (!game || !filename) return game;
    
    // Create root JSON object
    cJSON *root = cJSON_CreateObject();
    cJSON *levelsArray = cJSON_CreateArray();
    cJSON_AddItemToObject(root, "Levels", levelsArray);
    
    // Create level object
    cJSON *level = cJSON_CreateObject();
    cJSON_AddItemToArray(levelsArray, level);
    
    // Add player data
    if (game->player) {
        cJSON *player = cJSON_CreateObject();
        cJSON_AddNumberToObject(player, "id", game->player->id);
        cJSON_AddNumberToObject(player, "x", game->player->position.x);
        cJSON_AddNumberToObject(player, "y", game->player->position.y);
        cJSON_AddItemToObject(level, "player", player);
    }
    
    // Add enemies data
    cJSON *enemies = cJSON_CreateArray();
    cJSON_AddItemToObject(level, "enemies", enemies);
    
    if (game->enemy) {
        cJSON *enemy = cJSON_CreateObject();
        cJSON_AddNumberToObject(enemy, "id", game->enemy->id);
        cJSON_AddNumberToObject(enemy, "sceneId", game->enemy->sceneId);
        
        cJSON *position = cJSON_CreateObject();
        cJSON_AddNumberToObject(position, "x", game->enemy->position.x);
        cJSON_AddNumberToObject(position, "y", game->enemy->position.y);
        cJSON_AddItemToObject(enemy, "position", position);
        
        cJSON *patrolA = cJSON_CreateObject();
        cJSON_AddNumberToObject(patrolA, "x", game->enemy->patrolPointA.x);
        cJSON_AddNumberToObject(patrolA, "y", game->enemy->patrolPointA.y);
        cJSON_AddItemToObject(enemy, "patrolPointA", patrolA);
        
        cJSON *patrolB = cJSON_CreateObject();
        cJSON_AddNumberToObject(patrolB, "x", game->enemy->patrolPointB.x);
        cJSON_AddNumberToObject(patrolB, "y", game->enemy->patrolPointB.y);
        cJSON_AddItemToObject(enemy, "patrolPointB", patrolB);
        
        cJSON_AddNumberToObject(enemy, "speed", game->enemy->speed);
        cJSON_AddBoolToObject(enemy, "hasLight", false);  // Default value since not in struct
        
        cJSON_AddItemToArray(enemies, enemy);
    }
    
    // Add obstacles data
    cJSON *obstacles = cJSON_CreateArray();
    cJSON_AddItemToObject(level, "obstacles", obstacles);
    
    if (game->obstacles) {
        for (int i = 0; i < game->obstacles->size; i++) {
            Obstacle *obs = (Obstacle*)dynamic_array_get(game->obstacles, i);
            if (obs->sceneId == game->currentScene->id) {
                cJSON *obstacle = cJSON_CreateObject();
                cJSON_AddNumberToObject(obstacle, "id", obs->id);
                cJSON_AddNumberToObject(obstacle, "sceneId", obs->sceneId);
                cJSON_AddStringToObject(obstacle, "name", "editorObstacle");
                
                cJSON *rectangle = cJSON_CreateObject();
                cJSON_AddNumberToObject(rectangle, "x", obs->rectangle.x);
                cJSON_AddNumberToObject(rectangle, "y", obs->rectangle.y);
                cJSON_AddNumberToObject(rectangle, "width", obs->rectangle.width);
                cJSON_AddNumberToObject(rectangle, "height", obs->rectangle.height);
                cJSON_AddItemToObject(obstacle, "rectangle", rectangle);
                
                cJSON *color = cJSON_CreateObject();
                cJSON_AddNumberToObject(color, "r", obs->color.r);
                cJSON_AddNumberToObject(color, "g", obs->color.g);
                cJSON_AddNumberToObject(color, "b", obs->color.b);
                cJSON_AddNumberToObject(color, "a", obs->color.a);
                cJSON_AddItemToObject(obstacle, "color", color);
                
                cJSON_AddItemToArray(obstacles, obstacle);
            }
        }
    }
    
    // Add background objects data
    cJSON *backgroundObjects = cJSON_CreateArray();
    cJSON_AddItemToObject(level, "backgroundObjects", backgroundObjects);
    
    if (game->backgroundObjects) {
        for (int i = 0; i < game->backgroundObjects->size; i++) {
            BackgroundObject *bgObj = (BackgroundObject*)dynamic_array_get(game->backgroundObjects, i);
            if (bgObj->sceneId == game->currentScene->id) {
                cJSON *backgroundObj = cJSON_CreateObject();
                cJSON_AddNumberToObject(backgroundObj, "id", bgObj->id);
                cJSON_AddNumberToObject(backgroundObj, "sceneId", bgObj->sceneId);
                cJSON_AddStringToObject(backgroundObj, "name", "editorBackground");
                
                cJSON *bounds = cJSON_CreateObject();
                cJSON_AddNumberToObject(bounds, "x", bgObj->bounds.x);
                cJSON_AddNumberToObject(bounds, "y", bgObj->bounds.y);
                cJSON_AddNumberToObject(bounds, "width", bgObj->bounds.width);
                cJSON_AddNumberToObject(bounds, "height", bgObj->bounds.height);
                cJSON_AddItemToObject(backgroundObj, "bounds", bounds);
                
                cJSON *color = cJSON_CreateObject();
                cJSON_AddNumberToObject(color, "r", bgObj->color.r);
                cJSON_AddNumberToObject(color, "g", bgObj->color.g);
                cJSON_AddNumberToObject(color, "b", bgObj->color.b);
                cJSON_AddNumberToObject(color, "a", bgObj->color.a);
                cJSON_AddItemToObject(backgroundObj, "color", color);
                
                cJSON_AddItemToArray(backgroundObjects, backgroundObj);
            }
        }
    }
    
    // Add lights data
    cJSON *lights = cJSON_CreateArray();
    cJSON_AddItemToObject(level, "lights", lights);
    
    if (game->lights) {
        for (int i = 0; i < game->lights->size; i++) {
            Light *light = (Light*)dynamic_array_get(game->lights, i);
            cJSON *lightObj = cJSON_CreateObject();
            
            cJSON *position = cJSON_CreateObject();
            cJSON_AddNumberToObject(position, "x", light->position.x);
            cJSON_AddNumberToObject(position, "y", light->position.y);
            cJSON_AddItemToObject(lightObj, "position", position);
            
            cJSON_AddNumberToObject(lightObj, "radius", light->radius);
            cJSON_AddNumberToObject(lightObj, "lightingRadius", light->lightingRadius);
            
            cJSON *color = cJSON_CreateObject();
            cJSON_AddNumberToObject(color, "r", light->color.r);
            cJSON_AddNumberToObject(color, "g", light->color.g);
            cJSON_AddNumberToObject(color, "b", light->color.b);
            cJSON_AddNumberToObject(color, "a", light->color.a);
            cJSON_AddItemToObject(lightObj, "color", color);
            
            cJSON_AddBoolToObject(lightObj, "flicker", light->flicker);
            cJSON_AddBoolToObject(lightObj, "breakable", light->breakable);
            cJSON_AddBoolToObject(lightObj, "isOn", light->isOn);
            
            cJSON_AddItemToArray(lights, lightObj);
        }
    }
    
    // Write JSON to file
    char *jsonString = cJSON_Print(root);
    if (jsonString) {
        FILE *file = fopen(filename, "w");
        if (file) {
            fprintf(file, "%s", jsonString);
            fclose(file);
            printf("Level saved successfully to %s\n", filename);
        } else {
            printf("Failed to open file %s for writing\n", filename);
        }
        free(jsonString);
    }
    
    // Clean up
    cJSON_Delete(root);
    
    return game;
}