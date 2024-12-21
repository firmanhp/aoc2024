#include "sequences.h"

#include <array>
#include <cassert>

namespace sequences {

namespace {

constexpr int abs(int x) noexcept { return x < 0 ? -x : x; }

struct MovementVector {
  int up;
  int down;
  int left;
  int right;
};

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

constexpr int kKeypadRows = 4;
constexpr int kKeypadCols = 3;
constexpr Keypad kKeymap[kKeypadRows][kKeypadCols] = {
    {Keypad::k7, Keypad::k8, Keypad::k9},          //
    {Keypad::k4, Keypad::k5, Keypad::k6},          //
    {Keypad::k1, Keypad::k2, Keypad::k3},          //
    {Keypad::kDoNotTouch, Keypad::k0, Keypad::kA}, //
};

constexpr MovementVector make_keypad_movements(Keypad from, Keypad to) {
  const int from_row = static_cast<int>(from) / kKeypadCols;
  const int from_col = static_cast<int>(from) % kKeypadCols;
  const int to_row = static_cast<int>(to) / kKeypadCols;
  const int to_col = static_cast<int>(to) % kKeypadCols;
  const int vertical_diff = to_row - from_row;
  const int horizontal_diff = to_col - from_col;

  return MovementVector{
      .up = vertical_diff < 0 ? abs(vertical_diff) : 0,
      .down = vertical_diff > 0 ? abs(vertical_diff) : 0,
      .left = horizontal_diff < 0 ? abs(horizontal_diff) : 0,
      .right = horizontal_diff > 0 ? abs(horizontal_diff) : 0,
  };
}

constexpr int kKeypadMax = static_cast<int>(Keypad::MAX);
constexpr std::array<std::array<MovementVector, kKeypadMax>, kKeypadMax>
    kKeymapMovements = []() constexpr {
      std::array<std::array<MovementVector, kKeypadMax>, kKeypadMax> movements;
      for (int i = 0; i < kKeypadMax; ++i)
        for (int j = 0; j < kKeypadMax; ++j) {
          if (i == static_cast<int>(Keypad::kDoNotTouch) ||
              j == static_cast<int>(Keypad::kDoNotTouch)) {
            movements[i][j] = MovementVector{};
          }
          movements[i][j] = make_keypad_movements(static_cast<Keypad>(i),
                                                  static_cast<Keypad>(j));
        }
      return movements;
    }();

/**
    +---+---+
    | ^ | A |
+---+---+---+
| < | v | > |
+---+---+---+
 */

constexpr int kRobotKeypadRows = 2;
constexpr int kRobotKeypadCols = 3;
constexpr RobotKeypad kRobotKeymap[kRobotKeypadRows][kRobotKeypadCols] = {
    {RobotKeypad::kDoNotTouch, RobotKeypad::kUp, RobotKeypad::kA}, //
    {RobotKeypad::kLeft, RobotKeypad::kDown, RobotKeypad::kRight}, //
};

constexpr MovementVector make_robot_keypad_movements(RobotKeypad from,
                                                     RobotKeypad to) {
  const int from_row = static_cast<int>(from) / kRobotKeypadCols;
  const int from_col = static_cast<int>(from) % kRobotKeypadCols;
  const int to_row = static_cast<int>(to) / kRobotKeypadCols;
  const int to_col = static_cast<int>(to) % kRobotKeypadCols;
  const int vertical_diff = to_row - from_row;
  const int horizontal_diff = to_col - from_col;

  const bool horizontal_first =
      kRobotKeymap[to_row][from_col] == RobotKeypad::kDoNotTouch;

  return MovementVector{
      .up = vertical_diff < 0 ? abs(vertical_diff) : 0,
      .down = vertical_diff > 0 ? abs(vertical_diff) : 0,
      .left = horizontal_diff < 0 ? abs(horizontal_diff) : 0,
      .right = horizontal_diff > 0 ? abs(horizontal_diff) : 0,
  };
}

constexpr int kRobotKeypadMax = static_cast<int>(RobotKeypad::MAX);
constexpr std::array<std::array<MovementVector, kRobotKeypadMax>,
                     kRobotKeypadMax>
    kRobotKeymapMovements = []() constexpr {
      std::array<std::array<MovementVector, kRobotKeypadMax>, kRobotKeypadMax>
          movements;
      for (int i = 0; i < kRobotKeypadMax; ++i)
        for (int j = 0; j < kRobotKeypadMax; ++j) {
          if (i == static_cast<int>(RobotKeypad::kDoNotTouch) ||
              j == static_cast<int>(RobotKeypad::kDoNotTouch)) {
            movements[i][j] = MovementVector{};
          }
          movements[i][j] = make_robot_keypad_movements(
              static_cast<RobotKeypad>(i), static_cast<RobotKeypad>(j));
        }
      return movements;
    }();

} // namespace

std::optional<std::string> make_move(Keypad from, Keypad to,
                                     bool horizontal_first) {
  assert(from != Keypad::kDoNotTouch);
  assert(to != Keypad::kDoNotTouch);

  const int from_row = static_cast<int>(from) / kKeypadCols;
  const int from_col = static_cast<int>(from) % kKeypadCols;
  const int to_row = static_cast<int>(to) / kKeypadCols;
  const int to_col = static_cast<int>(to) % kKeypadCols;
  MovementVector vec =
      kKeymapMovements[static_cast<int>(from)][static_cast<int>(to)];

  std::string res;
  if (horizontal_first) {
    if (kKeymap[from_row][to_col] == Keypad::kDoNotTouch)
      return std::nullopt;

    while (vec.left-- > 0) {
      res.push_back(LEFT);
    }
    while (vec.right-- > 0) {
      res.push_back(RIGHT);
    }
    while (vec.up-- > 0) {
      res.push_back(UP);
    }
    while (vec.down-- > 0) {
      res.push_back(DOWN);
    }
    return res;
  }

  if (kKeymap[to_row][from_col] == Keypad::kDoNotTouch)
    return std::nullopt;
  while (vec.up-- > 0) {
    res.push_back(UP);
  }
  while (vec.down-- > 0) {
    res.push_back(DOWN);
  }
  while (vec.left-- > 0) {
    res.push_back(LEFT);
  }
  while (vec.right-- > 0) {
    res.push_back(RIGHT);
  }
  return res;
}

std::optional<std::string> make_move(RobotKeypad from, RobotKeypad to,
                                     bool horizontal_first) {
  assert(from != RobotKeypad::kDoNotTouch);
  assert(to != RobotKeypad::kDoNotTouch);

  const int from_row = static_cast<int>(from) / kRobotKeypadCols;
  const int from_col = static_cast<int>(from) % kRobotKeypadCols;
  const int to_row = static_cast<int>(to) / kRobotKeypadCols;
  const int to_col = static_cast<int>(to) % kRobotKeypadCols;
  MovementVector vec =
      kRobotKeymapMovements[static_cast<int>(from)][static_cast<int>(to)];

  std::string res;
  if (horizontal_first) {
    if (kRobotKeymap[from_row][to_col] == RobotKeypad::kDoNotTouch)
      return std::nullopt;
    while (vec.left-- > 0) {
      res.push_back(LEFT);
    }
    while (vec.right-- > 0) {
      res.push_back(RIGHT);
    }
    while (vec.up-- > 0) {
      res.push_back(UP);
    }
    while (vec.down-- > 0) {
      res.push_back(DOWN);
    }
    return res;
  }
  if (kRobotKeymap[to_row][from_col] == RobotKeypad::kDoNotTouch)
    return std::nullopt;
  while (vec.up-- > 0) {
    res.push_back(UP);
  }
  while (vec.down-- > 0) {
    res.push_back(DOWN);
  }
  while (vec.left-- > 0) {
    res.push_back(LEFT);
  }
  while (vec.right-- > 0) {
    res.push_back(RIGHT);
  }
  return res;
}

} // namespace sequences