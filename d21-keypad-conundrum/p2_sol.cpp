#include <algorithm>
#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <span>
#include <sstream>
#include <thread>
#include <unordered_set>

#include "input.h"
#include "sequences.h"
#include "util/vector_2d.h"

template <> struct std::hash<std::pair<std::string, int>> {
  std::size_t operator()(const std::pair<std::string, int>& p) const noexcept {
    auto h1 = std::hash<std::string>{}(p.first);
    auto h2 = std::hash<int>{}(p.second);
    return h1 ^ h2;
  }
};

namespace {

using util::print;
using util::valid_spot;
using util::Vector2D;
using pii = std::pair<int, int>;
using sequences::Keypad;
using sequences::make_move;
using sequences::RobotKeypad;
using sequences::to_keypad;
using sequences::to_robot_keypad;
typedef long long ll;

#ifdef TEST
constexpr std::string INPUT = "p1_input.txt";
#endif
#ifdef SAMPLE
constexpr std::string INPUT = "p1_sample.txt";
#endif

// i don't like this problem
//                        keypad -> door
//               keypad -> robot
//      keypad -> robot
//        you

constexpr int TOTAL_ROBOTS = 25;

std::unordered_map<std::pair<std::string, int>, ll> cache;
ll gen_mov(std::string code, int level) {
  const int sz = code.size();
  // std::cout << "DBG: level " << level << ", size " << sz << '\n';
  if (level == TOTAL_ROBOTS) {
    return sz;
  }

  auto p = std::make_pair(code, level);
  if (cache.contains(p)) {
    return cache[p];
  }

  ll ans = 0;
  for (int i = 0; i < sz; ++i) {
    RobotKeypad bef = i == 0 ? RobotKeypad::kA : to_robot_keypad(code[i - 1]);
    RobotKeypad cur = to_robot_keypad(code[i]);
    ll min_out = std::numeric_limits<ll>::max();
    for (int j = 0; j < 2; ++j) {
      auto s = make_move(bef, cur, j);
      if (!s.has_value())
        continue;
      min_out = std::min(min_out, gen_mov(*s + "A", level + 1));
    }
    ans += min_out;
  }
  cache[p] = ans;
  return ans;
}

int get_numeric_val(std::string_view code) {
  int num = 0;
  for (char c : code) {
    if (c == 'A')
      break;
    num *= 10;
    num += (c - '0');
  }

  return num;
}

ll solve(const std::vector<std::string> input) {
  std::string code = input.front();
  ll res = 0;
  for (const auto& code : input) {
    std::cout << code << '\n';

    ll len = 0;
    for (int i = 0; i < (int)code.size(); ++i) {
      Keypad bef = i == 0 ? Keypad::kA : to_keypad(code[i - 1]);
      Keypad cur = to_keypad(code[i]);
      ll min_out = std::numeric_limits<ll>::max();
      for (int j = 0; j < 2; ++j) {
        auto s = make_move(bef, cur, j);
        if (!s.has_value())
          continue;
        min_out = std::min(min_out, gen_mov(*s + "A", /*level=*/0));
      }
      len += min_out;
    }
    std::cout << len << " x " << get_numeric_val(code) << '\n';
    res += (ll)len * (ll)get_numeric_val(code);
  }

  return res;
}

} // namespace

int main() {
  auto input = input::read_input(INPUT);
  std::cout << solve(input) << '\n';
  return 0;
}
