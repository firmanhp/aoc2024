#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <span>
#include <sstream>

namespace {

using Report = std::vector<int>;

std::vector<Report> read_input() {
  std::fstream file;
  file.open("p1_input.txt", std::ios_base::in);
  assert(file.is_open());

  std::vector<Report> res;

  std::string s;
  while (std::getline(file, s, '\n')) {
    std::istringstream iss(std::move(s));
    Report report{};
    int level;
    while (iss >> level) {
      report.push_back(level);
    }
    res.push_back(std::move(report));
  }

  return res;
}

bool is_safe(const Report& report) {
  bool has_increasing = true;
  bool has_decreasing = true;
  bool safe_difference = true;
  for (int i = 0; i < report.size() - 1; ++i) {
    has_increasing &= report[i] < report[i + 1];
    has_decreasing &= report[i] > report[i + 1];
    const int diff = abs(report[i] - report[i + 1]);
    safe_difference &= (1 <= diff) && (diff <= 3);
  }

  return (has_increasing || has_decreasing) && safe_difference;
}

int solve(std::span<const Report> input) {
  int res = 0;
  for (const auto& report : input) {
    if (is_safe(report))
      ++res;
  }
  return res;
}

} // namespace

int main() {
  std::cout << solve(read_input()) << '\n';
  return 0;
}
