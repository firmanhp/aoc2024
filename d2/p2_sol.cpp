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

std::optional<int> is_safe(const Report& report) {
  // wow i met a potential UB
  for (int skip = -1; skip < (int)report.size(); ++skip) {
    bool has_increasing = true;
    bool has_decreasing = true;
    bool safe_difference = true;

    auto advance = [skip](int val) -> int {
      ++val;
      if (val == skip)
        ++val;
      return val;
    };

    const int start = (skip == 0 ? 1 : 0);
    for (int cur = start, next = advance(start); next < report.size();
         cur = advance(cur), next = advance(next)) {
      has_increasing &= report[cur] < report[next];
      has_decreasing &= report[cur] > report[next];
      const int diff = abs(report[cur] - report[next]);
      safe_difference &= (1 <= diff) && (diff <= 3);
    }

    if ((has_increasing || has_decreasing) && safe_difference) {
      return skip;
    }
  }

  return std::nullopt;
}

int solve(std::span<const Report> input) {
  int res = 0;
  for (const auto& report : input) {
    if (is_safe(report))
      ++res;
  }
  return res;
}

void test() {
  const Report reports[] = {
      Report{7, 6, 4, 2, 1}, //
      Report{1, 2, 7, 8, 9}, //
      Report{9, 7, 6, 2, 1}, //
      Report{1, 3, 2, 4, 5}, //
      Report{8, 6, 4, 4, 1}, //
      Report{1, 3, 6, 7, 9}, //
  };

  for (int i = 0; i < std::size(reports); ++i) {
    const Report& report = reports[i];
    const std::optional<int> safe_spot = is_safe(report);

    std::cout << "SampleCase #" << i << ": ";
    if (safe_spot.has_value()) {
      std::cout << "Safe at " << *safe_spot;
    } else {
      std::cout << "Unsafe";
    }
    std::cout << '\n';
  }
}

} // namespace

int main() {
  test();
  std::cout << solve(read_input()) << '\n';
  return 0;
}
