/// \file Timer.h
/// \brief Calculate delta time
/// \author Josh Bailey
/// \version 1.0
/// \date 13/01/21 Updated to NCCA Coding Standard
/// Revision History:
///
/// \todo


#ifndef TIMER_H_
#define TIMER_H_

#include <chrono>

class Timer
{
  public:
    Timer();
    // Calculate delta time
    float DeltaTime();

  private:
    std::chrono::high_resolution_clock::time_point m_previousTime;
};

#endif  // _TIMER_H_
