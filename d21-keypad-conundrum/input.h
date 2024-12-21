#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

#include "util/vector_2d.h"

namespace input {

inline std::vector<std::string> read_input(std::string_view file_name) {
  std::fstream file;
  file.open(file_name, std::ios_base::in);
  assert(file.is_open());

  std::vector<std::string> res;
  std::string s;
  while (getline(file, s)) {
    res.push_back(std::move(s));
  }

  return res;
}

} // namespace input
