#include <algorithm>
#include <atomic>
#include <cassert>
#include <chrono>
#include <span>
#include <thread>
#include <unordered_set>

#include "cpu.h"
#include "input.h"
#include "util/vector_2d.h"

namespace {

using input::Input;
using input::read_input;

uint8_t eval_b(uint64_t a) {
  /*
  Program: 2,4,1,3,7,5,0,3,4,1,1,5,5,5,3,0
  2,4, B = A & 0b111
  1,3, B = B ^ 0b11
  4,1, B = B ^ (A >> B)
  1,5, B = B ^ 5
  5,5, print B % 8
  0,3, A = A >> 3
  3,0 jnz 0
  */

  // after some manual assembly reordering, a is only used in 3bit chunks and we
  // only care about b for the output
  uint64_t b = a % 8;
  b ^= 3;
  b ^= (a >> b);
  b ^= 5;
  return static_cast<uint8_t>(b % 8);
}

std::optional<uint64_t> result;

// evaluate and check the output from backwards
void brute_force(uint64_t candidate_a, std::span<const uint8_t> target,
                 int target_idx) {
  if (target_idx < 0) {
    result = std::min(result.value_or(std::numeric_limits<uint64_t>::max()),
                      candidate_a);
    return;
  }

  for (uint8_t i = 0; i < 8; ++i) {
    uint64_t new_a = (candidate_a << 3) | i;
    if (eval_b(new_a) == target[target_idx])
      brute_force(new_a, target, target_idx - 1);
  }
}

uint64_t solve(const Input& /*input*/) {
  constexpr uint8_t target[] = {2, 4, 1, 3, 7, 5, 0, 3, 4, 1, 1, 5, 5, 5, 3, 0};
  brute_force(0, target, std::size(target) - 1);
  assert(result.has_value());
  return *result;
}

} // namespace

int main() {
  auto input = read_input("p1_input.txt");
  std::cout << solve(input) << '\n';
  return 0;
}
