Tutorials {#tutorials}
=========

Tutorial source code is provided in the Sceneview source distribution, in the
top-level "tutorials" directory.

1. \ref tutorial_01
2. \ref tutorial_02
3. \ref tutorial_03

# Build and running tutorials {#tutorials_building}

From the sceneview source tree:

@code
mkdir tutorial_build
cd tutorial_build
cmake ../tutorial
make
@endcode

Each tutorial binary is placed in its own subdirectory. For example, to run
tutorial 01, you could then run:
@code
tutorial_01/tutorial_01_hello
@endcode
