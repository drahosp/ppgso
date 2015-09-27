# This CMake script is mainly designed for CLion IDE and QTCreator projects on Windows
# It should find bundled binary dependencies automatically when using MinGW
cmake_minimum_required ( VERSION 2.6 )
project( ppgso CXX )

# Set up external dependencies for Wwindows users
if ( MINGW )
  set ( CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${CMAKE_SOURCE_DIR}/dependencies/mingw/include/" )
  set ( CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${CMAKE_SOURCE_DIR}/dependencies/mingw/lib/" )
endif ()

# Basic CMake settings
set ( CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/_install )
set ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake )
set ( CMAKE_CXX_STANDARD 11 )
set ( CMAKE_EXPORT_COMPILE_COMMANDS ON )

# Find dependencies
find_package ( GLFW3 REQUIRED )
find_package ( GLEW REQUIRED )
find_package ( GLM REQUIRED )     # GLM is header only
find_package ( OpenGL REQUIRED )

# Include header directories
include_directories ( ${OPENGL_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} )

# Prepare libraries for linking
set ( LIBRARIES ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} )

# raw_gradient
add_executable ( raw_gradient raw_gradient.cpp )
install ( TARGETS raw_gradient DESTINATION . )

# gl_gradient
add_executable ( gl_gradient gl_gradient.cpp )
target_link_libraries( gl_gradient ${LIBRARIES} )

install ( TARGETS gl_gradient DESTINATION . )
install ( FILES gl_gradient.vert gl_gradient.frag DESTINATION . )

# gl_texture
add_executable ( gl_texture gl_texture.cpp  )
target_link_libraries( gl_texture ${LIBRARIES} )

install ( TARGETS gl_texture DESTINATION . )
install ( FILES lena.rgb gl_texture.vert gl_texture.frag DESTINATION . )

# gl_animate
add_executable ( gl_animate gl_animate.cpp  )
target_link_libraries( gl_animate ${LIBRARIES} )

install ( TARGETS gl_animate DESTINATION . )

# ADD YOUR PROJECT HERE
#add_executable( my_project my_project.cpp my_project_other.cpp )
#target_link_libraries( my_project ${LIBRARIES} )

#install ( TARGETS my_project DESTINATION . )
#install ( FILES my_project.file my_project.vert my_project.frag DESTINATION . )

# Install dlls to destination when on Windows
if ( MINGW )
  install ( FILES ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} DESTINATION . )
  # Install dlls into binary dir for development and debugging
  file ( COPY ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} DESTINATION ${CMAKE_BINARY_DIR} )
endif ()
