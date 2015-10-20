add_executable(sv_model_viewer
               main.cpp
               model_renderer.cpp)

target_link_libraries(sv_model_viewer sceneview Qt5::Widgets Qt5::Gui)

install(TARGETS sv_model_viewer RUNTIME DESTINATION bin)
