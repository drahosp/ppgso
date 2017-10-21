# This CMake script is designed for CLion IDE and QTCreator projects on Windows
# It should find bundled binary dependencies automatically when using MinGW compiler
# On Linux and Mac the dependencies (glm, glew, glfw) need to be installed manually

cmake_minimum_required(VERSION 3.1)
project(ppgso CXX)

#
# CONFIGURATION
#

# Basic CMake settings
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/_install)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data)

# Use add_resources function to convert .glsl to a .cpp files which will be compiled into a separate static library
include(add_resources)

#
# DEPENDENCIES
#

# Set up external dependencies for Windows users
if (MINGW)
  set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${CMAKE_SOURCE_DIR}/dependencies/include/")
  set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${CMAKE_SOURCE_DIR}/dependencies/lib/mingw")
elseif (MSVC)
  set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${CMAKE_SOURCE_DIR}/dependencies/include/")
  set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${CMAKE_SOURCE_DIR}/dependencies/lib/vc2015")
  set(USE_STRICT_COMPILE_WARNINGS OFF CACHE BOOL "" FORCE)
endif ()

# Warnings for Debug mode
option(USE_STRICT_COMPILE_WARNINGS "Use strict compilation warnings in debug mode." ON)
# These compile flags should apply for GCC and Clang compilers
set(STRICT_COMPILE_FLAGS "-Wpedantic -Wall -Wno-c++98-compat -Wextra -Wno-sign-conversion -Wno-unused-parameter")
if (USE_STRICT_COMPILE_WARNINGS)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${STRICT_COMPILE_FLAGS}")
endif ()

# Find required packages
find_package(GLFW3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
find_package(OpenGL REQUIRED)

# Optional packages
find_package(OpenMP)
if(OPENMP_FOUND)
  list(APPEND CMAKE_CXX_FLAGS ${OpenMP_CXX_FLAGS})
endif()

# Set default installation destination
if (NOT CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "../_install")
endif ()

#
# Shaders / Materials
#

set(PPGSO_SHADER_SRC
        shader/color_vert.glsl shader/color_frag.glsl
        shader/convolution_vert.glsl shader/convolution_frag.glsl
        shader/diffuse_vert.glsl shader/diffuse_frag.glsl
        shader/texture_vert.glsl shader/texture_frag.glsl
        )
add_resources(shaders ${PPGSO_SHADER_SRC})

# PPGSO library
add_library(ppgso STATIC
        ppgso/mesh.cpp
        ppgso/tiny_obj_loader.cpp
        ppgso/shader.cpp
        ppgso/image.cpp
        ppgso/image_bmp.cpp
        ppgso/image_raw.cpp
        ppgso/texture.cpp
        ppgso/window.cpp
        )

# Make sure GLM uses radians and GLEW is a static library
target_compile_definitions(ppgso PUBLIC -DGLM_FORCE_RADIANS -DGLEW_STATIC)

# Link to GLFW, GLEW and OpenGL
target_link_libraries(ppgso PUBLIC ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES})
# Pass on include directories
target_include_directories(ppgso PUBLIC
        ppgso
        ${GLFW_INCLUDE_DIRS}
        ${GLEW_INCLUDE_DIRS}
        ${GLM_INCLUDE_DIRS}
        ${OPENGL_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR})

#
# TARGETS
#

# raw1_gradient
add_executable(raw1_gradient src/raw1_gradient/raw1_gradient.cpp)
target_link_libraries(raw1_gradient ppgso)
install(TARGETS raw1_gradient DESTINATION .)

# raw2_raycast
add_executable(raw2_raycast src/raw2_raycast/raw2_raycast.cpp)
target_link_libraries(raw2_raycast ppgso)
install(TARGETS raw2_raycast DESTINATION .)

# raw3_raytrace
add_executable(raw3_raytrace src/raw3_raytrace/raw3_raytrace.cpp)
target_link_libraries(raw3_raytrace ppgso)
install(TARGETS raw3_raytrace DESTINATION .)

