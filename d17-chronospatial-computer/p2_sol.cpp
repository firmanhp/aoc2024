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

uint8_t eval_b(std::span<const uint8_t> candidate_a, int start_idx) {
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
  uint64_t a = 0;
  const int sz = candidate_a.size();
  for (int i = sz - 1; i >= start_idx; --i) {
    a <<= 3;
    a |= candidate_a[i];
  }

  uint64_t b = a % 8;
  b ^= 3;
  b ^= (a >> b);
  b ^= 5;
  return static_cast<uint8_t>(b % 8);
}

std::optional<uint64_t> result;

void brute_force(std::span<uint8_t> candidate_a,
                 std::span<const uint8_t> target, int fill_idx) {
  if (fill_idx < 0) {
    uint64_t candidate = 0;
    const int sz = candidate_a.size();
    for (int i = sz - 1; i >= 0; --i) {
      candidate <<= 3;
      candidate |= candidate_a[i];
    }
    result = std::min(result.value_or(std::numeric_limits<uint64_t>::max()),
                      candidate);
    return;
  }

  for (int i = 0; i < 8; ++i) {
    candidate_a[fill_idx] = i;
    if (eval_b(candidate_a, fill_idx) == target[fill_idx])
      brute_force(candidate_a, target, fill_idx - 1);
  }
}

uint64_t solve(const Input& input) {
  const int out_len = input.bytecode.size();
  std::vector<uint8_t> candidate_buf(out_len, 0);
  constexpr uint8_t target[] = {2, 4, 1, 3, 7, 5, 0, 3, 4, 1, 1, 5, 5, 5, 3, 0};
  brute_force(candidate_buf, target, out_len - 1);
  assert(result.has_value());
  return *result;
}

} // namespace

int main() {
  auto input = read_input("p1_input.txt");
  std::cout << solve(input) << '\n';
  return 0;
}
