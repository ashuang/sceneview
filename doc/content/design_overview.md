Design Overview {#design_overview}
========

# Overview {#design_architecture}
This document assumes you have some familiarity with:
- Computer graphics principles
- OpenGL
- Qt

In particular, it will help if you're familiar with concepts like a scene
graph, the OpenGL programmable pipeline, and GUI toolkits.

Sceneview is built on top of Qt 5, and relies heavily on the Qt5Gui and
Qt5Widgets modules.


The major components of Sceneview are:
- Rendering engine
- Qt widgets for building user interfaces.
- Renderer / Input handler system

[TOC]

# Rendering Engine {#design_rendering_engine}

There are two ways to draw things with Sceneview:
1. Using the Sceneview rendering engine.
2. Using direct OpenGL commands, either using the older OpenGL fixed-function
   pipeline (e.g., with `glBegin`, `glVertex` and friends) or using your own
   programmable shaders directly.

It is possible to use both in a single render cycle. This section
describes the architecture of the Sceneview rendering engine, which is a
shader-driven architecture for the modern OpenGL programmable pipeline.

The Sceneview rendering engine is similar to what you might find in a larger,
more complex rendering engine like Ogre3d or Unity, but greatly slimmed down
and simplified. Sceneview strives for simplicity and a primary goal is a small
API that is just enough to enable sophisticated rendering techniques while
still being simple enough for most people to completely understand.

The major concepts in the Sceneview rendering engine are:
- The resource system
- The scene graph

## Resources {#design_resources}
A _resource_ is generally any data that is needed for some aspect of drawing, but
that can be shared across multiple things being drawn. That's pretty vague, so
the specific types of resources in Sceneview are:
- \ref design_resources_font
- \ref design_resources_geometry
- \ref design_resources_material
- \ref design_resources_scene
- \ref design_resources_shader

Some general rules about resources:
- A resource is not drawable by itself.
- A single resource can be shared by multiple nodes in the scene graph.

### Font {#design_resources_font}
A font resource is used to draw text. Sceneview supports all fonts supported by
Qt. For an example on drawing text, see
`examples/example_viewer/text_renderer.cpp`

See: sv::FontResource.

### Geometry {#design_resources_geometry}
Geometry resources primarily describe the geometry of something to draw. They
can be represented as any [OpenGL
primitive](https://www.opengl.org/wiki/Primitive), such as `GL_TRIANGLES`,
`GL_LINES`, `GL_POINTS`, etc. A geometry resource can also contain per-vertex
attributes such as color and texture data.

A geometry resource is not drawable by itself. To make something show up on the
screen, you need to instantiate a geometry resource together with a material
resource into a mesh.

See: sv::GeometryResource.

### Material {#design_resources_material}
A material resource describes the visual appearance of something to draw.
Materials consist of three parts:
- A shader program
- A set of parameters to pass to the shader (e.g., color)
- A set of parameters to control OpenGL behavior (e.g., depth test).

Materials are complementary to geometries - geometry specifies the shape and
structure of what to draw, and materials determine their appearance.

See: sv::MaterialResource.

### Scene graph {#design_resources_scene}
A scene graph is also a resource. Users will typically interact with a primary
scene graph that contains the objects to be drawn in the scene.  Other data
can also be represented as a scene graph. In particular, 3D models
imported into Sceneview are represented as scene graph resources.

See: sv::Scene.

### Shader {#design_resources_shader}
A shader resource is the instantiation of an OpenGL shader program. Sceneview
currently supports vertex and fragment shaders.

Sceneview provides a small number of stock shaders that are generally useful,
and it also provides a mechanism for you to use your own custom shaders.

When using a shader, Sceneview automatically fills out and passes certain
uniform variables to the shader (e.g., the projection matrix). Other parameters
must be specified as material parameters.

See: sv::ShaderResource and sv::StockResources.

#### Materials vs Shaders

A shader is the program that ultimately controls the appearance of something
being drawn. A material contains both a reference to a shader as well as
parameters to input to the shader.

- A single shader can be used by many materials.
- Shaders can take input from both a geometry and a material. e.g., a shader
  that does lighting calculations using a single material property.

## Scene Graph {#design_scene_graph}

Sceneview maintains a fairly simple tree-based scene graph with a single root
and the following concrete node types:
- Camera node
- Group node
- Light node
- Mesh node

See: sv::Scene.

### Camera {#design_sg_camera}

Contains camera projection parameters (e.g., FOV, perspective/orthographic
parameters) as well as camera position and orientation. The scene can be drawn
from the perspective of any camera in the scene.

See: sv::CameraNode.

### Group {#design_sg_group}

A group node exists solely to contain other nodes, and is also the only node
type allowed to have children.

See: sv::GroupNode.

### Light {#design_sg_light}

Place lights in a scene graph to.. well, light up the scene and provide
somewhat realistic lighting effects.  The Sceneview-provided stock lighting
shaders determine the appearance of objects in the scene using the lights.

See: sv::LightNode.

### Mesh {#design_sg_mesh}

A mesh is the fundamental drawable unit in Sceneview, and consists of one or
more geometry / material resource pairs.

To draw something in the scene (e.g., a 3D model, a triangle, a point cloud),
it must be instantiated into a mesh.

See: sv::MeshNode.

# Qt widgets {#design_qt_widgets}

Sceneview provides a set of Qt widgets that are designed to make it quick and
easy to put together a standalone visualization tool customized to your needs.
The two most important of these are:
- sv::Viewer
- sv::Viewport

# Renderer / Input handler system {#design_renderer_input_handlers}

A renderer is a class that is tightly integrated into the Sceneview GUI.
Writing custom renderers is the typical and recommended way of drawing things
using Sceneview.

Things commonly done in a renderer:
- Insert / remove items into the scene graph
- Issue direct OpenGL commands before or after the scene graph is rendered
  (e.g., if you want to bypass the Sceneview rendering engine).
- Provide a QWidget as a UI for the renderer (e.g., to adjust the renderer's
  behavior). If provided, the renderer's widget is integrated into the
  sv::Viewer window.
- Communicate with another subsystem.

Sceneview provides a single stock renderer, sv::GridRenderer, that can be used
in your application, as well as example renderers in the `examples` directory
in the source distribution.

See: sv::Renderer.

Similarly, you can also create your own input handlers to handle mouse and
keyboard events. For example, if you don't like the Sceneview-provided camera
controls, you can build your own and swap them in. The Sceneview sv::Viewer
window provides a mechanism for switching between different input handlers.

Input handlers can also provide their own widgets to be integrated into the
Sceneview GUI.

See: sv::InputHandler.
