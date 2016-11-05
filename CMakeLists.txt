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
  set(USE_STRICT_COMPILE_WARNINGS OFF)
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

# Set default installation destination
if (NOT CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "../_install")
endif ()

# PPGSO library
add_library(libppgso STATIC
        src/lib/ppgso/mesh.cpp
        src/lib/ppgso/tiny_obj_loader.cpp
        src/lib/ppgso/shader.cpp
        src/lib/ppgso/texture.cpp
        src/lib/ppgso/window.cpp)
# Make sure GLM uses radians and static GLEW library
target_compile_definitions(libppgso PUBLIC -DGLM_FORCE_RADIANS -DGLEW_STATIC )
# Link to GLFW, GLEW and OpenGL
target_link_libraries(libppgso PUBLIC ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES})
# Pass on include directories
target_include_directories(libppgso PUBLIC
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

# raw_gradient
set(RAW_GRADIENT_SRC
        src/raw_gradient/raw_gradient.cpp)
add_executable(raw_gradient ${RAW_GRADIENT_SRC})
target_link_libraries(raw_gradient libppgso)
install(TARGETS raw_gradient DESTINATION .)

# gl_gradient
set(GL_GRADIENT_SRC src/gl_gradient/gl_gradient.cpp)
generate_shaders(GL_GRADIENT_SHADERS
        src/gl_gradient/gl_gradient_vert.glsl
        src/gl_gradient/gl_gradient_frag.glsl)
add_executable(gl_gradient ${GL_GRADIENT_SRC} ${GL_GRADIENT_SHADERS})
target_link_libraries(gl_gradient libppgso)
install(TARGETS gl_gradient DESTINATION .)

# gl_texture
set(GL_TEXTURE_SRC src/gl_texture/gl_texture.cpp)
generate_shaders(GL_TEXTURE_SHADERS
        src/gl_texture/gl_texture_vert.glsl
        src/gl_texture/gl_texture_frag.glsl)
add_executable(gl_texture ${GL_TEXTURE_SRC} ${GL_TEXTURE_SHADERS})
target_link_libraries(gl_texture libppgso)
install(TARGETS gl_texture DESTINATION .)

# gl_animate
set(GL_ANIMATE_SRC src/gl_animate/gl_animate.cpp)
generate_shaders(GL_ANIMATE_SHADERS
        src/gl_animate/gl_animate_vert.glsl
        src/gl_animate/gl_animate_frag.glsl)
add_executable(gl_animate ${GL_ANIMATE_SRC} ${GL_ANIMATE_SHADERS})
target_link_libraries(gl_animate libppgso)
install(TARGETS gl_animate DESTINATION .)

# gl_transform
set(GL_TRANSFORM_SRC src/gl_transform/gl_transform.cpp)
generate_shaders(GL_TRANSFORM_SHADERS
        src/gl_transform/gl_transform_vert.glsl
        src/gl_transform/gl_transform_frag.glsl)
add_executable(gl_transform ${GL_TRANSFORM_SRC} ${GL_TRANSFORM_SHADERS})
target_link_libraries(gl_transform libppgso)
install(TARGETS gl_transform DESTINATION .)

# gl_projection
set(GL_PROJECTION_SRC src/gl_projection/gl_projection.cpp)
generate_shaders(GL_PROJECTION_SHADERS
        src/gl_projection/gl_projection_vert.glsl
        src/gl_projection/gl_projection_frag.glsl)
add_executable(gl_projection ${GL_PROJECTION_SRC} ${GL_PROJECTION_SHADERS})
target_link_libraries(gl_projection libppgso)
install(TARGETS gl_projection DESTINATION .)

# gl_mesh
set(GL_MESH_SRC src/gl_mesh/gl_mesh.cpp)
generate_shaders(GL_MESH_SHADERS
        src/gl_mesh/gl_mesh_vert.glsl
        src/gl_mesh/gl_mesh_frag.glsl)
add_executable(gl_mesh ${GL_MESH_SRC} ${GL_MESH_SHADERS})
target_link_libraries(gl_mesh libppgso)
install(TARGETS gl_mesh DESTINATION .)

# gl_framebuffer
set(GL_FRAMEBUFFER_SRC src/gl_framebuffer/gl_framebuffer.cpp)
generate_shaders(GL_FRAMEBUFFER_SHADERS
        src/gl_framebuffer/gl_framebuffer_vert.glsl
        src/gl_framebuffer/gl_framebuffer_frag.glsl
        src/gl_framebuffer/gl_framebuffer_texture_vert.glsl
        src/gl_framebuffer/gl_framebuffer_texture_frag.glsl)
add_executable(gl_framebuffer ${GL_FRAMEBUFFER_SRC} ${GL_FRAMEBUFFER_SHADERS})
target_link_libraries(gl_framebuffer libppgso)
install(TARGETS gl_framebuffer DESTINATION .)

# gl_diffuse
set(GL_DIFFUSE_SRC src/gl_diffuse/gl_diffuse.cpp)
generate_shaders(GL_DIFFUSE_SHADERS
        src/gl_diffuse/gl_diffuse_vert.glsl
        src/gl_diffuse/gl_diffuse_frag.glsl)
add_executable(gl_diffuse ${GL_DIFFUSE_SRC} ${GL_DIFFUSE_SHADERS})
target_link_libraries(gl_diffuse libppgso)
install(TARGETS gl_diffuse DESTINATION .)

# gl_scene
set(GL_SCENE_SRC
        src/gl_scene/gl_scene.cpp
        src/gl_scene/object.cpp
        src/gl_scene/scene.cpp
        src/gl_scene/camera.cpp
        src/gl_scene/asteroid.cpp
        src/gl_scene/generator.cpp
        src/gl_scene/player.cpp
        src/gl_scene/projectile.cpp
        src/gl_scene/explosion.cpp
        src/gl_scene/space.cpp)
generate_shaders(GL_SCENE_SHADERS
        src/gl_scene/space_frag.glsl
        src/gl_scene/space_vert.glsl
        src/gl_scene/object_frag.glsl
        src/gl_scene/object_vert.glsl
        src/gl_scene/explosion_frag.glsl
        src/gl_scene/explosion_vert.glsl)
add_executable(gl_scene ${GL_SCENE_SRC} ${GL_SCENE_SHADERS})
target_link_libraries(gl_scene libppgso)
install(TARGETS gl_scene DESTINATION .)

# gl_framebuffer
set(GL_FRAMEBUFFER_SRC src/gl_framebuffer/gl_framebuffer.cpp)
generate_shaders(GL_FRAMEBUFFER_SHADERS
        src/gl_framebuffer/gl_framebuffer_vert.glsl
        src/gl_framebuffer/gl_framebuffer_frag.glsl
        src/gl_framebuffer/texture_vert.glsl
        src/gl_framebuffer/texture_frag.glsl)
add_executable(gl_framebuffer ${GL_FRAMEBUFFER_SRC} ${GL_FRAMEBUFFER_SHADERS})
target_link_libraries(gl_framebuffer libppgso)
install(TARGETS gl_framebuffer DESTINATION .)

# ADD YOUR PROJECT HERE
#set(MY_PROJECT_SRC src/my_project/my_project.cpp)
#generate_shaders(MY_PROJECT_SHADERS src/my_project/my_project_vert.glsl src/my_project/my_project_frag.glsl)
#add_executable(my_project ${MY_PROJECT_SRC} ${MY_PROJECT_SHADERS})
#target_link_libraries(my_project libppgso)
#install (TARGETS my_project DESTINATION .)

#
# INSTALLATION
#

install(DIRECTORY data/ DESTINATION .)
