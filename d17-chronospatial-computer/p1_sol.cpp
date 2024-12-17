#include <algorithm>
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

std::string solve(Input& input) {
  cpu::execute(input.state, input.bytecode);
  return input.state.out_buf;
}

} // namespace

int main() {
  auto input = read_input("p1_input.txt");
  std::cout << solve(input) << '\n';
  return 0;
}
