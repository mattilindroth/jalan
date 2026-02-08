#include "game.h"
#include "level_loader.h"
#include <math.h>

Game* createGame(Scene* initialScene, enum GameState initialState) {
    Game* game = (Game*)malloc(sizeof(Game));
    if (game != NULL) {
        game->currentScene = initialScene;
        game->state = initialState;
        game->obstacles = dynamic_array_create_default();
        game->backgroundObjects = dynamic_array_create_default();
        game->lights = dynamic_array_create_default();
        game->player = NULL;
        game->enemy = NULL;
        game->editorGrid = createGrid(32.0f);  // 32 pixel grid
        
        // Initialize editor state
        game->selectedObstacle = NULL;
        game->selectedBgObj = NULL;
        game->selectedLight = NULL;
        game->editorMode = 0;  // 0=obstacles, 1=background objects, 2=lights
        game->currentColorIndex = 0;
        game->dragMode = 0;  // 0=none, 1=move, 2=resize
        game->dragStartPos = (Vector2){0, 0};
        game->dragStartObjPos = (Vector2){0, 0};
        game->dragStartObjSize = (Vector2){0, 0};
        
        // Initialize camera with default values (will be updated later)
        game->camera.target = (Vector2){0.0f, 0.0f};
        game->camera.offset = (Vector2){960.0f, 600.0f}; // Default screen center
        game->camera.rotation = 0.0f;
        game->camera.zoom = 1.0f;
    }
    return game;
}

void addObstacleToGame(Game* game, Obstacle* obstacle) {
    if(game != NULL && obstacle != NULL) {
        dynamic_array_push(game->obstacles, obstacle);
    }
}

void addBackgroundObjectToGame(Game* game, BackgroundObject* backgroundObject) {
    if(game != NULL && backgroundObject != NULL) {
        dynamic_array_push(game->backgroundObjects, backgroundObject);
    }
}

void addLightToGame(Game* game, Light* light) {
    if(game != NULL && light != NULL) {
        dynamic_array_push(game->lights, light);
    }
}

void resolveCollisions(Game* game) {
    if (!game || !game->player || !game->obstacles) return;

    int sceneId = game->currentScene->id;
    int onGround = 0;

    // First, handle active collisions (when player is overlapping with obstacles)
    for (int i = 0; i < game->obstacles->size; i++) {
        Obstacle* obs = (Obstacle*)dynamic_array_get(game->obstacles, i);
        
        // Filter obstacles by scene
        if (obs->sceneId != sceneId) continue;

        // Update playerRect with current position for accurate collision detection
        Rectangle playerRect = { 
            game->player->position.x, 
            game->player->position.y, 
            game->player->boundingBox.width, 
            game->player->boundingBox.height 
        };

        enum CollisionType col = checkCollisionWithObstacle(playerRect, obs);
        
        if (col != COLLISION_NONE) {
            // Resolve collision based on the side of impact
            if (col == COLLISION_TOP) {
                // Landed on TOP
                game->player->position.y = obs->rectangle.y - game->player->boundingBox.height;
                game->player->speed.y = 0;
                onGround = 1;
            } else if (col == COLLISION_BOTTOM) {
                // Hit BOTTOM (Ceiling)
                game->player->position.y = obs->rectangle.y + obs->rectangle.height;
                game->player->speed.y = 0;
            } else if (col == COLLISION_LEFT) {
                game->player->position.x = obs->rectangle.x - game->player->boundingBox.width;
                game->player->speed.x = 0;
            } else if (col == COLLISION_RIGHT) {
                game->player->position.x = obs->rectangle.x + obs->rectangle.width;
                game->player->speed.x = 0;
            }
        }
    }

    // Second, check if player is standing on ground (even without collision)
    // Create a small rectangle just below the player's feet to check for ground support
    Rectangle groundCheckRect = {
        game->player->position.x + 2,  // Slightly inset to avoid edge issues
        game->player->position.y + game->player->boundingBox.height,
        game->player->boundingBox.width - 4,  // Slightly narrower
        2  // Small height to check just below feet
    };

    for (int i = 0; i < game->obstacles->size; i++) {
        Obstacle* obs = (Obstacle*)dynamic_array_get(game->obstacles, i);
        
        // Filter obstacles by scene
        if (obs->sceneId != sceneId) continue;

        // Check if the ground check rectangle overlaps with any obstacle
        if (CheckCollisionRecs(groundCheckRect, obs->rectangle)) {
            onGround = 1;
            break;
        }
    }

    // Update state based on ground contact
    if (onGround) {
        if (game->player->state == PLAYER_AIR) {
            game->player->state = PLAYER_NORMAL;
        }
    } else {
        // Player is not on ground, should be falling
        if (game->player->state != PLAYER_AIR) {
            game->player->state = PLAYER_AIR;
        }
    }
}

