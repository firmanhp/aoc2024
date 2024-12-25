#include <algorithm>
#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <span>
#include <sstream>
#include <thread>
#include <tuple>
#include <unordered_set>

#include "input.h"

namespace {

using util::print;
using util::valid_spot;
using util::Vector2D;
typedef long long ll;
typedef std::pair<int, int> pii;

#ifdef TEST
constexpr std::string INPUT = "p1_input.txt";
#endif
#ifdef SAMPLE
constexpr std::string INPUT = "p1_sample.txt";
#endif

enum class Type : int { Lock, Key };

std::ostream& operator<<(std::ostream& o, Type type) {
  switch (type) {
  case Type::Lock:
    return o << "Lock";
  case Type::Key:
    return o << "Key";
  }
}

struct Pins {
  Type type;
  int max_height;
  std::vector<int> heights;
};

bool pins_match(const Pins& a, const Pins& b) {
  if (a.type == b.type)
    return false;

  assert(a.heights.size() == b.heights.size());
  assert(a.max_height == b.max_height);
  const int max_height = a.max_height;
  const int N = a.heights.size();
  for (int i = 0; i < N; ++i) {
    if (a.heights[i] + b.heights[i] > max_height)
      return false;
  }
  return true;
}

Pins to_pins(const Vector2D<char>& grid) {
  const int cols = grid.width(), rows = grid.height();
  // if not a lock, then it's a key
  bool is_a_lock = true;
  for (int i = 0; i < cols; ++i) {
    if (grid[0][i] == '.') {
      is_a_lock = false;
      break;
    }
  }

  std::vector<int> heights(cols);
  // assume valid grid
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      if (grid[i][j] == '#')
        ++heights[j];
    }
  }

  return Pins{
      .type = is_a_lock ? Type::Lock : Type::Key,
      .max_height = rows,
      .heights = std::move(heights),
  };
}

std::vector<Pins> to_pins(const std::vector<Vector2D<char>>& input) {
  std::vector<Pins> pins;
  pins.reserve(input.size());
  for (const auto& grid : input) {
    pins.push_back(to_pins(grid));
  }
  return pins;
}

int solve(const std::vector<Vector2D<char>>& input) {
  int ans = 0;
  std::cout << "inputs: " << input.size() << '\n';
  std::vector<Pins> pins = to_pins(input);

  for (const Pins& pin : pins) {
    std::cout << "pin: " << pin.type << " " << pin.max_height << " -";
    for (int h : pin.heights) {
      std::cout << " " << h;
    }
    std::cout << '\n';
  }

  const int N = pins.size();
  for (int i = 0; i < N; ++i) {
    for (int j = i + 1; j < N; ++j) {
      if (pins_match(pins[i], pins[j]))
        ++ans;
    }
  }

  return ans;
}

} // namespace

int main() {
  auto input = input::read_input(INPUT);
  std::cout << solve(input) << '\n';
  return 0;
}
