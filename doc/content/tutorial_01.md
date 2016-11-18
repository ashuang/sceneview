Tutorial 01 - Hello {#tutorial_01}
===========

There are three major components of Sceneview:
- Rendering engine
- Qt widgets for building user interfaces.
- Renderer / Input handler system

This first tutorial describes how to use one of the Sceneview Qt widgets to
create a GUI. Later tutorials will introduce the other components.

@include tutorial_01/tutorial_01_hello.cpp

If you build and run this example, you would hopefully see something like:

@image html tutorial_01.png "tutorial_01_hello"

This first snippet of code is our version of "Hello, world." sv::Viewer is a
QMainWindow that bundles together a few commonly used widgets:
- A viewport that contains the 3D rendering area
- A couple more widgets that can be used for controlling rendering options and input handling (RendererWidgetStack and InputHandlerStack).

There's nothing much to see yet other than the widgets showing up. We'll get to
those in later tutorials.

In advanced usage, you don't need to use sv::Viewer, and you can create your
own QMainWindow and mix-and-match Sceneview widgets however you like. However,
sv::Viewer can be useful for prototyping and for throwing together something quick-and-dirty.