int updateGame(Game* game) {
    // Implement game update logic here

    if(game->state == STATE_EDITOR) {
        return 0; // Skip updates in editor mode
    }
    
    int sceneId = game->currentScene->id;

    if(game->state == STATE_STORY) {

    } else {

        updatePlayer(game->player); 

        updateEnemy(game->enemy, getPlayerPosition(game->player));

        resolveCollisions(game);
    }
    return 0;
}

int handleEditorInput(Game *game) {

    if(IsKeyPressed(KEY_ESCAPE)) {
        game->state = STATE_PLAYING;
        game->selectedObstacle = NULL;  // Clear selections
        game->selectedBgObj = NULL;
        game->selectedLight = NULL;
        game->dragMode = 0;
        return 0;
    }

    // Mode switching (Tab key)
    if (IsKeyPressed(KEY_TAB)) {
        game->editorMode = (game->editorMode + 1) % 3;  // Toggle between 0, 1, and 2
        game->selectedObstacle = NULL;  // Clear selections when switching modes
        game->selectedBgObj = NULL;
        game->selectedLight = NULL;
        game->dragMode = 0;
    }

    // Color cycling (C key)
    if (IsKeyPressed(KEY_C)) {
        game->currentColorIndex = (game->currentColorIndex + 1) % getEditorColorCount();
        
        // Apply color to selected object
        if (game->editorMode == 0 && game->selectedObstacle != NULL) {
            game->selectedObstacle->color = getEditorColor(game->currentColorIndex);
        } else if (game->editorMode == 1 && game->selectedBgObj != NULL) {
            game->selectedBgObj->color = getEditorColor(game->currentColorIndex);
        } else if (game->editorMode == 2 && game->selectedLight != NULL) {
            game->selectedLight->color = getEditorColor(game->currentColorIndex);
        }
    }

    // Save level (Shift + S)
    if ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) && IsKeyPressed(KEY_S)) {
        saveLevel("levels.json", game);
    }

    // Grid controls
    if (IsKeyPressed(KEY_G)) {
        setGridVisible(game->editorGrid, !game->editorGrid->visible);
    }
    
    // Grid size adjustment
    if (IsKeyPressed(KEY_EQUAL) || IsKeyPressed(KEY_KP_ADD)) {
        float newSize = game->editorGrid->cellSize * 2.0f;
        if (newSize <= 128.0f) setGridCellSize(game->editorGrid, newSize);
    }
    if (IsKeyPressed(KEY_MINUS) || IsKeyPressed(KEY_KP_SUBTRACT)) {
        float newSize = game->editorGrid->cellSize / 2.0f;
        if (newSize >= 8.0f) setGridCellSize(game->editorGrid, newSize);
    }

    // Delete selected object
    if (IsKeyPressed(KEY_DELETE)) {
        if (game->editorMode == 0 && game->selectedObstacle != NULL) {
            removeObstacleFromGame(game, game->selectedObstacle);
            game->selectedObstacle = NULL;
        } else if (game->editorMode == 1 && game->selectedBgObj != NULL) {
            removeBackgroundObjectFromGame(game, game->selectedBgObj);
            game->selectedBgObj = NULL;
        } else if (game->editorMode == 2 && game->selectedLight != NULL) {
            removeLightFromGame(game, game->selectedLight);
            game->selectedLight = NULL;
        }
        game->dragMode = 0;
    }

    // Light property editing (only in light mode with selected light)
    if (game->editorMode == 2 && game->selectedLight != NULL) {
        // Toggle flicker with F key
        if (IsKeyPressed(KEY_F)) {
            game->selectedLight->flicker = !game->selectedLight->flicker;
            if (!game->selectedLight->flicker) {
                game->selectedLight->isOn = true; // Turn on when disabling flicker
            }
        }
        
        // Toggle breakable with B key
        if (IsKeyPressed(KEY_B)) {
            game->selectedLight->breakable = !game->selectedLight->breakable;
        }
        
        // Adjust radius with R key + mouse wheel or +/- keys
        if (IsKeyDown(KEY_R)) {
            float mouseWheelMove = GetMouseWheelMove();
            if (mouseWheelMove != 0.0f) {
                game->selectedLight->radius += mouseWheelMove * 5.0f;
                game->selectedLight->radius = fmaxf(5.0f, fminf(100.0f, game->selectedLight->radius));
            }
            if (IsKeyPressed(KEY_EQUAL) || IsKeyPressed(KEY_KP_ADD)) {
                game->selectedLight->radius += 5.0f;
                game->selectedLight->radius = fminf(100.0f, game->selectedLight->radius);
            }
            if (IsKeyPressed(KEY_MINUS) || IsKeyPressed(KEY_KP_SUBTRACT)) {
                game->selectedLight->radius -= 5.0f;
                game->selectedLight->radius = fmaxf(5.0f, game->selectedLight->radius);
            }
        }
        
        // Adjust lighting radius with L key + mouse wheel or +/- keys  
        if (IsKeyDown(KEY_L)) {
            float mouseWheelMove = GetMouseWheelMove();
            if (mouseWheelMove != 0.0f) {
                game->selectedLight->lightingRadius += mouseWheelMove * 10.0f;
                game->selectedLight->lightingRadius = fmaxf(20.0f, fminf(300.0f, game->selectedLight->lightingRadius));
            }
            if (IsKeyPressed(KEY_EQUAL) || IsKeyPressed(KEY_KP_ADD)) {
                game->selectedLight->lightingRadius += 10.0f;
                game->selectedLight->lightingRadius = fminf(300.0f, game->selectedLight->lightingRadius);
            }
            if (IsKeyPressed(KEY_MINUS) || IsKeyPressed(KEY_KP_SUBTRACT)) {
                game->selectedLight->lightingRadius -= 10.0f;
                game->selectedLight->lightingRadius = fmaxf(20.0f, game->selectedLight->lightingRadius);
            }
        }
    }

    // Camera movement in editor
    float cameraSpeed = 300.0f * GetFrameTime();
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        game->camera.target.x -= cameraSpeed;
    }
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        game->camera.target.x += cameraSpeed;
    }
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
        game->camera.target.y -= cameraSpeed;
    }
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
        game->camera.target.y += cameraSpeed;
    }

    // Mouse interaction
    Vector2 mousePos = GetMousePosition();
    Vector2 mouseWorldPos = GetScreenToWorld2D(mousePos, game->camera);
    
    // Handle mouse button press
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (game->editorMode == 0) {  // Obstacle mode
            Obstacle* clickedObstacle = findObstacleAtPosition(game, mouseWorldPos);
            
            if (clickedObstacle != NULL) {
                // Check if clicking on resize corner
                if (isInResizeCorner(clickedObstacle->rectangle, mouseWorldPos, 16.0f)) {
                    game->selectedObstacle = clickedObstacle;
                    game->selectedBgObj = NULL;
                    game->selectedLight = NULL;
                    game->dragMode = 2; // Resize mode
                    game->dragStartPos = mouseWorldPos;
                    game->dragStartObjPos = (Vector2){clickedObstacle->rectangle.x, clickedObstacle->rectangle.y};
                    game->dragStartObjSize = (Vector2){clickedObstacle->rectangle.width, clickedObstacle->rectangle.height};
                } else {
                    // Select and start moving
                    game->selectedObstacle = clickedObstacle;
                    game->selectedBgObj = NULL;
                    game->selectedLight = NULL;
                    game->dragMode = 1; // Move mode
                    game->dragStartPos = mouseWorldPos;
                    game->dragStartObjPos = (Vector2){clickedObstacle->rectangle.x, clickedObstacle->rectangle.y};
                }
            } else {
                // Create new obstacle
                Obstacle* newObs = createObstacleAtPosition(game, mouseWorldPos);
                newObs->color = getEditorColor(game->currentColorIndex);
                game->selectedObstacle = newObs;
                game->selectedBgObj = NULL;
                game->selectedLight = NULL;
                game->dragMode = 0;
            }
        } else if (game->editorMode == 1) {  // Background object mode
            BackgroundObject* clickedBgObj = findBackgroundObjectAtPosition(game, mouseWorldPos);
            
            if (clickedBgObj != NULL) {
                // Check if clicking on resize corner
                if (isInResizeCorner(clickedBgObj->bounds, mouseWorldPos, 16.0f)) {
                    game->selectedBgObj = clickedBgObj;
                    game->selectedObstacle = NULL;
                    game->selectedLight = NULL;
                    game->dragMode = 2; // Resize mode
                    game->dragStartPos = mouseWorldPos;
                    game->dragStartObjPos = (Vector2){clickedBgObj->bounds.x, clickedBgObj->bounds.y};
                    game->dragStartObjSize = (Vector2){clickedBgObj->bounds.width, clickedBgObj->bounds.height};
                } else {
                    // Select and start moving
                    game->selectedBgObj = clickedBgObj;
                    game->selectedObstacle = NULL;
                    game->selectedLight = NULL;
                    game->dragMode = 1; // Move mode
                    game->dragStartPos = mouseWorldPos;
                    game->dragStartObjPos = (Vector2){clickedBgObj->bounds.x, clickedBgObj->bounds.y};
                }
            } else {
                // Create new background object
                BackgroundObject* newBgObj = createBackgroundObjectAtPosition(game, mouseWorldPos);
                game->selectedBgObj = newBgObj;
                game->selectedObstacle = NULL;
                game->selectedLight = NULL;
                game->dragMode = 0;
            }
        } else {  // Light mode (editorMode == 2)
            Light* clickedLight = findLightAtPosition(game, mouseWorldPos);
            
            if (clickedLight != NULL) {
                // Lights don't have resize corners, just selection and movement
                game->selectedLight = clickedLight;
                game->selectedObstacle = NULL;
                game->selectedBgObj = NULL;
                game->dragMode = 1; // Move mode
                game->dragStartPos = mouseWorldPos;
                game->dragStartObjPos = (Vector2){clickedLight->position.x, clickedLight->position.y};
            } else {
                // Create new light
                Light* newLight = createLightAtPosition(game, mouseWorldPos);
                game->selectedLight = newLight;
                game->selectedObstacle = NULL;
                game->selectedBgObj = NULL;
                game->dragMode = 0;
            }
        }
    }
    
    // Handle mouse drag
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && game->dragMode > 0) {
        Vector2 deltaPos = {mouseWorldPos.x - game->dragStartPos.x, mouseWorldPos.y - game->dragStartPos.y};
        
        if (game->dragMode == 1) { // Move mode
            Vector2 newPos = {game->dragStartObjPos.x + deltaPos.x, game->dragStartObjPos.y + deltaPos.y};
            Vector2 snappedPos = snapToGrid(game->editorGrid, newPos);
            
            if (game->selectedObstacle != NULL) {
                game->selectedObstacle->rectangle.x = snappedPos.x;
                game->selectedObstacle->rectangle.y = snappedPos.y;
            } else if (game->selectedBgObj != NULL) {
                game->selectedBgObj->bounds.x = snappedPos.x;
                game->selectedBgObj->bounds.y = snappedPos.y;
            } else if (game->selectedLight != NULL) {
                game->selectedLight->position.x = snappedPos.x;
                game->selectedLight->position.y = snappedPos.y;
            }
        } else if (game->dragMode == 2) { // Resize mode
            float newWidth = game->dragStartObjSize.x + deltaPos.x;
            float newHeight = game->dragStartObjSize.y + deltaPos.y;
            
            // Minimum size constraints
            newWidth = fmaxf(newWidth, game->editorGrid->cellSize);
            newHeight = fmaxf(newHeight, game->editorGrid->cellSize);
            
            // Snap to grid
            newWidth = roundf(newWidth / game->editorGrid->cellSize) * game->editorGrid->cellSize;
            newHeight = roundf(newHeight / game->editorGrid->cellSize) * game->editorGrid->cellSize;
            
            if (game->selectedObstacle != NULL) {
                game->selectedObstacle->rectangle.width = newWidth;
                game->selectedObstacle->rectangle.height = newHeight;
            } else if (game->selectedBgObj != NULL) {
                game->selectedBgObj->bounds.width = newWidth;
                game->selectedBgObj->bounds.height = newHeight;
            }
        }
    }
    
    // Stop dragging on mouse release
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        game->dragMode = 0;
    }
    
    // Deselect with right click
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        game->selectedObstacle = NULL;
        game->selectedBgObj = NULL;
        game->selectedLight = NULL;
        game->dragMode = 0;
    }

    return 0;

}

