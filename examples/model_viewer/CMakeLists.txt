cmake_minimum_required(VERSION 3.10.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets)

set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

add_executable(sv_model_viewer
               main.cpp
               model_renderer.cpp)

target_link_libraries(sv_model_viewer Sceneview::sview Qt5::Widgets Qt5::Gui)

install(TARGETS sv_model_viewer RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
