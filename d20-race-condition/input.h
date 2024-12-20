#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

#include "util/vector_2d.h"

namespace input {

inline util::Vector2D<char> read_input(std::string_view file_name) {
  std::fstream file;
  file.open(file_name, std::ios_base::in);
  assert(file.is_open());

  util::Vector2D<char> grid = util::Vector2D<char>::unsized();
  int inferred_row = 0, inferred_col = 0;
  std::string s;
  while (getline(file, s)) {
    if (inferred_col == 0)
      inferred_col = s.size();

    assert((int)s.size() == inferred_col);
    for (char c : s)
      grid.push_back(c);

    ++inferred_row;
  }
  grid.resize(inferred_row, inferred_col);
  return grid;
}

} // namespace input