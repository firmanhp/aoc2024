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

void gen_mov1(std::string_view code, int& min_out);
void gen_mov2(std::string_view code, int& min_out);
void gen_mov3(std::string_view code, int& min_out);

void gen_mov1(std::string_view code, int& min_out) {
  const int sz = code.size();
  // try out between horizontal/vertical first
  for (ll BITMASK = 0; BITMASK < (1LL << sz); ++BITMASK) {
    std::string mov1;
    bool ok = true;
    for (int i = 0; i < sz; ++i) {
      Keypad bef = i == 0 ? Keypad::kA : to_keypad(code[i - 1]);
      Keypad cur = to_keypad(code[i]);
      auto m =
          make_move(bef, cur, /*horizontal_first*/ (BITMASK & (1LL << i)) > 0);
      if (!m.has_value()) {
        ok = false;
        break;
      }
      mov1.append(*std::move(m));
      mov1.push_back('A');
    }

    if (ok)
      gen_mov2(mov1, min_out);
  }
}

void gen_mov2(std::string_view code, int& min_out) {
  const int sz = code.size();
  // try out between horizontal/vertical first
  for (ll BITMASK = 0; BITMASK < (1LL << sz); ++BITMASK) {
    std::string mov2;
    bool ok = true;
    for (int i = 0; i < sz; ++i) {
      RobotKeypad bef = i == 0 ? RobotKeypad::kA : to_robot_keypad(code[i - 1]);
      RobotKeypad cur = to_robot_keypad(code[i]);
      auto m =
          make_move(bef, cur, /*horizontal_first*/ (BITMASK & (1LL << i)) > 0);
      if (!m.has_value()) {
        ok = false;
        break;
      }
      mov2.append(*std::move(m));
      mov2.push_back('A');
    }

    if (ok)
      gen_mov3(mov2, min_out);
  }
}

void gen_mov3(std::string_view code, int& min_out) {
  const int sz = code.size();
  // try out between horizontal/vertical first
  for (ll BITMASK = 0; BITMASK < (1LL << sz); ++BITMASK) {
    std::string mov3;
    bool ok = true;
    for (int i = 0; i < sz; ++i) {
      RobotKeypad bef = i == 0 ? RobotKeypad::kA : to_robot_keypad(code[i - 1]);
      RobotKeypad cur = to_robot_keypad(code[i]);
      auto m =
          make_move(bef, cur, /*horizontal_first*/ (BITMASK & (1LL << i)) > 0);
      if (!m.has_value()) {
        ok = false;
        break;
      }
      mov3.append(*std::move(m));
      mov3.push_back('A');
    }

    if (ok)
      min_out = std::min(min_out, (int)mov3.size());
  }
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

int solve(const std::vector<std::string> input) {
  std::string code = input.front();
  int res = 0;
  for (const auto& code : input) {
    std::cout << code << '\n';

    int len = 0;
    for (int i = 0; i < (int)code.size(); ++i) {
      Keypad bef = i == 0 ? Keypad::kA : to_keypad(code[i - 1]);
      Keypad cur = to_keypad(code[i]);
      int min_out = 1e9;
      for (int j = 0; j < 2; ++j) {
        auto s = make_move(bef, cur, j);
        if (!s.has_value())
          continue;
        gen_mov2(*s + "A", min_out);
      }
      len += min_out;
    }
    res += len * get_numeric_val(code);
  }

  return res;
}

} // namespace

int main() {
  auto input = input::read_input(INPUT);
  std::cout << solve(input) << '\n';
  return 0;
}
