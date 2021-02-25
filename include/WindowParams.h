/// \file WindowParams.h
/// \brief Stores the window parameters
/// \author Josh Bailey
/// \version 1.0
/// \date 13/01/21 Updated to NCCA Coding Standard
/// Revision History:
///
/// \todo

#ifndef WINDOWPARAMS_H_
#define WINDOWPARAMS_H_

struct WinParams
{
  //Window dimensions
  int width = 1024;
  int height = 720;

  // Mouse coordinates
  int prev_x = 1024 / 2;
  int prev_y = 720 / 2;
  int curr_x = 1024 / 2;
  int curr_y = 720 / 2;
};

//----------------------------------------------------------------------------------------------------------------------
/// @brief the increment for x/y translation with mouse movement
//----------------------------------------------------------------------------------------------------------------------
constexpr float INCREMENT=0.01f;
//----------------------------------------------------------------------------------------------------------------------
/// @brief the increment for the wheel zoom
//----------------------------------------------------------------------------------------------------------------------
constexpr float ZOOM=0.1f;

#endif  // _WINDOWPARAMS_H_
