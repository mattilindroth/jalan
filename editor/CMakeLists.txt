# Editor CMakeLists.txt

# Collect all source files
file(GLOB_RECURSE EDITOR_SOURCES 
    "src/*.c"
    "include/*.h"
)

# If no sources exist yet, create a dummy main file
if(NOT EDITOR_SOURCES)
    set(EDITOR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c")
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c")
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src")
        file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c" 
            "#include <raylib.h>\n#include <engine.h>\n\nint main(void) {\n    // Initialize raylib\n    const int screenWidth = 1200;\n    const int screenHeight = 800;\n    InitWindow(screenWidth, screenHeight, \"Jalan Engine Editor\");\n    SetTargetFPS(60);\n    \n    // Initialize engine\n    engine_init();\n    \n    // Main editor loop\n    while (!WindowShouldClose()) {\n        BeginDrawing();\n        ClearBackground(DARKGRAY);\n        \n        DrawText(\"Jalan Engine Editor\", 10, 10, 32, WHITE);\n        DrawText(\"Press ESC to close\", 10, 50, 20, LIGHTGRAY);\n        \n        EndDrawing();\n    }\n    \n    CloseWindow();\n    return 0;\n}")
    endif()
endif()

# Create the editor executable
add_executable(editor ${EDITOR_SOURCES})

# Set include directories
target_include_directories(editor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link libraries
target_link_libraries(editor 
    PRIVATE 
        engine
        raylib
        Threads::Threads
)

# Set compiler definitions
target_compile_definitions(editor PRIVATE
    PLATFORM_DESKTOP
)

# Set properties
set_target_properties(editor PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    OUTPUT_NAME "jalan_editor"
)

# Set working directory for debugging (Visual Studio)
if(MSVC)
    set_target_properties(editor PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
endif()

message(STATUS "Editor executable configured")