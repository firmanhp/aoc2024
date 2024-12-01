#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <span>

namespace {

using PQ = std::priority_queue<int, std::vector<int>, std::greater<int>>;
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
  PQ left, right;
  long long res = 0;
  for (const auto& [le, ri] : input) {
    left.push(le);
    right.push(ri);
  }
  assert(left.size() == right.size());

  while (!left.empty() && !right.empty()) {
    const int le = left.top();
    left.pop();
    const int ri = right.top();
    right.pop();

    res += std::abs(ri - le);
  }

  return res;
}

} // namespace

int main() {
  std::cout << solve(read_input()) << '\n';
  return 0;
}
