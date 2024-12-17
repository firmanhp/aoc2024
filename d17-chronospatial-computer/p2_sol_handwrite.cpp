#include <algorithm>
#include <atomic>
#include <cassert>
#include <chrono>
#include <thread>
#include <unordered_set>

#include "cpu.h"
#include "input.h"
#include "util/vector_2d.h"

namespace {

using input::Input;
using input::read_input;

bool handwrite_input_exec(long long a) {
  /*
  2,4, B = A % 8
  1,3, B = B ^ 3
  7,5, C = A >> B
  0,3, A = A >> 3
  4,1, B = B ^ (A >> B)
  1,5, B = B ^ 5
  5,5, print B % 8
  3,0 jnz 0
  */
  //  Program: 2,4,1,3,7,5,0,3,4,1,1,5,5,5,3,0

  constexpr int mem[] = {2, 4, 1, 3, 7, 5, 0, 3, 4, 1, 1, 5, 5, 5, 3, 0};
  // constexpr int mem[] = {3, 0};
  long long b = 0, c = 0;
  int quine_idx = 0;
  do {
    b = a % 8;
    b = b ^ 3;
    c = a >> b;
    a = a >> 3;
    b = b ^ c;
    b = b ^ 5;

    const int val = b % 8;
    if (quine_idx >= (int)std::size(mem) || mem[quine_idx] != val)
      return false;
    ++quine_idx;
  } while (a != 0);

  return quine_idx == (int)std::size(mem);
}

bool sample_handwrite_input_exec(long long a, std::string_view mem) {
  long long b = 0, c = 0;
  int quine_idx = 0;
  /*
  0,3 A = A >> 3
  5,4 out A
  3,0 jnz 0
  */
  do {
    a >>= 3;
    const int val = a % 8;
    if (quine_idx >= (int)mem.size() || (mem[quine_idx] - '0') != val)
      return false;
    ++quine_idx;
  } while (a != 0);

  return quine_idx == (int)mem.size();
}

void find_a(std::string_view /*mem*/, long long begin, long long end) {
  auto t1 = std::chrono::steady_clock::now();
  for (long long a = begin; a < end; ++a) {
    if (handwrite_input_exec(a)) {
      std::cout << "######################################### FOUND A = " << a
                << '\n';
      std::exit(0);
      return;
    }
  }
  auto t2 = std::chrono::steady_clock::now();
  std::cout
      << "Execution [" << begin << ", " << end << "] took "
      << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()
      << "ms \n";
}

void solve(const Input& input) {
  constexpr long long COUNT_PER_JOB = 1 * 1e8;
  constexpr long long FROM = 1173000000000;
  // constexpr long long FROM = 0;
  constexpr long long MAX = 1e15;
  constexpr int MAX_JOBS = 12;
  std::queue<std::thread> jobs;
  std::string_view mem = input.bytecode;

  for (long long start = FROM; start < MAX; start += COUNT_PER_JOB) {
    while (jobs.size() >= MAX_JOBS) {
      auto& t = jobs.front();
      if (t.joinable())
        t.join();
      jobs.pop();
    }
    std::cout << "spawn thread " << start << " " << start + COUNT_PER_JOB
              << '\n';
    jobs.push(std::thread(
        [start, mem]() { find_a(mem, start, start + COUNT_PER_JOB); }));
  }
  // find_a(mem, 0, MAX);
}

} // namespace

int main() {
  auto input = read_input("p1_input.txt");
  solve(input);
  return 0;
}
