cmake_minimum_required(VERSION 3.10.0)

project(sceneview)

include(GNUInstallDirs)

# export the compile commands that CMake uses. This is for code completion
# tools like YouCompleteMe
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

find_package(Qt5Widgets)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Werror -Wno-inconsistent-missing-override ${CMAKE_CXX_FLAGS}")

include_directories(src)

if(APPLE)
  set(OPENGL_LIBS "-framework OpenGL")
else()
  set(OPENGL_LIBS "GL")
endif()

set(SV_VERSION 0.0.1)

set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)


# Look for gtest in
#   /usr/src/gtest
#
# This is the case on Ubuntu with the package libgtest-dev installed
#
# Unlike most 3rd-party libraries, gtest should be built using
# exactly the compiler flags used to build our own binaries. This is why we
# compile the libraries with our project instead of linking to pre-compiled
# binaries.
set(GTEST_DIR /usr/src/gtest)
set(HAVE_GTEST FALSE)

if(EXISTS ${GTEST_DIR})
  find_package(Threads REQUIRED)

  include_directories(${GTEST_DIR})

  add_library(gtest
    ${GTEST_DIR}/src/gtest-all.cc)
  target_link_libraries(gtest ${CMAKE_THREAD_LIBS_INIT})

  add_library(gtest_main
    ${GTEST_DIR}/src/gtest_main.cc)
  target_link_libraries(gtest_main ${CMAKE_THREAD_LIBS_INIT})

  set(HAVE_GTEST TRUE)
  message("Found gtest. Unit tests will be built")
  enable_testing()
endif()

add_subdirectory(src)
add_subdirectory(examples)
