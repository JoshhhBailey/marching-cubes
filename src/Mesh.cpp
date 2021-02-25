///
/// @file Mesh.cpp
/// @brief Implementation of Marching Cubes algorithm / mesh generation

#include <algorithm>
#include <iostream>
#include <string>

#include "Mesh.h"

void Mesh::Initialise(std::vector<std::vector<int>> _pointData, unsigned int _imageWidth, unsigned int _imageHeight, unsigned int _sampleResolution)
{
  m_pointData = _pointData;
  m_imageHeight = _imageHeight;
  m_imageWidth = _imageWidth;
  m_sampleResolution = _sampleResolution;

  m_pointsPerRow = m_imageWidth / m_sampleResolution;
  m_columns = m_imageHeight / m_sampleResolution;
  m_totalSquares = (m_pointsPerRow - 1) * (m_columns - 1);

  m_offset = m_sampleResolution / 2.0f;

  std::cout << "Mesh initialised!\n";
}

std::vector<ngl::Vec3> Mesh::MarchCubes()
{
  // Clear previous data
  m_vertexData.clear();
  
  std::cout << "Marching cubes...\n";
  // Example:
  // m_pointData[0] = 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16
  // The vector actually represents a 4x4 grid of sampled points...
  //     1  2  3  4
  //     5  6  7  8
  //     9 10 11 12
  //    13 14 15 16
  // Their are 9 total squares...
  //    1 2   2 3   3 4   5  6    6  7    7  8    9 10   10 11   11 12
  //    5 6   6 7   7 8   9 10   10 11   11 12   13 14   14 15   15 16
  // Parallel squares from 'z' and 'z + 1' create a cube...
  for (int z = 0; z < m_pointData.size() - 1; ++z)
  {
    // Points must go clockwise so the binary conversion triangulates
    // to match Bourkes (1994) triangulation table...
    //    Layer A              Layer B
    //    p0 -> p1             p4 -> p5
    //          |            /       |
    //          v          /         v
    //    p3 <- p2  ... p3     p7 -> p6
    int p0_index = -1;                  // Top left (TL)
    int p1_index = 0;                   // Top right (TR)
    int p2_index = 0 + m_pointsPerRow;  // Bottom right (BR)
    int p3_index = -1 + m_pointsPerRow; // Bottom left (BL)
    int count = 0;

    for (unsigned int i = 0; i < m_totalSquares; ++i)
    {
      //std::cout << "Square: " << i << "\n";

      // For every 3 points, skip 1. For example p0_index will be...
      //    0, 1, 2, . 4, 5, 6, . 8,  9, 10, . 12, 13, 14, .
      // These indexes are equivalent to the top left point of each square (listed above)...
      //    1, 2, 3, . 5, 6, 7, . 9, 10, 11, . 13, 14, 15, .
      if (count == m_pointsPerRow - 1)
      {
        p0_index += 2;
        p1_index += 2;
        p2_index += 2;
        p3_index += 2;
        count = 0;
      }
      else
      {
        p0_index++;
        p1_index++;
        p2_index++;
        p3_index++;
      }
      // Layer A
      int p0 = m_pointData[z][p0_index];
      int p1 = m_pointData[z][p1_index]; 
      int p2 = m_pointData[z][p2_index]; 
      int p3 = m_pointData[z][p3_index]; 
      // Layer B
      int p4 = m_pointData[z + 1][p0_index]; 
      int p5 = m_pointData[z + 1][p1_index]; 
      int p6 = m_pointData[z + 1][p2_index]; 
      int p7 = m_pointData[z + 1][p3_index]; 
      count++;

      // Convert points into a binary string
      std::vector<int> points = {p0, p1, p2, p3, p4, p5, p6, p7};
      std::string binary = "";

      for (size_t i = 0; i < points.size(); ++i)
      {
        if (points[i] >= m_surfaceLevel)
        {
          binary += "1";
        }
        else
        {
          binary += "0";
        }
      }
      reverse(binary.begin(), binary.end());

      // Return edges that need to be connected
      std::vector<int> edges = m_table.Triangulate(binary);

      for (size_t i = 0; i < edges.size(); ++i)
      {
        // All edges have been read
        if (edges[i] == -1)
        {
          break;
        }
        // Calculate coordinates of edge (at midpoint), add to vertexData vector
        // Edge positions yet again based off of Bourkes (1994) methodology
        // See cube diagram at: http://paulbourke.net/geometry/polygonise/
        else
        {
          switch(edges[i])
          {
            case 0:
            {
              ngl::Vec3 e0 = {static_cast<ngl::Real>(((p0_index % m_pointsPerRow) * m_sampleResolution) + m_offset) - m_imageWidth / 2.0f,
                              static_cast<ngl::Real>((p0_index / m_pointsPerRow) * m_sampleResolution) - m_imageHeight / 2.0f,
                              static_cast<ngl::Real>(z * m_sampleResolution) - (m_pointData.size() * (m_sampleResolution / 2.0f))};
              e0 *= m_meshScale;
              m_vertexData.push_back(e0);
              break;
            }

            case 1:
            {
              ngl::Vec3 e1 = {static_cast<ngl::Real>((p1_index % m_pointsPerRow) * m_sampleResolution) - m_imageWidth / 2.0f,
                              static_cast<ngl::Real>(((p1_index / m_pointsPerRow) * m_sampleResolution) + m_offset) - m_imageHeight / 2.0f,
                              static_cast<ngl::Real>(z * m_sampleResolution) - (m_pointData.size() * (m_sampleResolution / 2.0f))};
              e1 *= m_meshScale;
              m_vertexData.push_back(e1);
              break;
            }

            case 2:
            {
              ngl::Vec3 e2 = {static_cast<ngl::Real>(((p3_index % m_pointsPerRow) * m_sampleResolution) + m_offset) - m_imageWidth / 2.0f,
                              static_cast<ngl::Real>((p3_index / m_pointsPerRow) * m_sampleResolution) - m_imageHeight / 2.0f,
                              static_cast<ngl::Real>(z * m_sampleResolution) - (m_pointData.size() * (m_sampleResolution / 2.0f))};
              e2 *= m_meshScale;
              m_vertexData.push_back(e2);
              break;
            }

            case 3:
            {
              ngl::Vec3 e3 = {static_cast<ngl::Real>((p0_index % m_pointsPerRow) * m_sampleResolution) - m_imageWidth / 2.0f,
                              static_cast<ngl::Real>(((p0_index / m_pointsPerRow) * m_sampleResolution) + m_offset) - m_imageHeight / 2.0f,
                              static_cast<ngl::Real>(z * m_sampleResolution) - (m_pointData.size() * (m_sampleResolution / 2.0f))};
              e3 *= m_meshScale;
              m_vertexData.push_back(e3);
              break;
            }

            case 4:
            {
              ngl::Vec3 e4 = {static_cast<ngl::Real>(((p0_index % m_pointsPerRow) * m_sampleResolution) + m_offset) - m_imageWidth / 2.0f,
                              static_cast<ngl::Real>((p0_index / m_pointsPerRow) * m_sampleResolution) - m_imageHeight / 2.0f,
                              static_cast<ngl::Real>((z + 1) * m_sampleResolution) - (m_pointData.size() * (m_sampleResolution / 2.0f))};
              e4 *= m_meshScale;
              m_vertexData.push_back(e4);
              break;
            }

            case 5:
            {
              ngl::Vec3 e5 = {static_cast<ngl::Real>((p1_index % m_pointsPerRow) * m_sampleResolution) - m_imageWidth / 2.0f,
                              static_cast<ngl::Real>(((p1_index / m_pointsPerRow) * m_sampleResolution) + m_offset) - m_imageHeight / 2.0f,
                              static_cast<ngl::Real>((z + 1) * m_sampleResolution) - (m_pointData.size() * (m_sampleResolution / 2.0f))};
              e5 *= m_meshScale;
              m_vertexData.push_back(e5);
              break;
            }

            case 6:
            {
              ngl::Vec3 e6 = {static_cast<ngl::Real>(((p3_index % m_pointsPerRow) * m_sampleResolution) + m_offset) - m_imageWidth / 2.0f,
                              static_cast<ngl::Real>((p3_index / m_pointsPerRow) * m_sampleResolution) - m_imageHeight / 2.0f,
                              static_cast<ngl::Real>((z + 1) * m_sampleResolution) - (m_pointData.size() * (m_sampleResolution / 2.0f))};
              e6 *= m_meshScale;
              m_vertexData.push_back(e6);
              break;
            }

            case 7:
            {
              ngl::Vec3 e7 = {static_cast<ngl::Real>((p0_index % m_pointsPerRow) * m_sampleResolution) - m_imageWidth / 2.0f,
                              static_cast<ngl::Real>(((p0_index / m_pointsPerRow) * m_sampleResolution) + m_offset) - m_imageHeight / 2.0f,
                              static_cast<ngl::Real>((z + 1) * m_sampleResolution) - (m_pointData.size() * (m_sampleResolution / 2.0f))};
              e7 *= m_meshScale;
              m_vertexData.push_back(e7);
              break;
            }

            case 8:
            {
              ngl::Vec3 e8 = {static_cast<ngl::Real>((p0_index % m_pointsPerRow) * m_sampleResolution) - m_imageWidth / 2.0f,
                              static_cast<ngl::Real>((p0_index / m_pointsPerRow) * m_sampleResolution) - m_imageHeight / 2.0f,
                              static_cast<ngl::Real>((z * m_sampleResolution) + m_offset) - (m_pointData.size() * (m_sampleResolution / 2.0f))};
              e8 *= m_meshScale;
              m_vertexData.push_back(e8);
              break;
            }

            case 9:
            {
              ngl::Vec3 e9 = {static_cast<ngl::Real>((p1_index % m_pointsPerRow) * m_sampleResolution) - m_imageWidth / 2.0f,
                              static_cast<ngl::Real>((p1_index / m_pointsPerRow) * m_sampleResolution) - m_imageHeight / 2.0f,
                              static_cast<ngl::Real>((z * m_sampleResolution) + m_offset) - (m_pointData.size() * (m_sampleResolution / 2.0f))};
              e9 *= m_meshScale;
              m_vertexData.push_back(e9);
              break;
            }

            case 10:
            {
              ngl::Vec3 e10 = {static_cast<ngl::Real>((p2_index % m_pointsPerRow) * m_sampleResolution) - m_imageWidth / 2.0f,
                               static_cast<ngl::Real>((p2_index / m_pointsPerRow) * m_sampleResolution) - m_imageHeight / 2.0f,
                               static_cast<ngl::Real>((z * m_sampleResolution) + m_offset) - (m_pointData.size() * (m_sampleResolution / 2.0f))};
              e10 *= m_meshScale;
              m_vertexData.push_back(e10);
              break;
            }

            case 11:
            {
              ngl::Vec3 e11 = {static_cast<ngl::Real>((p3_index % m_pointsPerRow) * m_sampleResolution) - m_imageWidth / 2.0f,
                               static_cast<ngl::Real>((p3_index / m_pointsPerRow) * m_sampleResolution) - m_imageHeight / 2.0f,
                               static_cast<ngl::Real>((z * m_sampleResolution) + m_offset) - (m_pointData.size() * (m_sampleResolution / 2.0f))};
              e11 *= m_meshScale;
              m_vertexData.push_back(e11);
              break;
            }

            default:
            {
              break;
            }
          }
        }
      }
    }
  }
  std::cout << "Cubes marched!\n";
  return m_vertexData;
}

void Mesh::SetSurfaceLevel(int _surfaceLevel)
{
  if (_surfaceLevel < 0)
  {
    ErrorMessage("SURFACE LEVEL ERROR", "Surface level is too small.", "The minimum surface level is: 0");
  }
  else if (_surfaceLevel > 255)
  {
    ErrorMessage("SURFACE LEVEL ERROR", "Surface level is too large.", "The maximum surface level is: 255");
  }
  else
  {
    m_surfaceLevel = _surfaceLevel;
  }
}

void Mesh::ErrorMessage(std::string _type, std::string _line1, std::string _line2)
{
  std::cout << "==============================================\n"
            << _type << ":\n"
            << "      " << _line1 << '\n'
            << "      " << _line2 << '\n'
            << "==============================================\n";
}