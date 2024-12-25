#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

#include "util/vector_2d.h"

namespace input {

namespace internal {
util::Vector2D<char> to_grid(const std::vector<std::string>& grid_vec) {
  const int rows = grid_vec.size();
  const int cols = grid_vec[0].size();

  util::Vector2D<char> res(rows, cols);
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      res[i][j] = grid_vec[i][j];
    }
  }
  return res;
}
} // namespace internal

inline std::vector<util::Vector2D<char>>
read_input(std::string_view file_name) {
  std::fstream file;
  file.open(file_name, std::ios_base::in);
  assert(file.is_open());

  std::vector<util::Vector2D<char>> grids;
  std::string s;
  std::vector<std::string> grid_vec;
  while (getline(file, s)) {
    if (s.empty()) {
      grids.push_back(internal::to_grid(std::move(grid_vec)));
      grid_vec.clear();
      continue;
    }
    grid_vec.push_back(std::move(s));
  }

  if (!grid_vec.empty()) {
    grids.push_back(internal::to_grid(std::move(grid_vec)));
    grid_vec.clear();
  }

  return grids;
}

} // namespace input
