add_executable(sv_example_viewer
               main.cpp
               example_renderer.cpp
               example_input_handler.cpp
               fixed_function_renderer.cpp
               text_renderer.cpp)

target_link_libraries(sv_example_viewer sceneview Qt5::Widgets Qt5::Gui)
