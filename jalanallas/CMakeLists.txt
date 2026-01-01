# Jalanallas Game CMakeLists.txt

# Collect all source files
file(GLOB_RECURSE GAME_SOURCES 
    "src/*.c"
    "include/*.h"
)

# If no sources exist yet, create a dummy main file
if(NOT GAME_SOURCES)
    set(GAME_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c")
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c")
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src")
        file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c" 
            "#include <raylib.h>\n#include <engine.h>\n\nint main(void) {\n    // Initialize raylib\n    const int screenWidth = 1024;\n    const int screenHeight = 768;\n    InitWindow(screenWidth, screenHeight, \"Jalanallas - 2D Platformer\");\n    SetTargetFPS(60);\n    \n    // Initialize engine\n    engine_init();\n    \n    // Game variables\n    Vector2 playerPos = { screenWidth/2.0f, screenHeight/2.0f };\n    \n    // Main game loop\n    while (!WindowShouldClose()) {\n        // Update\n        if (IsKeyDown(KEY_RIGHT)) playerPos.x += 200.0f * GetFrameTime();\n        if (IsKeyDown(KEY_LEFT)) playerPos.x -= 200.0f * GetFrameTime();\n        if (IsKeyDown(KEY_UP)) playerPos.y -= 200.0f * GetFrameTime();\n        if (IsKeyDown(KEY_DOWN)) playerPos.y += 200.0f * GetFrameTime();\n        \n        // Draw\n        BeginDrawing();\n        ClearBackground(SKYBLUE);\n        \n        DrawText(\"Jalanallas - 2D Platformer\", 10, 10, 32, DARKBLUE);\n        DrawText(\"Use arrow keys to move the square\", 10, 50, 20, DARKGRAY);\n        \n        DrawRectangle((int)playerPos.x - 25, (int)playerPos.y - 25, 50, 50, RED);\n        \n        EndDrawing();\n    }\n    \n    CloseWindow();\n    return 0;\n}")
    endif()
endif()

# Create the game executable
add_executable(jalanallas ${GAME_SOURCES})

# Set include directories
target_include_directories(jalanallas PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link libraries
target_link_libraries(jalanallas 
    PRIVATE 
        engine
        raylib
        Threads::Threads
)

# Set compiler definitions
target_compile_definitions(jalanallas PRIVATE
    PLATFORM_DESKTOP
)

# Set properties
set_target_properties(jalanallas PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    OUTPUT_NAME "jalanallas"
)

# Set working directory for debugging (Visual Studio)
if(MSVC)
    set_target_properties(jalanallas PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
endif()

message(STATUS "Jalanallas game executable configured")