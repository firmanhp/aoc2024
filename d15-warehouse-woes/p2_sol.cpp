#include <algorithm>
#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <set>
#include <span>
#include <sstream>
#include <thread>

#include "util/vector_2d.h"

namespace {

using util::print;
using util::Vector2D;
using pii = std::pair<int, int>;

enum class Direction : int { Right = 0, Down, Left, Up };

struct World {
  Vector2D<char> grid;
  std::vector<Direction> directions;
};

constexpr char SPACE = '.';
constexpr char ROBOT = '@';
constexpr char WALL = '#';
constexpr char BOX_1 = 'O';
constexpr char BOX_L = '[';
constexpr char BOX_R = ']';

World read_input() {
  std::fstream file;
  file.open("p1_input.txt", std::ios_base::in);
  assert(file.is_open());

  Vector2D<char> grid = Vector2D<char>::unsized();
  int inferred_row = 0, inferred_col = 0;
  std::string s;
  while (getline(file, s)) {
    if (s == "")
      break;
    if (inferred_col == 0)
      inferred_col = s.size();

    assert((int)s.size() == inferred_col);
    for (char c : s)
      grid.push_back(c);

    ++inferred_row;
  }
  grid.resize(inferred_row, inferred_col);

  std::vector<Direction> directions;
  while (getline(file, s)) {
    for (char c : s) {
      directions.push_back([](char ch) -> Direction {
        switch (ch) {
        case '<':
          return Direction::Left;
        case '>':
          return Direction::Right;
        case '^':
          return Direction::Up;
        case 'v':
          return Direction::Down;
        default:
          assert("Unknown direction" && false);
        };
        __builtin_unreachable();
      }(c));
    }
  }

  return World{.grid = std::move(grid), .directions = std::move(directions)};
}

inline bool valid_spot(int row, int col, int cur_row, int cur_col) {
  return cur_row >= 0 && cur_row < row && cur_col >= 0 && cur_col < col;
}

constexpr int dr[] = {0, 1, 0, -1};
constexpr int dc[] = {1, 0, -1, 0};
constexpr int DIRS = 4;

bool try_move_horizontal(Vector2D<char>& grid, pii pos, Direction direction) {
  const int rows = grid.height(), cols = grid.width();
  pii next_pos = std::make_pair(pos.first + dr[static_cast<int>(direction)],
                                pos.second + dc[static_cast<int>(direction)]);
  if (!valid_spot(rows, cols, next_pos.first, next_pos.second) ||
      grid[next_pos.first][next_pos.second] == WALL)
    return false;

  if (grid[next_pos.first][next_pos.second] != SPACE &&
      !try_move_horizontal(grid, next_pos, direction))
    return false;

  std::swap(grid[pos.first][pos.second], grid[next_pos.first][next_pos.second]);
  return true;
}

// has to move simultaneously for up/down.
bool try_move_vertical(Vector2D<char>& grid, std::set<pii> pos,
                       Direction direction) {
  if (pos.empty())
    return true;

  const int rows = grid.height(), cols = grid.width();
  std::set<pii> next_pos;
  for (pii p : pos) {
    const int x = p.first + dr[static_cast<int>(direction)],
              y = p.second + dc[static_cast<int>(direction)];
    if (!valid_spot(rows, cols, x, y) || grid[x][y] == WALL) {
      return false;
    }
    if (grid[x][y] == SPACE)
      continue;

    const char ch = grid[x][y];
    if (ch == BOX_L || ch == BOX_R) {
      const int y2 = (ch == BOX_L) ? y + 1 : y - 1;
      assert(valid_spot(rows, cols, x, y2));
      assert(grid[x][std::min(y, y2)] == BOX_L);
      assert(grid[x][std::max(y, y2)] == BOX_R);
      next_pos.insert(std::make_pair(x, y));
      next_pos.insert(std::make_pair(x, y2));
    }
  }

  if (!try_move_vertical(grid, std::move(next_pos), direction))
    return false;

  for (pii p : pos) {
    const int x = p.first + dr[static_cast<int>(direction)],
              y = p.second + dc[static_cast<int>(direction)];
    assert(valid_spot(rows, cols, x, y));
    std::swap(grid[p.first][p.second], grid[x][y]);
  }
  return true;
}

bool try_move(Vector2D<char>& grid, std::set<pii> pos, Direction direction) {
  switch (direction) {
  case Direction::Left:
  case Direction::Right:
    return try_move_horizontal(grid, *pos.begin(), direction);
  case Direction::Up:
  case Direction::Down:
    return try_move_vertical(grid, pos, direction);
  }
}

Vector2D<char> horizontal_scale_2x(const Vector2D<char>& grid) {
  const int width = grid.width(), height = grid.height();
  Vector2D<char> res(height, width * 2);
  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      switch (grid[i][j]) {
      case SPACE: {
        res[i][j * 2] = SPACE;
        res[i][j * 2 + 1] = SPACE;
        break;
      }
      case BOX_1: {
        res[i][j * 2] = BOX_L;
        res[i][j * 2 + 1] = BOX_R;
        break;
      }
      case WALL: {
        res[i][j * 2] = WALL;
        res[i][j * 2 + 1] = WALL;
        break;
      }
      case ROBOT: {
        res[i][j * 2] = ROBOT;
        res[i][j * 2 + 1] = SPACE;
        break;
      }
      }
    }
  }

  return res;
}

long long solve(World& world) {
  world.grid = horizontal_scale_2x(world.grid);
  print(world.grid);

  const int rows = world.grid.height(), cols = world.grid.width();
  pii robot = [&]() {
    for (int i = 0; i < rows; ++i) {
      for (int j = 0; j < cols; ++j) {
        if (world.grid[i][j] == ROBOT)
          return std::make_pair(i, j);
      }
    }
    assert("Robot not found" && false);
  }();

  for (Direction dir : world.directions) {
    using namespace std::chrono_literals;
    // std::this_thread::sleep_for(250ms);
    // std::cout << "\033[2J\033[1;1H";
    // print(world.grid);
    if (try_move(world.grid, std::set<pii>{robot}, dir)) {
      robot = std::make_pair(robot.first + dr[static_cast<int>(dir)],
                             robot.second + dc[static_cast<int>(dir)]);
    }
  }

  print(world.grid);

  long long sum = 0;
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      if (world.grid[i][j] == BOX_L) {
        sum += (100 * i) + j;
      }
    }
  }

  return sum;
}

} // namespace

int main() {
  World world = read_input();
  std::cout << solve(world) << '\n';
  return 0;
}
