// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_SCENEVIEW_HPP__
#define SCENEVIEW_SCENEVIEW_HPP__

// Convenience header file that just includes all public sceneview header
// files.
//
// You can generally improve compile times if you selectively include only the
// header files needed by your application. However, it is more time consuming
// to select exactly the right subset, so you can just include this one for
// convenience.

#include <sceneview/asset_importer.hpp>
#include <sceneview/axis_aligned_box.hpp>
#include <sceneview/camera_node.hpp>
#include <sceneview/expander_widget.hpp>
#include <sceneview/font_resource.hpp>
#include <sceneview/geometry_resource.hpp>
#include <sceneview/grid_renderer.hpp>
#include <sceneview/group_node.hpp>
#include <sceneview/input_handler.hpp>
#include <sceneview/input_handler_widget_stack.hpp>
#include <sceneview/light_node.hpp>
#include <sceneview/material_resource.hpp>
#include <sceneview/draw_node.hpp>
#include <sceneview/param_widget.hpp>
#include <sceneview/renderer.hpp>
#include <sceneview/renderer_widget_stack.hpp>
#include <sceneview/resource_manager.hpp>
#include <sceneview/scene.hpp>
#include <sceneview/scene_node.hpp>
#include <sceneview/sceneview.hpp>
#include <sceneview/selection_query.hpp>
#include <sceneview/shader_resource.hpp>
#include <sceneview/shader_uniform.hpp>
#include <sceneview/stock_resources.hpp>
#include <sceneview/text_billboard.hpp>
#include <sceneview/viewer.hpp>
#include <sceneview/view_handler_horizontal.hpp>
#include <sceneview/viewport.hpp>

/**
 * @defgroup sv_resources Resource System
 * Resource system
 */

/**
 * @defgroup sv_scenegraph Scene Graph
 * Scene Graph
 */

/**
 * @defgroup sv_gui GUI
 * Building a 3D application with Sceneview
 */

#endif  // SCENEVIEW_SCENEVIEW_HPP__