int handleInput(Game* game) {
    
    // Exit game with Ctrl+Q or Alt+F4
    if ((IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Q)) || 
        (IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_F4))) {
        // Signal to close the window
        return -1;  // We'll handle this return value in main
    }
    
    // Game level design mode.
    if(game->state != STATE_EDITOR) {
        if(IsKeyPressed(KEY_F1)) {
            game->state = STATE_EDITOR;
        }
        // Exit game with ESC when in playing mode
        if(IsKeyPressed(KEY_ESCAPE)) {
            return -1;  // Signal to exit game
        }
    } 
    if(game->state == STATE_EDITOR) {
        handleEditorInput(game);
        return 0;
    }

    // Normal gameplay input handling
    if (game->state != STATE_STORY) {
        // Implement input handling logic here
        if(game->player->state != PLAYER_AIR) {
            game->player->speed.y = 0; // Reset vertical speed if on ground

            if(IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
                if(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
                    game->player->speed.x = PLAYER_SNEAK_SPEED;
                    game->player->state = PLAYER_SNEAK;
                } else {
                    game->player->speed.x = PLAYER_RUNNING_SPEED;
                    game->player->state = PLAYER_RUNNING;            
                }
            } else if(IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
                if(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
                    game->player->speed.x = -PLAYER_SNEAK_SPEED;
                    game->player->state = PLAYER_SNEAK;            
                } else {
                    game->player->speed.x = -PLAYER_RUNNING_SPEED;
                    game->player->state = PLAYER_RUNNING;
                }
            } else {
                game->player->speed.x = 0;
            }
        }
        if(IsKeyPressed(KEY_SPACE)) {
            // Jump only if on ground
            if(game->player->state != PLAYER_AIR) {
                game->player->speed.y = -10.0f; // Jump impulse
                game->player->state = PLAYER_AIR;
            }
        }
    } else {
        // Story mode input handling (if any)
        // Check if any key was pressed to exit story mode
        if (GetKeyPressed() != 0) {
            game->state = STATE_PLAYING;
        }
    }

    return 0;
}

