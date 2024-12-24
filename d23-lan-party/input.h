#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

#include "util/vector_2d.h"

namespace input {

struct Edge {
  std::string u;
  std::string v;
};

inline std::vector<Edge> read_input(std::string_view file_name) {
  std::fstream file;
  file.open(file_name, std::ios_base::in);
  assert(file.is_open());

  std::vector<Edge> res;
  std::string s;
  while (getline(file, s)) {
    std::replace(s.begin(), s.end(), '-', ' ');
    Edge edge;
    std::istringstream(std::move(s)) >> edge.u >> edge.v;
    res.push_back(std::move(edge));
  }

  return res;
}

} // namespace input
