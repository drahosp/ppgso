# This CMake script is designed for CLion IDE and QTCreator projects on Windows
# It should find bundled binary dependencies automatically when using MinGW compiler
# On Linux and Mac the dependencies (glm, glew, glfw) need to be installed manually

cmake_minimum_required(VERSION 2.6)
project(ppgso CXX)

# Set up external dependencies for Windows users
if (MINGW)
  set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${CMAKE_SOURCE_DIR}/dependencies/mingw/include/")
  set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${CMAKE_SOURCE_DIR}/dependencies/mingw/lib/")
endif ()

# Basic CMake settings
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/_install)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Warnings for Debug mode
option(USE_STRICT_COMPILE_WARNINGS "Use strict compilation warnings in debug mode." ON)
if (USE_STRICT_COMPILE_WARNINGS)
  # Assuming Clang and GCC
  set(STRICT_COMPILE_WARNINGS "-Wpedantic -Wall -Wfloat-equal -Wextra -Wsign-promo -Wsign-compare -Wconversion -Wno-sign-conversion")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${STRICT_COMPILE_WARNINGS}")
endif ()

# Find dependencies
find_package(GLFW3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)     # GLM is header only
find_package(OpenGL REQUIRED)

# Add custom target for installation in CLion
add_custom_target(run_install COMMAND ${CMAKE_MAKE_PROGRAM} install)

# Set default installation destination
if (NOT CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "../_install")
endif ()

# Use generate_shader function to vonvert .glsl sources to C++ headers
include(shaders)

# Include header directories
include_directories(include ${OPENGL_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

# Prepare libraries for linking
set(LIBRARIES ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES} ${GLEW_LIBRARIES})

# raw_gradient
set(RAW_GRADIENT SRC src/raw_gradient/raw_gradient.cpp)
add_executable(raw_gradient ${RAW_GRADIENT})
install(TARGETS raw_gradient DESTINATION .)

# gl_gradient
set(GL_GRADIENT_SRC src/gl_gradient/gl_gradient.cpp)
generate_shaders(GL_GRADIENT_SHADERS src/gl_gradient/gl_gradient_vert.glsl src/gl_gradient/gl_gradient_frag.glsl)
add_executable(gl_gradient ${GL_GRADIENT_SRC} ${GL_GRADIENT_SHADERS})
target_link_libraries(gl_gradient ${LIBRARIES})
install(TARGETS gl_gradient DESTINATION .)

# gl_texture
set(GL_TEXTURE_SRC src/gl_texture/gl_texture.cpp)
generate_shaders(GL_TEXTURE_SHADERS src/gl_texture/gl_texture_vert.glsl src/gl_texture/gl_texture_frag.glsl)
add_executable(gl_texture ${GL_TEXTURE_SRC} ${GL_TEXTURE_SHADERS})
target_link_libraries(gl_texture ${LIBRARIES})
install(TARGETS gl_texture DESTINATION .)

# gl_animate
set(GL_ANIMATE_SRC src/gl_animate/gl_animate.cpp)
generate_shaders(GL_ANIMATE_SHADERS src/gl_animate/gl_animate_vert.glsl src/gl_animate/gl_animate_frag.glsl)
add_executable(gl_animate ${GL_ANIMATE_SRC} ${GL_ANIMATE_SHADERS})
target_link_libraries(gl_animate ${LIBRARIES})
install(TARGETS gl_animate DESTINATION .)

# gl_transform
set(GL_TRANSFORM_SRC src/gl_transform/gl_transform.cpp)
generate_shaders(GL_TRANSFORM_SHADERS src/gl_transform/gl_transform_vert.glsl src/gl_transform/gl_transform_frag.glsl)
add_executable(gl_transform ${GL_TRANSFORM_SRC} ${GL_TRANSFORM_SHADERS})
target_link_libraries(gl_transform ${LIBRARIES})
install(TARGETS gl_transform DESTINATION .)

# gl_projection
set(GL_PROJECTION_SRC src/gl_projection/gl_projection.cpp)
generate_shaders(GL_PROJECTION_SHADERS src/gl_projection/gl_projection_vert.glsl src/gl_projection/gl_projection_frag.glsl)
add_executable(gl_projection ${GL_PROJECTION_SRC} ${GL_PROJECTION_SHADERS})
target_link_libraries(gl_projection ${LIBRARIES})
install(TARGETS gl_projection DESTINATION .)

# gl_mesh
set(GL_MESH_SRC src/gl_mesh/gl_mesh.cpp src/lib/mesh.cpp src/lib/tiny_obj_loader.cpp)
generate_shaders(GL_MESH_SHADERS src/gl_mesh/gl_mesh_vert.glsl src/gl_mesh/gl_mesh_frag.glsl)
add_executable(gl_mesh ${GL_MESH_SRC} ${GL_MESH_SHADERS})
target_link_libraries(gl_mesh ${LIBRARIES})
install(TARGETS gl_mesh DESTINATION .)

# ADD YOUR PROJECT HERE
#set(MY_PROJECT_SRC src/my_project/my_project.cpp)
#generate_shaders(MY_PROJECT_SHADERS src/my_project/my_project_vert.glsl src/my_project/my_project_frag.glsl)
#add_executable(my_project ${MY_PROJECT_SRC} ${MY_PROJECT_SHADERS})
#target_link_libraries(my_project ${LIBRARIES})
#install (TARGETS my_project DESTINATION .)

# Install data to destination
install(DIRECTORY data/ DESTINATION .)

# Install runtime dlls to destination when on Windows
if (MINGW)
  install(FILES ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} DESTINATION .)
  # Install dlls into binary dir for development and debugging
  file(COPY ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} DESTINATION ${CMAKE_BINARY_DIR})
endif ()
