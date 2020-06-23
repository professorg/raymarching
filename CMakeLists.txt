cmake_minimum_required(VERSION 3.15)

project(GLRayMarching)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

set(SOURCES_DIR src/)
set(INCLUDE_DIR include/)

set(SOURCES main.cpp)
list(TRANSFORM SOURCES PREPEND ${SOURCES_DIR})

set(INCLUDES)
list(TRANSFORM INCLUDES PREPEND ${INCLUDE_DIR})

set(CMAKE_MODULE_PATH /home/professorg/Programming/libs/cmake)

# GLEW
find_package (GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
link_directories(${GLEW_LIBRARY_DIRS})
add_definitions(${GLEW_DEFINITIONS})

# GLFW
find_package (glfw3 REQUIRED)
#find_package (GLFW REQUIRED)
#include_directories(${GLFW_INCLUDE_DIRS})
#link_directories(${GLFW_LIBRARY_DIRS})
#add_definitions(${GLFW_DEFINITIONS})

# GLM
find_package (GLM REQUIRED)
include_directories(${GLM_INCLUDE_DIRS})
link_directories(${GLM_LIBRARY_DIRS})
add_definitions(${GLM_DEFINITIONS})

# OpenGL
find_package (OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})
link_directories(${OPENGL_LIBRARY_DIRS})
add_definitions(${OPENGL_DEFINITIONS})

add_executable(GLRayMarching ${SOURCES} ${INCLUDES})
include_directories(${INCLUDE_DIR})
#target_link_libraries(GLRayMarching ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${GLFW_LIBRARIES} ${GLEW_LIBRARIES})
target_link_libraries(GLRayMarching ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} glfw ${GLEW_LIBRARIES})
