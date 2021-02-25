///
/// @file Table.cpp
/// @brief Store triangulation table and handle binary to base 10 number conversion

#include <iostream>
#include <math.h>

#include "Table.h"

std::vector<int> Table::Triangulate(std::string _binary)
{
  int power = 7;
  size_t result = 0;

  for (size_t i = 0; i < _binary.length(); i++)
  {
    if (_binary[i] == '1')
    {
      result += pow(2, power);
    }
    power--;
  }
  return m_triTable[result];
}
