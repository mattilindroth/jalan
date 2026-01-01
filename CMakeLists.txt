cmake_minimum_required(VERSION 3.15)
project(JalanEngine VERSION 1.0.0 LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Generate compile_commands.json for IntelliSense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Set compiler flags
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

# Platform-specific settings
if(WIN32)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CRT_SECURE_NO_WARNINGS")
endif()

# Find packages
find_package(Threads REQUIRED)

# Set raylib paths
set(RAYLIB_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/lib/raylib")
set(RAYLIB_INCLUDE_DIR "${RAYLIB_ROOT}/include")
set(RAYLIB_LIB_DIR "${RAYLIB_ROOT}/lib")

# Check if raylib exists
if(NOT EXISTS "${RAYLIB_INCLUDE_DIR}/raylib.h")
    message(FATAL_ERROR "raylib.h not found at ${RAYLIB_INCLUDE_DIR}")
endif()

if(NOT EXISTS "${RAYLIB_LIB_DIR}/libraylib.a")
    message(FATAL_ERROR "libraylib.a not found at ${RAYLIB_LIB_DIR}")
endif()

# Create raylib imported target
add_library(raylib STATIC IMPORTED)
set_target_properties(raylib PROPERTIES
    IMPORTED_LOCATION "${RAYLIB_LIB_DIR}/libraylib.a"
    INTERFACE_INCLUDE_DIRECTORIES "${RAYLIB_INCLUDE_DIR}"
)

# Platform-specific raylib dependencies
if(WIN32)
    target_link_libraries(raylib INTERFACE winmm gdi32 opengl32)
elseif(UNIX AND NOT APPLE)
    # Linux
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(DEPS REQUIRED x11 xrandr xinerama xi xcursor gl)
    target_link_libraries(raylib INTERFACE ${DEPS_LIBRARIES} pthread dl m)
    target_compile_options(raylib INTERFACE ${DEPS_CFLAGS_OTHER})
elseif(APPLE)
    # macOS
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    find_library(OPENGL_LIBRARY OpenGL)
    target_link_libraries(raylib INTERFACE ${COCOA_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY} ${OPENGL_LIBRARY})
endif()

# Engine library
add_subdirectory(engine)

# Editor executable
add_subdirectory(editor)

# Game executable
add_subdirectory(jalanallas)

# Set startup project for Visual Studio
if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT jalanallas)
endif()

# Print configuration
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "Raylib include: ${RAYLIB_INCLUDE_DIR}")
message(STATUS "Raylib library: ${RAYLIB_LIB_DIR}/libraylib.a")