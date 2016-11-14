Build instructions {#build_instructions}
==================

[TOC]

# Ubuntu {#build_instructions_ubuntu}

## Ubuntu 15.04+ {#build_instructions_ubuntu_vivid}

Install prerequisites

    sudo apt-get install build-essential cmake git libassimp-dev qtbase5-dev

Download and build Sceneview

    git clone https://github.com/ashuang/sceneview
    cd sceneview
    mkdir build
    cd build
    cmake ..
    make
    sudo make install
    sudo ldconfig

## Ubuntu 14.04 LTS {#build_instructions_ubuntu_trusty}

Install prerequisites. The version of Qt 5 shipped with stock Ubuntu 14.04 is
too old, so you need to install an newer version from elsewhere. The way we
recommend is:

    sudo apt-add-repository ppa:beineri/opt-qt55-trusty
    sudo apt-get update
    sudo apt-get install qt-latest

Install the other dependencies and build tools

    sudo apt-get install build-essential cmake git libassimp-dev libgl1-mesa-dev

Download and build Sceneview. You'll need to point CMake to the nonstandard
install location for Qt5 in /opt.

    git clone https://github.com/ashuang/sceneview
    cd sceneview
    mkdir build
    cd build
    cmake -DCMAKE_PREFIX_PATH=/opt/qt55 ..
    make
    sudo make install
    sudo ldconfig

# OS/X (via Homebrew) {#build_instructions_osx_homebrew}

Install prerequisites

    brew install qt5 assimp cmake

Does anyone know how to automatically point CMake to the qt5 install location?

Otherwise:

    git clone https://github.com/ashuang/sceneview
    cd sceneview
    mkdir build
    cd build
    cmake -DCMAKE_PREFIX_PATH=/usr/local/Cellar/qt5/5.5.1 ..
    make
    make install

If you have a version of Qt5 other than 5.5.1, then replace the version number
as appropriate.

# Compiling and linking against Sceneview

Sceneview provides a pkg-config file. You can obtain the required compiler
flags by running:

```
pkg-config --cflags sceneview
```

The linker flags can be obtained by:
```
pkg-config --libs sceneview
```

## Including header files

The easiest way include all the right header files for Sceneview is to include
the master header file:

```
#include <sceneview/sceneview.hpp>
```

If you want to slightly optimize compile times, you can optionally include only
the header files you need. The class documentation indicates what headers to
include for each Sceneview class.
