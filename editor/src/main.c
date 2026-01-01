#include <raylib.h>
#include <core/engine.h>
#include "editor_layout.h"

int main(void) {
    // Initialize raylib
    const int screenWidth = 1280;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "Jalan Engine Editor");
    SetTargetFPS(60);
    
    // Initialize engine
    engine_init();
    
    // Create editor layout
    EditorLayout* editor = editor_layout_create(screenWidth, screenHeight);
    
    // Main editor loop
    while (!WindowShouldClose()) {
        float delta_time = GetFrameTime();
        
        // Update
        editor_layout_update(editor, delta_time);
        
        // Render
        BeginDrawing();
        ClearBackground(DARKGRAY);
        
        editor_layout_render(editor);
        
        EndDrawing();
    }
    
    // Cleanup
    editor_layout_destroy(editor);
    CloseWindow();
    return 0;
}