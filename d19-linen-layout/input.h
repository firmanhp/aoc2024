#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace input {

struct Input {
  std::vector<std::string> patterns;
  std::vector<std::string> designs;
};

inline Input read_input(std::string_view file_name) {
  std::fstream file;
  file.open(file_name, std::ios_base::in);
  assert(file.is_open());

  auto clear_comma = [](std::string s) {
    std::string result;
    for (char c : s) {
      if (c == ',')
        continue;
      result.push_back(c);
    }
    return result;
  };

  std::string s;
  std::vector<std::string> patterns;
  std::getline(file, s);
  std::istringstream iss(std::move(s));
  {
    std::string data;
    while (iss >> data) {
      patterns.push_back(clear_comma(std::move(data)));
    }
  }

  std::getline(file, s); // empty line
  std::vector<std::string> designs;
  {
    std::string data;
    while (std::getline(file, data)) {
      designs.push_back(std::move(data));
    }
  }

  return Input{
      .patterns = std::move(patterns),
      .designs = std::move(designs),
  };
}
} // namespace input