#pragma once

#include <cassert>
#include <string>

namespace sequences {

constexpr char UP = '^';
constexpr char DOWN = 'v';
constexpr char LEFT = '<';
constexpr char RIGHT = '>';
constexpr char ACT = 'A';

/**
  +---+---+---+
  | 7 | 8 | 9 |
  +---+---+---+
  | 4 | 5 | 6 |
  +---+---+---+
  | 1 | 2 | 3 |
  +---+---+---+
      | 0 | A |
      +---+---+
 */

enum class Keypad : uint8_t {
  k7 = 0,
  k8,
  k9,
  k4,
  k5,
  k6,
  k1,
  k2,
  k3,
  kDoNotTouch,
  k0,
  kA,
  MAX
};

inline Keypad to_keypad(char c) {
  switch (c) {
  case '0':
    return Keypad::k0;
  case '1':
    return Keypad::k1;
  case '2':
    return Keypad::k2;
  case '3':
    return Keypad::k3;
  case '4':
    return Keypad::k4;
  case '5':
    return Keypad::k5;
  case '6':
    return Keypad::k6;
  case '7':
    return Keypad::k7;
  case '8':
    return Keypad::k8;
  case '9':
    return Keypad::k9;
  case 'A':
    return Keypad::kA;
  default:
    assert("Unknown char" && false);
  }

  __builtin_unreachable();
}

/**
    +---+---+
    | ^ | A |
+---+---+---+
| < | v | > |
+---+---+---+
 */
enum class RobotKeypad : uint8_t {
  kDoNotTouch = 0,
  kUp,
  kA,
  kLeft,
  kDown,
  kRight,
  MAX,
};

inline RobotKeypad to_robot_keypad(char c) {
  switch (c) {
  case '^':
    return RobotKeypad::kUp;
  case 'v':
    return RobotKeypad::kDown;
  case '<':
    return RobotKeypad::kLeft;
  case '>':
    return RobotKeypad::kRight;
  case 'A':
    return RobotKeypad::kA;
  default:
    assert("Unknown char" && false);
  }
  __builtin_unreachable();
}

std::optional<std::string> make_move(Keypad from, Keypad to,
                                     bool horizontal_first);
std::optional<std::string> make_move(RobotKeypad from, RobotKeypad to,
                                     bool horizontal_first);

} // namespace sequences