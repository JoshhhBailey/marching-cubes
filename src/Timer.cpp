///
/// @file Timer.cpp
/// @brief Calculate delta time

#include "Timer.h"

Timer::Timer()
{
  m_previousTime = std::chrono::high_resolution_clock::now();
}

float Timer::DeltaTime()
{
  auto currentTime = std::chrono::high_resolution_clock::now();
  auto delta = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - m_previousTime).count();
  m_previousTime = currentTime;
  return delta;
}
