#include <algorithm>
#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <span>
#include <sstream>
#include <thread>
#include <unordered_set>

#include "input.h"
#include "util/vector_2d.h"

namespace {

using util::print;
using util::Vector2D;
using pii = std::pair<int, int>;

#ifdef TEST
constexpr std::string INPUT = "p1_input.txt";
#endif
#ifdef SAMPLE
constexpr std::string INPUT = "p1_sample.txt";
#endif

constexpr int dr[] = {0, 1, 0, -1};
constexpr int dc[] = {1, 0, -1, 0};
constexpr int DIRS = 4;
constexpr int INF = 1e9;

bool possible(std::span<const std::string> patterns, std::string_view design) {
  const int N = design.size();
  std::vector<bool> can(N + 1, false);

  // standard DP
  can[N] = true;
  for (int i = N - 1; i >= 0; --i) {
    for (const auto& s : patterns) {
      if (design.substr(i, s.size()) == s)
        can[i] = can[i] | can[i + s.size()];
      if (can[i])
        break; // early stopping
    }
  }

  return can[0];
}

int solve(const input::Input& input) {
  int ans = 0;
  for (const auto& design : input.designs) {
    if (possible(input.patterns, design))
      ++ans;
  }

  return ans;
}

} // namespace

int main() {
  auto input = input::read_input(INPUT);
  std::cout << solve(input) << '\n';
  return 0;
}
