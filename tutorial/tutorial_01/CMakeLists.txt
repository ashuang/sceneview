add_executable(tutorial_01_hello tutorial_01_hello.cpp)

target_link_libraries(tutorial_01_hello
    sceneview Qt5::Widgets Qt5::Gui ${OPENGL_LIBS})
