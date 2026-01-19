cmake_minimum_required(VERSION 3.10)
project(jalan)

set(CMAKE_C_STANDARD 99)

# Add include directory
include_directories(include)

# Add library directory
link_directories(lib)

# Add executable
add_executable(${PROJECT_NAME} src/main.c)

# Link raylib
target_link_libraries(${PROJECT_NAME} raylib)

# Platform specific libraries for raylib
if(WIN32)
    target_link_libraries(${PROJECT_NAME} winmm gdi32 opengl32)
elseif(APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} GL m pthread dl rt X11)
endif()