#pragma once

#include <ostream>

inline std::ostream& operator<<(std::ostream& o, std::pair<int, int> pii) {
  return o << "(" << pii.first << ", " << pii.second << ")";
}