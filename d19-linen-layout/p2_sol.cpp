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

long long count_possibilities(std::span<const std::string> patterns,
                              std::string_view design) {
  const int N = design.size();
  std::vector<long long> count(N + 1, 0);

  // standard DP
  count[N] = 1;
  for (int i = N - 1; i >= 0; --i) {
    for (const auto& s : patterns) {
      if (design.substr(i, s.size()) == s)
        count[i] += count[i + s.size()];
    }
  }

  return count[0];
}

long long solve(const input::Input& input) {
  long long ans = 0;
  for (const auto& design : input.designs) {
    ans += count_possibilities(input.patterns, design);
  }

  return ans;
}

} // namespace

int main() {
  auto input = input::read_input(INPUT);
  std::cout << solve(input) << '\n';
  return 0;
}
