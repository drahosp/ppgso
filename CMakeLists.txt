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
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add custom target for installation in CLion
add_custom_target(run_install COMMAND ${CMAKE_MAKE_PROGRAM} install)

# Use generate_shader function to convert .glsl sources to C++ headers
include(shaders)

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
set(STRICT_COMPILE_FLAGS "-Wpedantic -Wall -Wno-c++98-compat -Wfloat-equal -Wextra -Wsign-promo -Wsign-compare -Wconversion -Wno-sign-conversion -Wno-unused-parameter")
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

# PPGSO library
add_library(ppgso STATIC
        src/lib/ppgso/mesh.cpp
        src/lib/ppgso/tiny_obj_loader.cpp
        src/lib/ppgso/shader.cpp
        src/lib/ppgso/texture.cpp
        src/lib/ppgso/window.cpp)
# Make sure GLM uses radians and static GLEW library
target_compile_definitions(ppgso PUBLIC -DGLM_FORCE_RADIANS -DGLEW_STATIC )
# Link to GLFW, GLEW and OpenGL
target_link_libraries(ppgso PUBLIC ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES})
# Pass on include directories
target_include_directories(ppgso PUBLIC
        src/lib
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
set(RAW1_GRADIENT_SRC
        src/raw1_gradient/raw1_gradient.cpp)
add_executable(raw1_gradient ${RAW1_GRADIENT_SRC})
target_link_libraries(raw1_gradient ppgso)
install(TARGETS raw1_gradient DESTINATION .)

# gl1_gradient
set(GL1_GRADIENT_SRC src/gl1_gradient/gl1_gradient.cpp)
generate_shaders(GL1_GRADIENT_SHADERS
        src/gl1_gradient/gl1_gradient_vert.glsl
        src/gl1_gradient/gl1_gradient_frag.glsl)
add_executable(gl1_gradient ${GL1_GRADIENT_SRC} ${GL1_GRADIENT_SHADERS})
target_link_libraries(gl1_gradient ppgso)
install(TARGETS gl1_gradient DESTINATION .)

# gl2_texture
set(GL2_TEXTURE_SRC src/gl2_texture/gl2_texture.cpp)
generate_shaders(GL2_TEXTURE_SHADERS
        src/gl2_texture/gl2_texture_vert.glsl
        src/gl2_texture/gl2_texture_frag.glsl)
add_executable(gl2_texture ${GL2_TEXTURE_SRC} ${GL2_TEXTURE_SHADERS})
target_link_libraries(gl2_texture ppgso)
install(TARGETS gl2_texture DESTINATION .)

# gl3_animate
set(GL3_ANIMATE_SRC src/gl3_animate/gl3_animate.cpp)
generate_shaders(GL3_ANIMATE_SHADERS
        src/gl3_animate/gl3_animate_vert.glsl
        src/gl3_animate/gl3_animate_frag.glsl)
add_executable(gl3_animate ${GL3_ANIMATE_SRC} ${GL3_ANIMATE_SHADERS})
target_link_libraries(gl3_animate ppgso)
install(TARGETS gl3_animate DESTINATION .)

# gl4_transform
set(GL4_TRANSFORM_SRC src/gl4_transform/gl4_transform.cpp)
generate_shaders(GL4_TRANSFORM_SHADERS
        src/gl4_transform/gl4_transform_vert.glsl
        src/gl4_transform/gl4_transform_frag.glsl)
add_executable(gl4_transform ${GL4_TRANSFORM_SRC} ${GL4_TRANSFORM_SHADERS})
target_link_libraries(gl4_transform ppgso)
install(TARGETS gl4_transform DESTINATION .)

# gl5_projection
set(GL5_PROJECTION_SRC src/gl5_projection/gl5_projection.cpp)
generate_shaders(GL5_PROJECTION_SHADERS
        src/gl5_projection/gl5_projection_vert.glsl
        src/gl5_projection/gl5_projection_frag.glsl)
add_executable(gl5_projection ${GL5_PROJECTION_SRC} ${GL5_PROJECTION_SHADERS})
target_link_libraries(gl5_projection ppgso)
install(TARGETS gl5_projection DESTINATION .)

# gl6_mesh
set(GL6_MESH_SRC src/gl6_mesh/gl6_mesh.cpp)
generate_shaders(GL6_MESH_SHADERS
        src/gl6_mesh/gl6_mesh_vert.glsl
        src/gl6_mesh/gl6_mesh_frag.glsl)
add_executable(gl6_mesh ${GL6_MESH_SRC} ${GL6_MESH_SHADERS})
target_link_libraries(gl6_mesh ppgso)
install(TARGETS gl6_mesh DESTINATION .)

# gl7_diffuse
set(GL7_DIFFUSE_SRC src/gl7_diffuse/gl7_diffuse.cpp)
generate_shaders(GL7_DIFFUSE_SHADERS
        src/gl7_diffuse/gl7_diffuse_vert.glsl
        src/gl7_diffuse/gl7_diffuse_frag.glsl)
add_executable(gl7_diffuse ${GL7_DIFFUSE_SRC} ${GL7_DIFFUSE_SHADERS})
target_link_libraries(gl7_diffuse ppgso)
install(TARGETS gl7_diffuse DESTINATION .)

# gl8_framebuffer
set(GL8_FRAMEBUFFER_SRC src/gl8_framebuffer/gl8_framebuffer.cpp)
generate_shaders(GL8_FRAMEBUFFER_SHADERS
        src/gl8_framebuffer/gl8_framebuffer_vert.glsl
        src/gl8_framebuffer/gl8_framebuffer_frag.glsl
        src/gl8_framebuffer/gl8_framebuffer_texture_vert.glsl
        src/gl8_framebuffer/gl8_framebuffer_texture_frag.glsl)
add_executable(gl8_framebuffer ${GL8_FRAMEBUFFER_SRC} ${GL8_FRAMEBUFFER_SHADERS})
target_link_libraries(gl8_framebuffer ppgso)
install(TARGETS gl8_framebuffer DESTINATION .)

# gl9_scene
set(GL9_SCENE_SRC
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
generate_shaders(GL9_SCENE_SHADERS
        src/gl9_scene/space_frag.glsl
        src/gl9_scene/space_vert.glsl
        src/gl9_scene/object_frag.glsl
        src/gl9_scene/object_vert.glsl
        src/gl9_scene/explosion_frag.glsl
        src/gl9_scene/explosion_vert.glsl)
add_executable(gl9_scene ${GL9_SCENE_SRC} ${GL9_SCENE_SHADERS})
target_link_libraries(gl9_scene ppgso)
install(TARGETS gl9_scene DESTINATION .)

# TASKs

# task1_invert
set(TASK1_SRC src/task1_invert/task1_invert.cpp)
add_executable(task1_invert ${TASK1_SRC})
install(TARGETS task1_invert DESTINATION .)

# task2_bresenham
set(TASK2_SRC src/task2_bresenham/task2_bresenham.cpp)
add_executable(task2_bresenham ${TASK2_SRC})
install(TARGETS task2_bresenham DESTINATION .)

# task3_bezier
set(TASK3_SRC src/task3_bezier/task3_bezier.cpp)
generate_shaders(TASK3_SHADERS
        src/task3_bezier/task3_bezier_vert.glsl
        src/task3_bezier/task3_bezier_frag.glsl)
add_executable(task3_bezier ${TASK3_SRC} ${TASK3_SHADERS})
target_link_libraries(task3_bezier ppgso)
install(TARGETS task3_bezier DESTINATION .)

# task4_convolution
set(TASK4_SRC src/task4_convolution/task4_convolution.cpp)
generate_shaders(TASK4_SHADERS
        src/task4_convolution/task4_convolution_vert.glsl
        src/task4_convolution/task4_convolution_frag.glsl)
add_executable(task4_convolution ${TASK4_SRC} ${TASK4_SHADERS})
target_link_libraries(task4_convolution ppgso)
install(TARGETS task4_convolution DESTINATION .)

# task5_2dshapes
set(TASK5_SRC src/task5_2dshapes/task5_2dshapes.cpp)
generate_shaders(TASK5_SHADERS
        src/task5_2dshapes/task5_2dshapes_vert.glsl
        src/task5_2dshapes/task5_2dshapes_frag.glsl)
add_executable(task5_2dshapes ${TASK5_SRC} ${TASK5_SHADERS})
target_link_libraries(task5_2dshapes ppgso)
install(TARGETS task5_2dshapes DESTINATION .)

# task6_bezier_surface
set(TASK6_SRC src/task6_bezier_surface/task6_bezier_surface.cpp)
generate_shaders(TASK6_SHADERS
        src/task6_bezier_surface/task6_bezier_surface_vert.glsl
        src/task6_bezier_surface/task6_bezier_surface_frag.glsl)
add_executable(task6_bezier_surface ${TASK6_SRC} ${TASK6_SHADERS})
target_link_libraries(task6_bezier_surface ppgso)
install(TARGETS task6_bezier_surface DESTINATION .)

# task7_bezier_surface
set(TASK7_SRC src/task7_dynamic_surface/task7_dynamic_surface.cpp)
generate_shaders(TASK7_SHADERS
        src/task7_dynamic_surface/task7_dynamic_surface_vert.glsl
        src/task7_dynamic_surface/task7_dynamic_surface_frag.glsl)
add_executable(task7_dynamic_surface ${TASK7_SRC} ${TASK7_SHADERS})
target_link_libraries(task7_dynamic_surface ppgso)
install(TARGETS task7_dynamic_surface DESTINATION .)

# ADD YOUR PROJECT HERE
#set(MY_PROJECT_SRC src/my_project/my_project.cpp)
#generate_shaders(MY_PROJECT_SHADERS src/my_project/my_project_vert.glsl src/my_project/my_project_frag.glsl)
#add_executable(my_project ${MY_PROJECT_SRC} ${MY_PROJECT_SHADERS})
#target_link_libraries(my_project ppgso)
#install (TARGETS my_project DESTINATION .)

#
# INSTALLATION
#

install(DIRECTORY data/ DESTINATION .)
