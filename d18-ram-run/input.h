#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

namespace input {
inline std::vector<std::pair<int, int>> read_input(std::string_view file_name) {
  std::fstream file;
  file.open(file_name, std::ios_base::in);
  assert(file.is_open());

  std::vector<std::pair<int, int>> res;
  // this sucks
  std::string s;
  while (std::getline(file, s)) {
    size_t comma = s.find(",");
    assert(comma != std::string::npos);
    s.replace(comma, 1, " ");
    std::pair<int, int> pos;
    std::istringstream(std::move(s)) >> pos.first >> pos.second;
    res.push_back(pos);
  }

  return res;
}
} // namespace input