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

TEST(AxisAlignedBox, Intersection) {
  AxisAlignedBox box0(QVector3D(0, 0, 0), QVector3D(3, 3, 3));
  AxisAlignedBox box1(QVector3D(4, 4, 4), QVector3D(7, 7, 7));
  EXPECT_FALSE(box0.Intersection(box1).Valid());
  EXPECT_FALSE(box1.Intersection(box0).Valid());

  // Box 0 intersect itself
  AxisAlignedBox box00 = box0.Intersection(box0);
  EXPECT_EQ(box00.Min(), box0.Min());
  EXPECT_EQ(box00.Max(), box0.Max());

  // Box 2 completely inside box 0
  AxisAlignedBox box2(QVector3D(1, 1, 1), QVector3D(2, 2, 2));
  AxisAlignedBox box02 = box0.Intersection(box2);
  AxisAlignedBox box20 = box2.Intersection(box0);
  EXPECT_EQ(box02.Min(), box20.Min());
  EXPECT_EQ(box02.Max(), box20.Max());
  EXPECT_EQ(box02.Min(), QVector3D(1, 1, 1));
  EXPECT_EQ(box02.Max(), QVector3D(2, 2, 2));

  // Box 3 partial intersection with box 0
  AxisAlignedBox box3(QVector3D(2, 2, 2), QVector3D(5, 5, 5));
  AxisAlignedBox box03 = box0.Intersection(box3);
  AxisAlignedBox box30 = box3.Intersection(box0);
  EXPECT_EQ(box03.Min(), box30.Min());
  EXPECT_EQ(box03.Max(), box30.Max());
  EXPECT_EQ(box03.Min(), QVector3D(2, 2, 2));
  EXPECT_EQ(box03.Max(), QVector3D(3, 3, 3));

  // Box 3 partial intersection with box 1
  AxisAlignedBox box13 = box1.Intersection(box3);
  AxisAlignedBox box31 = box3.Intersection(box1);
  EXPECT_EQ(box13.Min(), box31.Min());
  EXPECT_EQ(box13.Max(), box31.Max());
  EXPECT_EQ(box13.Min(), QVector3D(4, 4, 4));
  EXPECT_EQ(box13.Max(), QVector3D(5, 5, 5));

  // Intersection of adjoining boxes should be the adjoining face.
  AxisAlignedBox box4(QVector3D(0, 0, 3), QVector3D(3, 3, 6));
  AxisAlignedBox box04 = box0.Intersection(box4);
  AxisAlignedBox box40 = box4.Intersection(box0);
  EXPECT_EQ(box04.Min(), box40.Min());
  EXPECT_EQ(box04.Max(), box40.Max());
  EXPECT_EQ(box04.Min(), QVector3D(0, 0, 3));
  EXPECT_EQ(box04.Max(), QVector3D(3, 3, 3));

  // Empty box intersecting itself.
  AxisAlignedBox box5(QVector3D(0, 0, 0), QVector3D(0, 0, 0));
  AxisAlignedBox box55 = box5.Intersection(box5);
  EXPECT_EQ(box55.Min(), box5.Min());
  EXPECT_EQ(box55.Max(), box5.Max());
}
