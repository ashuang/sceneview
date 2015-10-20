set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_definitions(-fPIC)

qt5_add_resources(sceneview_resources
                  resources.qrc)

add_library(sceneview
            SHARED
            asset_importer.cpp
            axis_aligned_box.cpp
            internal_gl.cpp
            camera_node.cpp
            draw_scene.cpp
            expander_widget.cpp
            font_resource.cpp
            geometry_resource.cpp
            grid_renderer.cpp
            group_node.cpp
            importer_assimp.cpp
            importer_rwx.cpp
            input_handler.cpp
            input_handler_widget.cpp
            light_node.cpp
            material_resource.cpp
            mesh_node.cpp
            param_widget.cpp
            renderer.cpp
            renderer_widget_stack.cpp
            resource_manager.cpp
            scene.cpp
            scene_node.cpp
            shader_resource.cpp
            shader_uniform.cpp
            stock_resources.cpp
            text_billboard.cpp
            viewer.cpp
            view_handler_horizontal.cpp
            viewport.cpp
            ${sceneview_resources})

target_link_libraries(sceneview ${OPENGL_LIBS} assimp Qt5::Widgets Qt5::Gui)

install(TARGETS sceneview LIBRARY DESTINATION lib)

# Create a .pc (pkg-config) file to install
set(pc_fname ${CMAKE_CURRENT_BINARY_DIR}/sceneview.pc)
file(WRITE ${pc_fname}
     "prefix=${CMAKE_INSTALL_PREFIX}\n"
     "exec_prefix=\${prefix}\n"
     "libdir=${CMAKE_INSTALL_PREFIX}/lib\n"
     "includedir=${CMAKE_INSTALL_PREFIX}/include\n"
     "\n"
     "Name: sceneview\n"
     "Description: Sceneview visualization toolkit\n"
     "Requires: Qt5Widgets Qt5Gui\n"
     "Version: ${SV_VERSION}\n"
     "Libs: -L\${libdir} -lsceneview\n"
     "Cflags: -I\${includedir}\n")
install(FILES ${pc_fname} DESTINATION lib/pkgconfig)

# Examples / tests
add_executable(sv_expander_widget_example
               expander_widget_example.cpp)
target_link_libraries(sv_expander_widget_example
                      sceneview Qt5::Widgets Qt5::Gui)
