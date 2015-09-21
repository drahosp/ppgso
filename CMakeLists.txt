cmake_minimum_required ( VERSION 2.6 )
project( glfwtest CXX )

# Basic cmake configuration
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake )
set( CMAKE_CXX_FLAGS "-std=c++11" )
set( CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Find dependencies
find_package( GLFW3 REQUIRED )
find_package( GLEW REQUIRED )
find_package( GLM REQUIRED )
find_package( OpenGL REQUIRED )

# Include header directories
include_directories(${OPENGL_INCLUDE_DIRS} ${GLFW3_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})

# Prepare libraries for linking
set( LIBRARIES ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES} ${GLEW_LIBRARIES})

# gl_gradient
add_executable( gl_gradient gl_gradient.cpp )
target_link_libraries( gl_gradient ${LIBRARIES} )

install ( TARGETS gl_gradient DESTINATION . )
install ( FILES gl_gradient.vert gl_gradient.frag DESTINATION . )

# gl_texture
add_executable( gl_texture gl_texture.cpp )
target_link_libraries( gl_texture ${LIBRARIES} )

install ( TARGETS gl_texture DESTINATION . )
install ( FILES lena.rgb gl_texture.vert gl_texture.frag DESTINATION . )
