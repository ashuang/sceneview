add_executable(tutorial_03_renderer tutorial_03_renderer.cpp my_renderer.cpp)

target_link_libraries(tutorial_03_renderer
    sceneview Qt5::Widgets Qt5::Gui ${OPENGL_LIBS})
