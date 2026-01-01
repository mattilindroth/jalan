#include <raylib.h>
#include <core/engine.h>

int main(void) {
    // Initialize raylib
    const int screenWidth = 1024;
    const int screenHeight = 768;
    InitWindow(screenWidth, screenHeight, "Jalanallas - 2D Platformer");
    SetTargetFPS(60);
    
    // Initialize engine
    engine_init();
    
    // Game variables
    Vector2 playerPos = { screenWidth/2.0f, screenHeight/2.0f };
    
    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        if (IsKeyDown(KEY_RIGHT)) playerPos.x += 200.0f * GetFrameTime();
        if (IsKeyDown(KEY_LEFT)) playerPos.x -= 200.0f * GetFrameTime();
        if (IsKeyDown(KEY_UP)) playerPos.y -= 200.0f * GetFrameTime();
        if (IsKeyDown(KEY_DOWN)) playerPos.y += 200.0f * GetFrameTime();
        
        // Draw
        BeginDrawing();
        ClearBackground(SKYBLUE);
        
        DrawText("Jalanallas - 2D Platformer", 10, 10, 32, DARKBLUE);
        DrawText("Use arrow keys to move the square", 10, 50, 20, DARKGRAY);
        
        DrawRectangle((int)playerPos.x - 25, (int)playerPos.y - 25, 50, 50, RED);
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}