int updateCamera(Game* game) {
    if (!game) return -1;
    
    // In gameplay mode, follow the player
    if (game->state == STATE_PLAYING && game->player != NULL) {
        game->camera.target.x = game->player->position.x + (game->player->boundingBox.width / 2.0f);
        game->camera.target.y = game->player->position.y + (game->player->boundingBox.height / 2.0f);
    }
    
    // In editor mode, camera movement is handled in handleEditorInput
    // Other states can have their own camera logic here
    
    return 0;
}

int renderGame(Game* game) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    
    BeginMode2D(game->camera);

    //Render backgroud objects
    for(int i = 0; i < game->backgroundObjects->size; i++) {
        BackgroundObject* bgObj = (BackgroundObject*)dynamic_array_get(game->backgroundObjects, i);
        if(bgObj->sceneId == game->currentScene->id) {
            renderBackgroundObject(bgObj);
            
            // Highlight selected background object in editor mode
            if (game->state == STATE_EDITOR && bgObj == game->selectedBgObj) {
                DrawRectangleLinesEx(bgObj->bounds, 3.0f, ORANGE);
                
                // Draw resize corner handle
                Rectangle resizeCorner = {
                    bgObj->bounds.x + bgObj->bounds.width - 16.0f,
                    bgObj->bounds.y + bgObj->bounds.height - 16.0f,
                    16.0f,
                    16.0f
                };
                DrawRectangleRec(resizeCorner, ORANGE);
                DrawRectangleLinesEx(resizeCorner, 1.0f, BLACK);
            }
        }
    }

    //Render player
    renderPlayer(game->player);  

    //Render obstacles
    for(int i = 0; i < game->obstacles->size; i++) {
        Obstacle* obs = (Obstacle*)dynamic_array_get(game->obstacles, i);
        if(obs->sceneId == game->currentScene->id) {
            DrawRectangleRec(obs->rectangle, obs->color);
            
            // Highlight selected obstacle in editor mode
            if (game->state == STATE_EDITOR && obs == game->selectedObstacle) {
                DrawRectangleLinesEx(obs->rectangle, 3.0f, YELLOW);
                
                // Draw resize corner handle
                Rectangle resizeCorner = {
                    obs->rectangle.x + obs->rectangle.width - 16.0f,
                    obs->rectangle.y + obs->rectangle.height - 16.0f,
                    16.0f,
                    16.0f
                };
                DrawRectangleRec(resizeCorner, YELLOW);
                DrawRectangleLinesEx(resizeCorner, 1.0f, BLACK);
            }
        }
    }   

    //Render lights
    if (game->lights) {
        for(int i = 0; i < game->lights->size; i++) {
            Light* light = (Light*)dynamic_array_get(game->lights, i);
            if (light) {
                // Update light (for flicker effect)
                updateLight(light);
                
                // Render light
                renderLight(light);
                
                // In editor mode, show light properties visually
                if (game->state == STATE_EDITOR) {
                    // Draw light radius as a circle outline
                    DrawCircleLines(light->position.x, light->position.y, light->radius, 
                                  (Color){light->color.r, light->color.g, light->color.b, 100});
                    
                    // Draw lighting radius as a larger, fainter circle
                    DrawCircleLines(light->position.x, light->position.y, light->lightingRadius, 
                                  (Color){light->color.r, light->color.g, light->color.b, 50});
                    
                    // Highlight selected light
                    if (light == game->selectedLight) {
                        DrawCircleLines(light->position.x, light->position.y, light->radius + 5, RED);
                        
                        // Draw selection indicator
                        DrawCircleV(light->position, 3.0f, RED);
                        
                        // Draw property indicators
                        if (light->flicker) {
                            DrawText("F", light->position.x - 20, light->position.y - light->radius - 20, 16, PURPLE);
                        }
                        if (light->breakable) {
                            DrawText("B", light->position.x + 10, light->position.y - light->radius - 20, 16, ORANGE);
                        }
                    }
                }
            }
        }
    }

    // Render grid when in editor mode
    if (game->state == STATE_EDITOR && game->editorGrid != NULL) {
        // Calculate view bounds based on camera
        Rectangle viewBounds = {
            game->camera.target.x - (GetScreenWidth() / 2.0f) / game->camera.zoom,
            game->camera.target.y - (GetScreenHeight() / 2.0f) / game->camera.zoom,
            GetScreenWidth() / game->camera.zoom,
            GetScreenHeight() / game->camera.zoom
        };
        renderGrid(game->editorGrid, viewBounds);
    }

    EndMode2D();
    
    // Draw UI elements (not affected by camera)
    if (game->state == STATE_EDITOR) {
        DrawText("EDITOR MODE - Press ESC to return to game", 10, 10, 20, RED);
        if (game->editorGrid != NULL) {
            const char* modeText;
            Color modeColor;
            if (game->editorMode == 0) {
                modeText = "OBSTACLES";
                modeColor = YELLOW;
            } else if (game->editorMode == 1) {
                modeText = "BACKGROUND OBJECTS";
                modeColor = ORANGE;
            } else {
                modeText = "LIGHTS";
                modeColor = PURPLE;
            }
            DrawText(TextFormat("Mode: %s (TAB to switch)", modeText), 10, 40, 18, modeColor);
            
            Color currentColor = getEditorColor(game->currentColorIndex);
            DrawText("Current Color: ", 10, 65, 16, DARKGRAY);
            DrawRectangle(130, 65, 20, 16, currentColor);
            DrawRectangleLines(130, 65, 20, 16, BLACK);
            DrawText("(C to cycle)", 155, 65, 16, DARKGRAY);
            
            DrawText(TextFormat("Grid Size: %.0fpx", game->editorGrid->cellSize), 10, 90, 16, DARKGRAY);
            DrawText("Controls: G=Grid, +/-=Grid Size, WASD=Camera, C=Color", 10, 115, 16, DARKGRAY);
            DrawText("Mouse: Left=Select/Create, Drag=Move, Corner=Resize, Right=Deselect", 10, 140, 16, DARKGRAY);
            DrawText("Keys: TAB=Switch Mode, DEL=Delete Selected, Shift+S=Save", 10, 165, 16, DARKGRAY);
            if (game->editorMode == 2) {
                DrawText("Light Controls: F=Flicker, B=Breakable, R+Wheel/±=Radius, L+Wheel/±=Lighting", 10, 190, 16, PURPLE);
            }
            
            int yOffset = (game->editorMode == 2) ? 215 : 190;
            if (game->selectedObstacle != NULL) {
                DrawText(TextFormat("Selected Obstacle: ID=%d, Pos=(%.0f,%.0f), Size=(%.0fx%.0f)", 
                    game->selectedObstacle->id,
                    game->selectedObstacle->rectangle.x,
                    game->selectedObstacle->rectangle.y,
                    game->selectedObstacle->rectangle.width,
                    game->selectedObstacle->rectangle.height), 10, yOffset, 16, YELLOW);
            } else if (game->selectedBgObj != NULL) {
                DrawText(TextFormat("Selected Background: ID=%d, Pos=(%.0f,%.0f), Size=(%.0fx%.0f)", 
                    game->selectedBgObj->id,
                    game->selectedBgObj->bounds.x,
                    game->selectedBgObj->bounds.y,
                    game->selectedBgObj->bounds.width,
                    game->selectedBgObj->bounds.height), 10, yOffset, 16, ORANGE);
            } else if (game->selectedLight != NULL) {
                DrawText(TextFormat("Selected Light: Pos=(%.0f,%.0f), Radius=%.0f, Lighting=%.0f", 
                    game->selectedLight->position.x,
                    game->selectedLight->position.y,
                    game->selectedLight->radius,
                    game->selectedLight->lightingRadius), 10, yOffset, 16, PURPLE);
                DrawText(TextFormat("Properties: Flicker=%s, Breakable=%s", 
                    game->selectedLight->flicker ? "ON" : "OFF",
                    game->selectedLight->breakable ? "ON" : "OFF"), 10, yOffset + 20, 16, PURPLE);
            }
        }
    }

    EndDrawing();
    return 0;
}

