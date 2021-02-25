///
/// @file Tests.cpp
/// @brief Test Driven Development (TDD)

#include <gtest/gtest.h>

#include <cstdlib>

#include "Camera.h"
#include "ImageStack.h"
#include "Mesh.h"
#include "Table.h"

// TABLE TESTS
TEST(TABLE, Constructor)
{
  Table t;
}

TEST(TABLE, Triangulate)
{
  Table t;
  std::vector<int> test = {3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
  for (size_t i = 0; i < test.size(); i++) {
    std::cout << test[i] << " ";
  }
  ASSERT_EQ(t.Triangulate("00001000"), test);
}

//CAMERA TESTS
TEST(CAMERA, Constructor)
{
  Camera c;
}

TEST(CAMERA, SetGetViewMatrix)
{
  Camera c;
  c.Initialise(1024, 720, 0);
  c.SetViewMatrix(ngl::Vec3{1.0f, 2.0f, 3.0f}, ngl::Vec3{4.0f, 5.0f, 6.0f}, ngl::Vec3{7.0f, 8.0f, 9.0f});
  ASSERT_EQ(c.GetViewMatrix(), ngl::lookAt(ngl::Vec3{1.0f, 2.0f, 3.0f}, ngl::Vec3{4.0f, 5.0f, 6.0f}, ngl::Vec3{7.0f, 8.0f, 9.0f}));
}

TEST(CAMERA, SetGetProjectionMatrix)
{
  Camera c;
  c.Initialise(1024, 720, 0);
  c.SetProjectionMatrix(10.0f, 1000, 200, 10.0f, 500.0f);
  ASSERT_EQ(c.GetProjectionMatrix(), ngl::perspective(10.0f, static_cast<float>(1000) / 200, 10.0f, 500.0f));
}

TEST(CAMERA, GetEye)
{
  Camera c;
  c.Initialise(1024, 720, 0);
  ngl::Vec3 eye = {0.0f, 100.0f, -150.0f};
  ASSERT_EQ(c.GetEye(), eye);
}

// IMAGE STACK TESTS
TEST(IMAGE_STACK, ctor)
{
  ImageStack stack;
}

TEST(IMAGE_STACK, ReadImages)
{
  ImageStack stack;
  stack.ReadImages("..\\..\\tests\\images\\readImages");
  ASSERT_EQ(stack.m_images[0], "..\\..\\tests\\images\\readImages\\black_01.png");
  ASSERT_EQ(stack.m_images[1], "..\\..\\tests\\images\\readImages\\RGBW_01.png");
  ASSERT_EQ(stack.m_images[2], "..\\..\\tests\\images\\readImages\\white_01.png");
}

TEST(IMAGE_STACK, SampleImagesBlack)
{
  ImageStack stack;
  stack.ReadImages("..\\..\\tests\\images\\black");
  stack.CheckDimensions();
  stack.SetSampleResolution(1);
  stack.SampleImages();
  // black.png
  for (size_t i = 0; i < stack.m_sampledPoints.size(); ++i)
  {
    for (size_t j = 0; j < stack.m_sampledPoints[i].size(); ++j)
    {
      ASSERT_EQ(stack.m_sampledPoints[i][j], 0);
    }
  }
}

TEST(IMAGE_STACK, SampleImagesRGBW)
{
  ImageStack stack;
  stack.ReadImages("..\\..\\tests\\images\\RGBW");
  stack.CheckDimensions();
  stack.SetSampleResolution(1);
  stack.SampleImages();

  // RGBW.png
  for (size_t i = 0; i < stack.m_sampledPoints.size(); ++i)
  {
    // Red
    for (int r = 0; r < 40000; ++r)
    {
      ASSERT_EQ(stack.m_sampledPoints[i][r], 255);
    }
    // Green
    for (int g = 40000; g < 80000; ++g)
    {
      ASSERT_EQ(stack.m_sampledPoints[i][g], 0);
    }
    // Blue
    for (int b = 80000; b < 120000; ++b)
    {
      ASSERT_EQ(stack.m_sampledPoints[i][b], 0);
    }
    // White
    for (int w = 120000; w < 160000; ++w)
    {
      ASSERT_EQ(stack.m_sampledPoints[i][w], 255);
    }
  }
}

TEST(IMAGE_STACK, SampleImagesWhite)
{
  ImageStack stack;
  stack.ReadImages("..\\..\\tests\\images\\white");
  stack.CheckDimensions();
  stack.SetSampleResolution(1);
  stack.SampleImages();
  // black.png
  for (size_t i = 0; i < stack.m_sampledPoints.size(); ++i)
  {
    for (size_t j = 0; j < stack.m_sampledPoints[i].size(); ++j)
    {
      ASSERT_EQ(stack.m_sampledPoints[i][j], 255);
    }
  }
}

TEST(IMAGE_STACK, SetGetSampleResolution)
{
  ImageStack stack;
  stack.ReadImages("..\\..\\images\\MRI\\test_data");
  stack.CheckDimensions();
  stack.SetSampleResolution(3);
  ASSERT_EQ(stack.GetSampleResolution(), 3);
}

TEST(IMAGE_STACK, GetImageWidth)
{
  ImageStack stack;
  stack.ReadImages("..\\..\\tests\\images\\white");
  stack.CheckDimensions();
  ASSERT_EQ(stack.GetImageWidth(), 400);
}

TEST(IMAGE_STACK, GetImageHeight)
{
  ImageStack stack;
  stack.ReadImages("..\\..\\tests\\images\\white");
  stack.CheckDimensions();
  ASSERT_EQ(stack.GetImageHeight(), 200);
}

// MESH TESTS
TEST(MESH, Constructor)
{
  Mesh m;
}

TEST(MESH, Initialise)
{
  std::vector<std::vector<int>> test;
  Mesh m;
  m.Initialise(test, 100, 200, 20);
}

TEST(MESH, SetGetSurfaceLevel)
{
  std::vector<std::vector<int>> test;
  Mesh m;
  m.Initialise(test, 100, 200, 20);
  m.SetSurfaceLevel(100);
  ASSERT_EQ(m.GetSurfaceLevel(), 100);
}