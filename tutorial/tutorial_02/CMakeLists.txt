add_executable(tutorial_02_builtin tutorial_02_builtin.cpp)

target_link_libraries(tutorial_02_builtin
    sceneview Qt5::Widgets Qt5::Gui ${OPENGL_LIBS})
