#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

#include "util/vector_2d.h"

namespace input {

struct Input {
  struct Gate {
    std::string operand_1, operand_2;
    std::string op;
    std::string operand_result;
  };

  std::vector<std::pair<std::string, int>> initial;
  std::vector<Gate> gates;
};

inline Input read_input(std::string_view file_name) {
  std::fstream file;
  file.open(file_name, std::ios_base::in);
  assert(file.is_open());

  std::vector<std::pair<std::string, int>> initials;
  std::string s;
  while (getline(file, s)) {
    if (s.empty())
      break;
    std::replace(s.begin(), s.end(), ':', ' ');
    std::pair<std::string, int> initial;
    std::istringstream(std::move(s)) >> initial.first >> initial.second;
    initials.push_back(std::move(initial));
  }

  std::vector<Input::Gate> gates;
  while (getline(file, s)) {
    std::string unused;
    Input::Gate gate;
    std::istringstream(std::move(s)) >> gate.operand_1 >> gate.op >>
        gate.operand_2 >> unused >> gate.operand_result;
    gates.push_back(std::move(gate));
  }

  return Input{.initial = std::move(initials), .gates = std::move(gates)};
}

} // namespace input
