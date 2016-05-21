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

add_executable(sv_example_viewer
               main.cpp
               polylines_renderer.cpp
               example_input_handler.cpp
               fixed_function_renderer.cpp
               hud_renderer.cpp
               stock_shape_selector.cpp
               stock_shape_renderer.cpp
               text_renderer.cpp
               texture_renderer.cpp)

target_link_libraries(sv_example_viewer
                      sceneview Qt5::Widgets Qt5::Gui ${OPENGL_LIBS})