void destroyGame(Game* game) {
    if (game != NULL) {
        if (game->player != NULL) {
            destroyPlayer(game->player);
        }
        if (game->enemy != NULL) {
            destroyEnemy(game->enemy);
        }
        if (game->obstacles != NULL) {
            for (int i = 0; i < game->obstacles->size; i++) {
                Obstacle* obs = (Obstacle*)dynamic_array_get(game->obstacles, i);
                destroyObstacle(obs);
            }
            dynamic_array_destroy(game->obstacles);
        }
        if (game->backgroundObjects != NULL) {
            for (int i = 0; i < game->backgroundObjects->size; i++) {
                BackgroundObject* bgObj = (BackgroundObject*)dynamic_array_get(game->backgroundObjects, i);
                destroyBackgroundObject(bgObj);
            }
            dynamic_array_destroy(game->backgroundObjects);
        }
        if (game->lights != NULL) {
            for (int i = 0; i < game->lights->size; i++) {
                Light* light = (Light*)dynamic_array_get(game->lights, i);
                destroyLight(light);
            }
            dynamic_array_destroy(game->lights);
        }
        if (game->editorGrid != NULL) {
            destroyGrid(game->editorGrid);
        }
        free(game);
    }
}

// Editor helper functions
Obstacle* findObstacleAtPosition(Game* game, Vector2 worldPos) {
    if (!game || !game->obstacles) return NULL;
    
    int sceneId = game->currentScene->id;
    
    // Search from top to bottom (last drawn = topmost)
    for (int i = game->obstacles->size - 1; i >= 0; i--) {
        Obstacle* obs = (Obstacle*)dynamic_array_get(game->obstacles, i);
        if (obs->sceneId != sceneId) continue;
        
        if (CheckCollisionPointRec(worldPos, obs->rectangle)) {
            return obs;
        }
    }
    return NULL;
}

