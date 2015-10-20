// Copyright [2015] Albert Huang

#include <gtest/gtest.h>

#include "sceneview/axis_aligned_box.hpp"

using sv::AxisAlignedBox;

TEST(AxisAlignedBox, OnePoint) {
  for (double x = -100; x < 100; ++x) {
    AxisAlignedBox box;
    const QVector3D point(x, 0, 0);
    box.IncludePoint(point);
    EXPECT_EQ(point, box.Min());
    EXPECT_EQ(point, box.Max());
  }
  for (double y = -100; y < 100; ++y) {
    AxisAlignedBox box;
    const QVector3D point(0, y, 0);
    box.IncludePoint(point);
    EXPECT_EQ(point, box.Min());
    EXPECT_EQ(point, box.Max());
  }
  for (double z = -100; z < 100; ++z) {
    AxisAlignedBox box;
    const QVector3D point(0, 0, z);
    box.IncludePoint(point);
    EXPECT_EQ(point, box.Min());
    EXPECT_EQ(point, box.Max());
  }
  for (double p = -100; p < 100; ++p) {
    AxisAlignedBox box;
    const QVector3D point(p, p, p);
    box.IncludePoint(point);
    EXPECT_EQ(point, box.Min());
    EXPECT_EQ(point, box.Max());
  }
}
