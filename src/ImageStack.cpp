///
/// @file ImageStack.cpp
/// @brief Reading in and checking image data

#include <QColor>
#include <QImage>

#include <algorithm>
#include <filesystem>
#include <iostream>

#include "ImageStack.h"

void ImageStack::ReadImages(const std::string _imageDirectory)
{
  // The following function is adapted from:
  // Garland, J. and Dawes, B., 2002. simple_ls program.
  // Available from: https://www.boost.org/doc/libs/1_36_0/libs/filesystem/example/simple_ls.cpp
  // Accessed [15 December 2020]

  std::cout << "Reading images...\n";
  // Clear previous data
  m_images.clear();
  m_directoryChecked = false;
  m_correctDimensions = false;
  m_checkedDimensions = false;
  m_sampledImages = false;

  // Create directory path
  const std::filesystem::path directoryPath = _imageDirectory;

  // If directory doesn't exist
  if (!std::filesystem::exists(directoryPath))
  {
    ErrorMessage("PATH ERROR", "The following path was not found: ", _imageDirectory);
  }
  else
  {
    if (std::filesystem::is_directory(directoryPath))
    {
      std::cout << "Directory found!" << "\n";

      // Iterate directory
      std::filesystem::directory_iterator end_itr;
      for (std::filesystem::directory_iterator dir_itr(directoryPath);
          dir_itr != end_itr;
          ++dir_itr)
          {

        // Print file path
        try
        {
          if (std::filesystem::is_regular_file(dir_itr->status()))
          {
            std::cout << "Found file: " << dir_itr->path() << "\n";
            m_images.push_back(dir_itr->path().string());
          }
        }
        // Print error
        catch (const std::exception &ex )
        {
          std::cout << dir_itr->path() << " " << ex.what() << std::endl;
        }
      }
      if (m_images.size() == 0)
      {
        m_output = _imageDirectory + " is empty!";
        ErrorMessage("DIRECTORY ERROR", m_output);
      }
      else if (m_images.size() > 1)
      {
        m_directoryChecked = true;
      }
      else
      {
        m_output = _imageDirectory + " only contains 1 image.";
        ErrorMessage("DIRECTORY ERROR", m_output, "At least 2 images are required.");
      }
    }
    else
    {
      m_output = _imageDirectory + " is not a directory!";
      ErrorMessage("DIRECTORY ERROR", m_output);
    }
    std::cout << "Images read!\n";
  }
}

void ImageStack::CheckDimensions()
{
  // Clear previous data
  m_imageWidth = 0;
  m_imageHeight = 0;
  
  if (m_directoryChecked)
  {
    std::cout << "Checking image dimensions..." << "\n";
    m_directoryChecked = false;

    // For each image
    for (int i = 0; i < m_images.size(); ++i)
    {
      QImage img(m_images[i].c_str());
      

      // Get dimensions of first image
      if (m_imageWidth == 0 && m_imageHeight == 0)
      {
        m_imageWidth = img.width();
        m_imageHeight = img.height();
      }

      // Check all images have the same dimensions
      if (img.width() != m_imageWidth || img.height() != m_imageHeight)
      {
        m_ss << m_images[i];
        m_ss >> m_s;
        m_output = m_s + " has different dimensions!";
        ErrorMessage("IMAGE CHECK ERROR", m_output, "Please ensure all images are the same width and height.");
        m_ss.clear();
        m_s.clear();
        m_correctDimensions = false;
        m_checkedDimensions = false;
        return;
      }
      std::cout << m_images[i] << " checked!" << "\n";
    }
    m_correctDimensions = true;
    m_checkedDimensions = true;
    std::cout << "Image dimensions checked!\n";
  }
  else if (!m_checkedDimensions)
  {
    ErrorMessage("IMAGE CHECK ERROR", "Image dimensions cannot be checked.", "Please ensure your directory path is correct and populated.");
  }
  else
  {
    std::cout << "Image dimensions have already been checked successfully!" << "\n";
  }
}

void ImageStack::SampleImages()
{
  if (m_correctDimensions)
  {
    std::cout << "Sampling images..." << "\n";
    // Clear previous data
    m_sampledPoints.clear();
    
    m_correctDimensions = false;
    m_sampledImages = true;
    // For each image (sampled at m_sampleResolution, not every layer)
    for (int i = 0; i < m_images.size(); i += m_sampleResolution)
    {
      QImage img(m_images[i].c_str());
      std::vector<int> points;

      // Loop image
      for (unsigned int y = 0; y < m_imageHeight; y += m_sampleResolution)
      {
        for (unsigned int x = 0; x < m_imageWidth; x += m_sampleResolution)
        {
          QColor col(img.pixel(x, y));

          // Sample one colour channel (all channels have equal values)
          int r;
          r = col.red();
          points.push_back(r);
        }
      }
      m_sampledPoints.push_back(points);
      std::cout << m_images[i] << " sampled!\n";

      // Avoid overflow
      if (i + m_sampleResolution >= m_images.size())
      {
        break;
      }
    }
    std::cout << "Images sampled!\n";
  }
  else if (!m_sampledImages)
  {
    ErrorMessage("IMAGE SAMPLE ERROR", "Images have not been cleared to sample.", "Please check all image dimensions are equal first.");
  }
  else
  {
    std::cout << "Images have already been sampled successfully!" << "\n";
  }
}

void ImageStack::SetSampleResolution(int _resolution)
{
  if (m_checkedDimensions)
  {
    std::vector<size_t> data = {m_imageWidth, m_imageHeight, m_images.size()};
    std::sort(data.begin(), data.end());

    // Maximum resolution is 10% of the smallest value
    if (_resolution > data[0] / 10.0f)
    {
      m_ss << ceil(data[0] / 10.0f);
      m_ss >> m_s;
      m_output = "Maximum sample resolution: " + m_s;
      ErrorMessage("SAMPLE RESOLUTION ERROR", "Sample resolution is too high.", m_output);
      m_ss.clear();
      m_s.clear();
    }
    else
    {
      m_sampleResolution = _resolution;
    }
  }
  else
  {
    ErrorMessage("SAMPLE RESOLUTION ERROR", "You cannot set the sample resolution yet.", "Read images and check their dimensions first.");
  }
}

void ImageStack::ErrorMessage(std::string _type, std::string _line1, std::string _line2)
{
  std::cout << "==============================================\n"
            << _type << ":\n"
            << "      " << _line1 << '\n'
            << "      " << _line2 << '\n'
            << "==============================================\n";
}