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

void find_a(std::string_view mem, long long begin, long long end) {
  auto t1 = std::chrono::steady_clock::now();
  for (long long a = begin; a < end; ++a) {
    cpu::State state;
    state.a = a;
    state.pc = 0;
    state.quine_idx = 0;
    cpu::execute(state, mem, /*for_p2=*/true);
    if (state.quine_idx == (int)mem.size()) {
      std::cout << "######################################### FOUND A = " << a
                << '\n';
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
  constexpr long long FROM = 42700000000;
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
  auto input = read_input("p1_sample.txt");
  solve(input);
  return 0;
}
