cmake_minimum_required(VERSION 2.8.11)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets)

set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

if(APPLE)
  set(OPENGL_LIBS "-framework OpenGL")
else()
  set(OPENGL_LIBS "GL")
endif()

add_subdirectory(tutorial_01)
add_subdirectory(tutorial_02)
add_subdirectory(tutorial_03)
