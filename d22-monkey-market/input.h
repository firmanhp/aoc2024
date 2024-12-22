#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

#include "util/vector_2d.h"

namespace input {

inline std::vector<long long> read_input(std::string_view file_name) {
  std::fstream file;
  file.open(file_name, std::ios_base::in);
  assert(file.is_open());

  std::vector<long long> res;
  long long val;
  while (file >> val) {
    res.push_back(val);
  }

  return res;
}

} // namespace input
