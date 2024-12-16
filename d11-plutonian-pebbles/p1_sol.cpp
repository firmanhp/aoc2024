#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <span>
#include <sstream>

namespace {

using Stone = long long;

std::vector<Stone> read_input() {
  std::fstream file;
  file.open("p1_input.txt", std::ios_base::in);
  assert(file.is_open());

  std::vector<Stone> res;

  std::string s;
  while (std::getline(file, s)) {
    std::istringstream iss(std::move(s));
    Stone level;
    while (iss >> level) {
      res.push_back(level);
    }
  }

  return res;
}

int digit_count(Stone stone) {
  int count = 0;
  while (stone) {
    ++count;
    stone /= 10;
  }
  return count;
}

std::optional<std::pair<Stone, Stone>> try_split(Stone stone) {
  const int count = digit_count(stone);
  assert(count > 0);
  if (count % 2 != 0) {
    return std::nullopt;
  }

  const long long divider = std::pow(10, count / 2);
  return std::make_pair(stone / divider, stone % divider);
}

void blink(std::vector<Stone>& stones) {
  const int N = stones.size();
  // despite the problem says "done in order", not doing the ordering does not
  // affect the correctness of the answer
  for (int i = 0; i < N; ++i) {
    Stone& stone = stones[i];
    if (stone == 0) {
      stone = 1;
      continue;
    } else if (auto split = try_split(stone); split.has_value()) {
      stone = split->first;
      // dangerous; must be done after assignment
      stones.push_back(split->second);
      continue;
    } else {
      stone = stone * 2024;
    }
  }
}

int solve(std::vector<Stone> input) {
  for (int i = 0; i < 25; ++i) {
    blink(input);
  }
  return input.size();
}

} // namespace

int main() {
  std::cout << solve(read_input()) << '\n';
  return 0;
}
