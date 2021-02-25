/// \file ImageStack.h
/// \brief Reading in and checking image data
/// \author Josh Bailey
/// \version 1.0
/// \date 16/01/21 Bug fixes
/// Revision History:
///
/// \todo

#ifndef IMAGE_STACK_H_
#define IMAGE_STACK_H_

#include <sstream>
#include <string>
#include <vector>

class ImageStack
{
  public:
    ImageStack() = default;

    // Read and write image paths from a directory to m_images
    void ReadImages(const std::string _imageDirectory);
    // Check all images have the same dimensions
    void CheckDimensions();
    // Sample each images colour data to store in m_sampledPoints
    void SampleImages();

    // Setters and getters
    void SetSampleResolution(int _resolution);
    unsigned int GetSampleResolution() { return m_sampleResolution; }
    unsigned int GetImageWidth() { return m_imageWidth; }
    unsigned int GetImageHeight() { return m_imageHeight; }
    bool CheckSampledImages() { return m_sampledImages; }

    // Store the paths of all images
    std::vector<std::string> m_images;
    // Store the colour value of each point, within each image
    std::vector<std::vector<int>> m_sampledPoints;

  private:
    // Image dimensions
    unsigned int m_imageWidth;
    unsigned int m_imageHeight;

    // The frequency of which colour values are read from each image
    unsigned int m_sampleResolution = 1;

    // Function checks
    bool m_directoryChecked = false;
    bool m_correctDimensions = false;

    // Already ran these functions?
    bool m_checkedDimensions = false;
    bool m_sampledImages = false;

    // Process errors
    void ErrorMessage(std::string _type, std::string _line1, std::string _line2 = "");
    std::stringstream m_ss;
    std::string m_s;
    std::string m_output;
};

#endif  // _IMAGE_STACK_H_
