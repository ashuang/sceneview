cmake_minimum_required(VERSION 2.8.11)

project(sceneview)

# export the compile commands that CMake uses. This is for code completion
# tools like YouCompleteMe
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

find_package(Qt5Widgets)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Werror -Wno-inconsistent-missing-override ${CMAKE_CXX_FLAGS}")

include_directories(src)

if(APPLE)
  set(OPENGL_LIBS "-framework OpenGL")
else()
  set(OPENGL_LIBS "GL")
endif()

set(SV_VERSION 0.0.1)

set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(src)
add_subdirectory(examples)
