cmake_minimum_required(VERSION 3.10)
project(jalanallas)

set(CMAKE_C_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add include directory
include_directories("game/include"
                    "engine/include"
                    "editor/include")

# Add library directory
link_directories(lib)

# Add executable
add_executable(jalan           game/src/main.c
                               engine/src/jalan_engine.c
                               engine/src/sprite.c
                               engine/src/entity.c
                               engine/src/dynamic_array.c
                               engine/src/parallax.c
                               engine/src/cJSON.c
                               engine/src/asset_loader.c)


add_executable(editor          editor/src/main.c
                               engine/src/jalan_engine.c
                               engine/src/sprite.c
                               engine/src/entity.c
                               engine/src/dynamic_array.c
                               engine/src/parallax.c
                               engine/src/cJSON.c
                               engine/src/asset_loader.c
                               editor/src/nuklear_impl.c
                               editor/src/ui.c)                                  

# Link raylib
target_link_libraries(jalan raylib)
target_link_libraries(editor raylib)

# Platform specific libraries for raylib
if(WIN32)
    target_link_libraries(jalan winmm gdi32 opengl32)
    target_link_libraries(editor winmm gdi32 opengl32)
elseif(APPLE)
    target_link_libraries(jalan "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
    target_link_libraries(editor "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
elseif(UNIX)
    target_link_libraries(jalan GL m pthread dl rt X11)
    target_link_libraries(editor GL m pthread dl rt X11)
endif()