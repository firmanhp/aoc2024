#pragma once

#include <fstream>
#include <iostream>
#include <sstream>

namespace input {
struct Input {
  cpu::State state;
  std::string bytecode;
};

Input read_input(std::string_view file_name) {
  std::fstream file;
  file.open(file_name, std::ios_base::in);
  assert(file.is_open());

  std::string s, unused;

  cpu::State state;
  long long* val_ptr[3] = {&state.a, &state.b, &state.c};
  for (int i = 0; i < 3; ++i) {
    assert(getline(file, s));
    // Register A: 729
    std::istringstream(std::move(s)) >> unused >> unused >> *val_ptr[i];
  }
  assert(getline(file, s) && s.empty()); // newline
  assert(getline(file, s));
  std::istringstream(std::move(s)) >> unused >> s;
  std::string bytecode;
  for (int i = 0; i < (int)s.size(); i += 2) {
    bytecode.push_back(s[i]);
  }

  return Input{.state = state, .bytecode = std::move(bytecode)};
}
} // namespace input