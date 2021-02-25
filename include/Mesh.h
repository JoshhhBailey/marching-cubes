/// \file Mesh.h
/// \brief Implementation of Marching Cubes algorithm / mesh generation
/// \author Josh Bailey
/// \version 1.0
/// \date 13/01/21 Updated to NCCA Coding Standard
/// Revision History:
///
/// \todo

#ifndef MESH_H_
#define MESH_H_

#include <ngl/Vec3.h>

#include <vector>

#include "Table.h"

class Mesh
{
  public:
    Mesh() = default;
    void Initialise(std::vector<std::vector<int>> _pointData, unsigned int _imageWidth, unsigned int _imageHeight, unsigned int _sampleResolution);
    // Perform Marching Cubes algorithm
    std::vector<ngl::Vec3> MarchCubes();

    // Setters and getters
    void SetSurfaceLevel(int _surfaceLevel);
    int GetSurfaceLevel() { return m_surfaceLevel; }

  private:
    std::vector<std::vector<int>> m_pointData;
    int m_surfaceLevel;

    // Image dimensions
    unsigned int m_imageWidth;
    unsigned int m_imageHeight;

    // The frequency of which colour values are read from each image
    unsigned int m_sampleResolution;

    // See MarchCubes() in Mesh.cpp for explanation and implementation
    unsigned int m_pointsPerRow;
    unsigned int m_columns;
    unsigned int m_totalSquares;

    Table m_table;
    // Midpoint offset of each edge
    float m_offset;
    // Stores vertex data of triangles to draw
    std::vector<ngl::Vec3> m_vertexData;

    // Scale of mesh from pixel to coordinate space
    float m_meshScale = 0.1f;

    // Process errors
    void ErrorMessage(std::string _type, std::string _line1, std::string _line2 = "");
};

#endif  // _MESH_H_
