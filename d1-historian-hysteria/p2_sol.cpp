#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include <span>

namespace {

using Pii = std::pair<int, int>;

std::vector<Pii> read_input() {
  std::fstream file;
  file.open("p1_input.txt", std::ios_base::in);
  assert(file.is_open());

  std::vector<Pii> res;
  long long left, right;
  while (file >> left >> right) {
    res.push_back(std::make_pair(left, right));
  }

  return res;
}

long long solve(std::span<const Pii> input) {

  long long res = 0;
  std::map<int, int> right_count;
  for (const auto [_, ri] : input) {
    right_count[ri]++;
  }

  for (const auto [le, _] : input) {
    res += (long long)le * right_count[le];
  }

  return res;
}

} // namespace

int main() {
  std::cout << solve(read_input()) << '\n';
  return 0;
}