# raw4_raster
add_executable(raw4_raster src/raw4_raster/raw4_raster.cpp)
target_link_libraries(raw4_raster ppgso)
install(TARGETS raw4_raster DESTINATION .)

# gl1_gradient
add_executable(gl1_gradient src/gl1_gradient/gl1_gradient.cpp)
target_link_libraries(gl1_gradient ppgso shaders)
install(TARGETS gl1_gradient DESTINATION .)

# gl2_texture
add_executable(gl2_texture src/gl2_texture/gl2_texture.cpp)
target_link_libraries(gl2_texture ppgso shaders)
install(TARGETS gl2_texture DESTINATION .)

# gl3_animate
add_executable(gl3_animate src/gl3_animate/gl3_animate.cpp)
target_link_libraries(gl3_animate ppgso shaders)
install(TARGETS gl3_animate DESTINATION .)

# gl4_transform
add_executable(gl4_transform src/gl4_transform/gl4_transform.cpp)
target_link_libraries(gl4_transform ppgso shaders)
install(TARGETS gl4_transform DESTINATION .)

# gl5_projection
add_executable(gl5_projection src/gl5_projection/gl5_projection.cpp)
target_link_libraries(gl5_projection ppgso shaders)
install(TARGETS gl5_projection DESTINATION .)

# gl6_mesh
add_executable(gl6_mesh src/gl6_mesh/gl6_mesh.cpp)
target_link_libraries(gl6_mesh ppgso shaders)
install(TARGETS gl6_mesh DESTINATION .)

# gl7_diffuse
add_executable(gl7_diffuse src/gl7_diffuse/gl7_diffuse.cpp)
target_link_libraries(gl7_diffuse ppgso shaders)
install(TARGETS gl7_diffuse DESTINATION .)

# gl8_framebuffer
add_executable(gl8_framebuffer src/gl8_framebuffer/gl8_framebuffer.cpp)
target_link_libraries(gl8_framebuffer ppgso shaders)
install(TARGETS gl8_framebuffer DESTINATION .)

# gl9_scene
add_executable(gl9_scene
        src/gl9_scene/gl9_scene.cpp
        src/gl9_scene/object.cpp
        src/gl9_scene/scene.cpp
        src/gl9_scene/camera.cpp
        src/gl9_scene/asteroid.cpp
        src/gl9_scene/generator.cpp
        src/gl9_scene/player.cpp
        src/gl9_scene/projectile.cpp
        src/gl9_scene/explosion.cpp
        src/gl9_scene/space.cpp)
target_link_libraries(gl9_scene ppgso shaders)
install(TARGETS gl9_scene DESTINATION .)

# Playground target
add_executable(playground src/playground/playground.cpp)
target_link_libraries(playground ppgso shaders)
install (TARGETS playground DESTINATION .)

# Task1
add_executable(task1_filter src/task1_filter/task1_filter.cpp)
target_link_libraries(task1_filter ppgso)
install (TARGETS task1_filter DESTINATION .)

# Task2
add_executable(task2_convolution src/task2_convolution/task2_convolution.cpp)
target_link_libraries(task2_convolution ppgso)
install (TARGETS task2_convolution DESTINATION .)

# Task3
add_executable(task3_bresenham src/task3_bresenham/task3_bresenham.cpp)
target_link_libraries(task3_bresenham ppgso)
install (TARGETS task3_bresenham DESTINATION .)

# Task4
add_executable(task4_bezier src/task4_bezier/task4_bezier.cpp)
target_link_libraries(task4_bezier ppgso shaders)
install (TARGETS task4_bezier DESTINATION .)

# Task5
add_executable(task5_2dshapes src/task5_2dshapes/task5_2dshapes.cpp)
target_link_libraries(task5_2dshapes ppgso shaders)
install (TARGETS task5_2dshapes DESTINATION .)

#
# INSTALLATION
#

install(DIRECTORY data/ DESTINATION .)
