Tutorial 02 - Bulitin renderers and Input handlers {#tutorial_02}
===========

Now that we have a basic UI up and running, we can start drawing.

This tutorial introduces the following:
- Viewport
- Renderers
- Input handlers

# Tutorial code

@include tutorial_02/tutorial_02_builtin.cpp

# Running the code

If you build and run this example, you'll hopefully see something like:

@image html tutorial_02.png "tutorial_02_builtin"

Try interacting with the 3D rendering widget by cilcking and dragging with the
mouse. You should be able to move the camera around, panning, zooming,
rotating.

# Viewport

sv::Viewport is a Qt Widget that provides an OpenGL drawing context. It does so
by subclassing QOpenGLWidget, and adding additional functionality for a typical
rendering pipeline, and maintains things like a default camera, the renderers,
input handlers, etc. You'll get to know this one more later, but for now, it's
enough to know that:

- sv::Viewport _is the widget with the OpenGL context and where all the drawing happens_.

# Renderer

A sv::Renderer in Sceneview is the recommended way of grouping together related
drawing functionality. For example, you might have one renderer for drawing a
terrain model. You might have another renderer for drawing a robot model, and
perhaps a third renderer for drawing image data. The sv::Renderer class is
generally intended to be subclassed by Sceneview users.

Sceneview provides one builtin renderer, sv::GridRenderer. It's best use case
is probably to serve as a standin renderer while you get something else up and
running, and can be a useful way to verify that 3D drawing works in Sceneview.

Future tutorials will show you how to create your own renderers.

## Renderer widget

A common pattern is for a renderer to have a set of parameters that control its
behavior. For example, you could have a parameter that controls the
transparency of a 3D model, or switches from solid mode to wireframe mode. Each
sv::Renderer can optionally provide a QWidget. If this widget is provided, then
it will appear under the "Renderers" widget in the viewer window. See
sv::Renderer API for more information.

The builtin sv::GridRenderer does not provide a widget, but future tutorials
will demonstrate how to do so.

# Input handler

A sv::InputHandler is complementary to a sv::Renderer. Whereas sv::Renderer is
the recommended way to group rendering functionality, sv::InputHandler is the
recommended way to group together functionality for handling keyboard/mouse
input. A common pattern might be to have multiple "tools". For example, you
might have one tool for moving the camera around the scene, another for
measuring distance, another for selecting an object in the scene. Each of these
could be its own sv::InputHandler.

Sceneview provides one builtin input handler, sv::ViewHandlerHorizontal that
controls the camera based on mouse input.

@note Input handlers can also draw to the viewport.

## Input handler widget

Similar to renderers, each sv::InputHandler can also provide a widget for
exposing and controlling behavior.

The builtin sv::ViewHandlerHorizontal provides a widget that allows switching
the camera between orthographic and perspective projections. Try switching them
back and forth to see how they change the camera view.
