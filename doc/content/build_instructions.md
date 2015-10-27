Build instructions {#build_instructions}
==================

# Ubuntu

Install prerequisites

    sudo apt-get install qtbase5-dev libassimp-dev cmake build-essential git

Download and build Sceneview

    git clone https://github.com/ashuang/sceneview
    cd sceneview
    mkdir build
    cd build
    cmake ..
    make
    sudo make install
    sudo ldconfig

# OS/X (via Homebrew)

Install prerequisites

    brew install qt5 assimp cmake

Does anyone know how to automatically point CMake to the qt5 install location?

Otherwise:

    git clone https://github.com/ashuang/sceneview
    cd sceneview
    mkdir build
    cd build
    cmake -DCMAKE_PREFIX_PATH=/usr/local/Cellar/qt5/5.5.0 ..
    make
    make install

If you have a version of Qt5 other than 5.5.0, then replace the version number
as appropriate.