Obstacle* createObstacleAtPosition(Game* game, Vector2 worldPos) {
    if (!game) return NULL;
    
    Vector2 snappedPos = snapToGrid(game->editorGrid, worldPos);
    Rectangle rect = {snappedPos.x, snappedPos.y, game->editorGrid->cellSize, game->editorGrid->cellSize};
    
    // Find next available ID
    int maxId = 0;
    for (int i = 0; i < game->obstacles->size; i++) {
        Obstacle* obs = (Obstacle*)dynamic_array_get(game->obstacles, i);
        if (obs->id > maxId) maxId = obs->id;
    }
    
    Obstacle* newObs = createObstacle(maxId + 1, game->currentScene->id, rect, BLUE);
    addObstacleToGame(game, newObs);
    return newObs;
}

bool isInResizeCorner(Rectangle rect, Vector2 worldPos, float cornerSize) {
    Rectangle corner = {
        rect.x + rect.width - cornerSize,
        rect.y + rect.height - cornerSize,
        cornerSize,
        cornerSize
    };
    return CheckCollisionPointRec(worldPos, corner);
}

void removeObstacleFromGame(Game* game, Obstacle* obstacle) {
    if (!game || !game->obstacles || !obstacle) return;
    
    for (int i = 0; i < game->obstacles->size; i++) {
        Obstacle* obs = (Obstacle*)dynamic_array_get(game->obstacles, i);
        if (obs == obstacle) {
            destroyObstacle(obs);
            dynamic_array_remove(game->obstacles, i);
            break;
        }
    }
}

