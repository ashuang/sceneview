cmake_minimum_required(VERSION 2.8.11)

project(sceneview)

# export the compile commands that CMake uses. This is for code completion
# tools like YouCompleteMe
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

#set(qt5_cmake_dir /usr/local/Cellar/qt5/5.5.0/lib/cmake)
#file(GLOB qt5_cmake_subdirs ${qt5_cmake_dir}/*)
#foreach(qt5_cmake_subdir ${qt5_cmake_subdirs})
#  list(APPEND CMAKE_MODULE_PATH ${qt5_cmake_subdir})
#endforeach()
#message("CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")

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

add_subdirectory(src)
add_subdirectory(examples)
