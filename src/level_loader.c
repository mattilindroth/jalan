#include "level_loader.h"

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
    if (cJSON_IsArray(levels)) {
        int levelCount = cJSON_GetArraySize(levels);
        // Process each level in the array (for now, just load the first level)
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
                    
                    Enemy *enemy = createEnemy(id, sceneId, position, patrolA, patrolB, speed, hasLight);
                    game->enemy = enemy;
                }
            }
            
            // Load obstacles
            cJSON *obstaclesArray = cJSON_GetObjectItem(level, "obstacles");
            if (cJSON_IsArray(obstaclesArray)) {
                int obstacleCount = cJSON_GetArraySize(obstaclesArray);
                for (int o = 0; o < obstacleCount; o++) {
                    cJSON *obstacleJson = cJSON_GetArrayItem(obstaclesArray, o);
                    
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
                    Color color = {
                        cJSON_GetObjectItem(colorJson, "r")->valueint,
                        cJSON_GetObjectItem(colorJson, "g")->valueint,
                        cJSON_GetObjectItem(colorJson, "b")->valueint,
                        cJSON_GetObjectItem(colorJson, "a")->valueint
                    };
                    
                    Obstacle *obstacle = createObstacle(id, sceneId, rect, color);
                    addObstacleToGame(game, obstacle);
                }
            }
            
            // Load background objects
            cJSON *bgObjectsArray = cJSON_GetObjectItem(level, "backgroundObjects");
            if (cJSON_IsArray(bgObjectsArray)) {
                int bgObjectCount = cJSON_GetArraySize(bgObjectsArray);
                for (int b = 0; b < bgObjectCount; b++) {
                    cJSON *bgObjectJson = cJSON_GetArrayItem(bgObjectsArray, b);
                    
                    int id = cJSON_GetObjectItem(bgObjectJson, "id")->valueint;
                    int sceneId = cJSON_GetObjectItem(bgObjectJson, "sceneId")->valueint;
                    
                    cJSON *rectJson = cJSON_GetObjectItem(bgObjectJson, "rectangle");
                    Rectangle rect = {
                        cJSON_GetObjectItem(rectJson, "x")->valuedouble,
                        cJSON_GetObjectItem(rectJson, "y")->valuedouble,
                        cJSON_GetObjectItem(rectJson, "width")->valuedouble,
                        cJSON_GetObjectItem(rectJson, "height")->valuedouble
                    };
                    
                    cJSON *colorJson = cJSON_GetObjectItem(bgObjectJson, "color");
                    Color color = {
                        cJSON_GetObjectItem(colorJson, "r")->valueint,
                        cJSON_GetObjectItem(colorJson, "g")->valueint,
                        cJSON_GetObjectItem(colorJson, "b")->valueint,
                        cJSON_GetObjectItem(colorJson, "a")->valueint
                    };
                    
                    BackgroundObject *bgObject = createBackgroundObject(id, sceneId, rect, color);
                    addBackgroundObjectToGame(game, bgObject);
                }
            }
        }
    }

    cJSON_Delete(json);
    return game;
}