// Define editor color palette using Raylib colors
Color getEditorColor(int colorIndex) {
    static Color colors[] = {
        BLUE, RED, GREEN, YELLOW, ORANGE, PURPLE, PINK, LIME,
        DARKBLUE, MAROON, DARKGREEN, GOLD, VIOLET, SKYBLUE,
        BROWN, DARKBROWN, BEIGE, LIGHTGRAY, GRAY, DARKGRAY
    };
    int colorCount = sizeof(colors) / sizeof(colors[0]);
    return colors[colorIndex % colorCount];
}

int getEditorColorCount(void) {
    return 20;  // Number of colors in our palette
}

BackgroundObject* findBackgroundObjectAtPosition(Game* game, Vector2 worldPos) {
    if (!game || !game->backgroundObjects) return NULL;
    
    int sceneId = game->currentScene->id;
    
    // Search from top to bottom (last drawn = topmost)
    for (int i = game->backgroundObjects->size - 1; i >= 0; i--) {
        BackgroundObject* bgObj = (BackgroundObject*)dynamic_array_get(game->backgroundObjects, i);
        if (bgObj->sceneId != sceneId) continue;
        
        if (CheckCollisionPointRec(worldPos, bgObj->bounds)) {
            return bgObj;
        }
    }
    return NULL;
}

