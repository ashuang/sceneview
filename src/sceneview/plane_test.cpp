// Copyright [2015] Albert Huang

#include <gtest/gtest.h>

#include "sceneview/plane.hpp"

using sv::Plane;

const double kEpsilon = 1e-6;

#define SDIST_CHECK(plane, x, y, z, expected) \
  EXPECT_NEAR(expected, plane.SignedDistance(QVector3D(x, y, z)), kEpsilon)

TEST(Plane, SignedDistance) {
  Plane z0_up(0, 0, 1, 0);
  SDIST_CHECK(z0_up, 0, 0, 1, 1);
  SDIST_CHECK(z0_up, 0, 0, -1, -1);
  SDIST_CHECK(z0_up, 0, 0, 10, 10);
  SDIST_CHECK(z0_up, 0, 0, -10, -10);

  Plane z0_down(0, 0, -2, 2);
  SDIST_CHECK(z0_down, 0, 0, 1, 0);
  SDIST_CHECK(z0_down, 0, 0, -1, 2);
  SDIST_CHECK(z0_down, 0, 0, 10, -9);
  SDIST_CHECK(z0_down, 0, 0, -10, 11);

  Plane x0_up(1, 0, 0, 0);
  SDIST_CHECK(x0_up, 1, 0, 0, 1);
  SDIST_CHECK(x0_up, -1, 0, 0, -1);
  SDIST_CHECK(x0_up, 10, 0, 0, 10);
  SDIST_CHECK(x0_up, -10, 0, 0, -10);

  Plane x0_down(-2, 0, 0, 2);
  SDIST_CHECK(x0_down, 1, 0, 0, 0);
  SDIST_CHECK(x0_down, -1, 0, 0, 2);
  SDIST_CHECK(x0_down, 10, 0, 0, -9);
  SDIST_CHECK(x0_down, -10, 0, 0, 11);

  Plane y0_up(0, 1, 0, 0);
  SDIST_CHECK(y0_up, 0, 1, 0, 1);
  SDIST_CHECK(y0_up, 0, -1, 0, -1);
  SDIST_CHECK(y0_up, 0, 10, 0, 10);
  SDIST_CHECK(y0_up, 0, -10, 0, -10);

  Plane y0_down(0, -2, 0, 2);
  SDIST_CHECK(y0_down, 0, 1, 0, 0);
  SDIST_CHECK(y0_down, 0, -1, 0, 2);
  SDIST_CHECK(y0_down, 0, 10, 0, -9);
  SDIST_CHECK(y0_down, 0, -10, 0, 11);
}
