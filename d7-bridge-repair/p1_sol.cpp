#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include <span>
#include <sstream>

namespace {

struct Equation {
  long long target;
  std::vector<long long> nums;
};
using Input = std::vector<Equation>;

Input read_input() {
  std::fstream file;
  file.open("p1_input.txt", std::ios_base::in);
  assert(file.is_open());

  Input input;
  std::string s;
  while (std::getline(file, s) && !s.empty()) {
    std::replace(s.begin(), s.end(), ':', ' ');
    std::stringstream ss(std::move(s));

    Equation equ;
    ss >> equ.target;
    long long num;
    while (ss >> num) {
      equ.nums.push_back(num);
    }
    input.push_back(std::move(equ));
  }

  return input;
}

// brute force.
bool solvable(long long target, const std::vector<long long>& nums,
              int cur_idx = 0, long long cur_val = 0) {
  if (cur_idx == 0)
    return solvable(target, nums, cur_idx + 1, nums[0]);
  const int N = nums.size();
  if (cur_idx == N)
    return cur_val == target;

  return solvable(target, nums, cur_idx + 1, cur_val + nums[cur_idx]) ||
         solvable(target, nums, cur_idx + 1, cur_val * nums[cur_idx]);
}

int solve(const Input& input) {
  long long ans = 0;
  for (const auto& equ : input) {
    if (solvable(equ.target, equ.nums)) {
      // use python to evaluate this
      std::cout << equ.target << " + ";
    }
  }
  std::cout << "0\n";
  return ans;
}

} // namespace

int main() {
  solve(read_input());
  return 0;
}