BackgroundObject* createBackgroundObjectAtPosition(Game* game, Vector2 worldPos) {
    if (!game) return NULL;
    
    Vector2 snappedPos = snapToGrid(game->editorGrid, worldPos);
    Rectangle bounds = {snappedPos.x, snappedPos.y, game->editorGrid->cellSize * 2, game->editorGrid->cellSize * 2};
    
    // Find next available ID
    int maxId = 0;
    for (int i = 0; i < game->backgroundObjects->size; i++) {
        BackgroundObject* bgObj = (BackgroundObject*)dynamic_array_get(game->backgroundObjects, i);
        if (bgObj->id > maxId) maxId = bgObj->id;
    }
    
    Color currentColor = getEditorColor(game->currentColorIndex);
    BackgroundObject* newBgObj = createBackgroundObject(maxId + 1, game->currentScene->id, bounds, currentColor);
    addBackgroundObjectToGame(game, newBgObj);
    return newBgObj;
}

void removeBackgroundObjectFromGame(Game* game, BackgroundObject* bgObj) {
    if (!game || !game->backgroundObjects || !bgObj) return;
    
    for (int i = 0; i < game->backgroundObjects->size; i++) {
        BackgroundObject* obj = (BackgroundObject*)dynamic_array_get(game->backgroundObjects, i);
        if (obj == bgObj) {
            destroyBackgroundObject(obj);
            dynamic_array_remove(game->backgroundObjects, i);
            break;
        }
    }
}

Light* findLightAtPosition(Game* game, Vector2 worldPos) {
    if (!game || !game->lights) return NULL;
    
    for (int i = 0; i < game->lights->size; i++) {
        Light* light = (Light*)dynamic_array_get(game->lights, i);
        Vector2 lightPos = light->position;
        float distance = sqrt((worldPos.x - lightPos.x) * (worldPos.x - lightPos.x) + 
                             (worldPos.y - lightPos.y) * (worldPos.y - lightPos.y));
        if (distance <= light->radius) {
            return light;
        }
    }
    return NULL;
}

Light* createLightAtPosition(Game* game, Vector2 worldPos) {
    if (!game) return NULL;
    
    // Snap to grid
    Vector2 snappedPos = snapToGrid(game->editorGrid, worldPos);
    
    // Create light with default properties
    Color lightColor = getEditorColor(game->currentColorIndex);
    Light* newLight = createLight(snappedPos, 20.0f, 100.0f, lightColor, false, false, true);
    if (newLight) {
        newLight->isOn = true;
        newLight->flickerTimer = 0.0f;
    }
    
    addLightToGame(game, newLight);
    return newLight;
}

void removeLightFromGame(Game* game, Light* light) {
    if (!game || !game->lights || !light) return;
    
    for (int i = 0; i < game->lights->size; i++) {
        Light* obj = (Light*)dynamic_array_get(game->lights, i);
        if (obj == light) {
            destroyLight(obj);
            dynamic_array_remove(game->lights, i);
            break;
        }
    }
}