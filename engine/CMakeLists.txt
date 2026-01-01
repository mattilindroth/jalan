# Engine Library CMakeLists.txt

# Collect all source files
file(GLOB_RECURSE ENGINE_SOURCES 
    "src/*.c"
    "include/*.h"
)

# If no sources exist yet, create dummy files to prevent errors
if(NOT ENGINE_SOURCES)
    set(ENGINE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/engine.c")
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/engine.c")
        file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/src/engine.c" 
            "// Engine implementation\n#include \"engine.h\"\n\nvoid engine_init(void) {\n    // TODO: Initialize engine\n}\n")
    endif()
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include/engine.h")
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include")
        file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/include/engine.h" 
            "#ifndef ENGINE_H\n#define ENGINE_H\n\n// Engine API\nvoid engine_init(void);\n\n#endif // ENGINE_H\n")
    endif()
endif()

# Create the engine library
add_library(engine STATIC ${ENGINE_SOURCES})

# Set include directories
target_include_directories(engine
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link raylib
target_link_libraries(engine 
    PUBLIC raylib
    PRIVATE Threads::Threads
)

# Set compiler definitions
target_compile_definitions(engine PRIVATE
    PLATFORM_DESKTOP
)

# Set properties
set_target_properties(engine PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    OUTPUT_NAME "jalanengine"
)

message(STATUS "Engine library configured")