set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_definitions(-fPIC)

qt5_add_resources(sceneview_resources
                  resources.qrc)

set(sview_src
    asset_importer.cpp
    axis_aligned_box.cpp
    camera_node.cpp
    drawable.cpp
    draw_context.cpp
    draw_group.cpp
    draw_node.cpp
    expander_widget.cpp
    font_resource.cpp
    geometry_resource.cpp
    grid_renderer.cpp
    group_node.cpp
    importer_assimp.cpp
    importer_rwx.cpp
    input_handler.cpp
    input_handler_widget_stack.cpp
    internal_gl.cpp
    light_node.cpp
    material_resource.cpp
    param_widget.cpp
    plane.cpp
    renderer.cpp
    renderer_widget_stack.cpp
    resource_manager.cpp
    scene.cpp
    scene_node.cpp
    selection_query.cpp
    shader_resource.cpp
    shader_uniform.cpp
    stock_resources.cpp
    text_billboard.cpp
    viewer.cpp
    view_handler_horizontal.cpp
    viewport.cpp
    ${sceneview_resources})

# Create the main sceneview library
add_library(sceneview1 SHARED ${sview_src})
set_target_properties(sceneview1 PROPERTIES
                      VERSION ${SV_VERSION}
                      SOVERSION ${SV_VERSION_MAJOR})
add_library(Sceneview::sceneview ALIAS sceneview1)

target_link_libraries(sceneview1
                      PUBLIC ${OPENGL_LIBS} Qt5::Widgets Qt5::Gui
                      PRIVATE assimp)

install(TARGETS sceneview1
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}${LIB_SUFFIX}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}${LIB_SUFFIX}
    )

# Install public header files
install(FILES asset_importer.hpp
              axis_aligned_box.hpp
              camera_node.hpp
              drawable.hpp
              draw_group.hpp
              draw_node.hpp
              expander_widget.hpp
              font_resource.hpp
              geometry_resource.hpp
              grid_renderer.hpp
              group_node.hpp
              input_handler.hpp
              input_handler_widget_stack.hpp
              light_node.hpp
              material_resource.hpp
              param_widget.hpp
              plane.hpp
              renderer.hpp
              renderer_widget_stack.hpp
              resource_manager.hpp
              scene.hpp
              scene_node.hpp
              sceneview.hpp
              selection_query.hpp
              shader_resource.hpp
              shader_uniform.hpp
              stock_resources.hpp
              text_billboard.hpp
              viewer.hpp
              view_handler_horizontal.hpp
              viewport.hpp
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sceneview)

# Create a .pc (pkg-config) file to install
set(pc_fname ${CMAKE_CURRENT_BINARY_DIR}/sceneview.pc)
file(WRITE ${pc_fname}
     "prefix=${CMAKE_INSTALL_PREFIX}\n"
     "exec_prefix=\${prefix}\n"
     "libdir=\${exec_prefix}/${CMAKE_INSTALL_LIBDIR}${LIB_SUFFIX}\n"
     "includedir=\${prefix}/${CMAKE_INSTALL_INCLUDEDIR}\n"
     "\n"
     "Name: sceneview\n"
     "Description: Sceneview visualization toolkit\n"
     "Requires: Qt5Widgets Qt5Gui\n"
     "Version: ${SV_VERSION}\n"
     "Libs: -L\${libdir} -lsceneview1\n"
     "Cflags: -I\${includedir}\n")
install(FILES ${pc_fname} DESTINATION ${CMAKE_INSTALL_LIBDIR}${LIB_SUFFIX}/pkgconfig)

# Examples / tests
add_executable(sv_expander_widget_example
               expander_widget_example.cpp)
target_link_libraries(sv_expander_widget_example
                      Sceneview::sceneview Qt5::Widgets Qt5::Gui)

if(HAVE_GTEST)
macro(sv_test name)
  add_executable(${name}_test ${name}_test.cpp)
  target_link_libraries(${name}_test Sceneview::sceneview gtest gtest_main)
  add_test(${name}_test ${EXECUTABLE_OUTPUT_PATH}/${name}_test)
endmacro()

sv_test(axis_aligned_box)
sv_test(plane)
endif()
