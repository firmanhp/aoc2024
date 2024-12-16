#include <algorithm>
#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <span>
#include <sstream>
#include <thread>

namespace {

template <typename T> struct Vector2D {
public:
  Vector2D(int row, int col) : row_(row), col_(col), container(row * col) {}
  static Vector2D<T> unsized() { return Vector2D{}; }

  void resize(int row, int col) {
    row_ = row;
    col_ = col;
    container.resize(row * col);
  }

  void push_back(T data) { container.push_back(data); }

  T* operator[](int row) {
    assert("unsized" && col_ > 0);
    return container.data() + (row * col_);
  }

  const T* operator[](int row) const {
    assert("unsized" && col_ > 0);
    return container.data() + (row * col_);
  }

  int height() const { return row_; }
  int width() const { return col_; }

private:
  Vector2D() {}

  int row_, col_;
  std::vector<T> container;
};

Vector2D<char> read_input() {
  std::fstream file;
  file.open("p1_input.txt", std::ios_base::in);
  assert(file.is_open());

  Vector2D<char> grid = Vector2D<char>::unsized();
  int inferred_row = 0, inferred_col = 0;
  std::string s;
  while (getline(file, s)) {
    if (inferred_col == 0) {
      inferred_col = s.size();
    }
    assert(s.size() == inferred_col);
    for (char c : s)
      grid.push_back(c);

    ++inferred_row;
  }

  grid.resize(inferred_row, inferred_col);
  return grid;
}

inline bool valid_spot(int row, int col, int cur_row, int cur_col) {
  return cur_row >= 0 && cur_row < row && cur_col >= 0 && cur_col < col;
}

constexpr char WALL = '#';
constexpr char FILL = 'X';
constexpr int dr[] = {0, 1, 0, -1};
constexpr int dc[] = {1, 0, -1, 0};
constexpr int DIRS = 4;

std::pair<std::pair<int, int> /*r,c*/, int /*d_idx (direction)*/>
find_guard(const Vector2D<char>& grid) {
  const int rows = grid.height(), cols = grid.width();
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      switch (grid[i][j]) {
      case '>':
        return {std::make_pair(i, j), 0};
      case 'v':
        return {std::make_pair(i, j), 1};
      case '<':
        return {std::make_pair(i, j), 2};
      case '^':
        return {std::make_pair(i, j), 3};
      default:
        continue;
      }
    }
  }

  assert("Guard not found" && false);
}

// satisfying
void print_grid_delayed(const Vector2D<char>& grid) {
  using namespace std::chrono_literals;
  const int rows = grid.height(), cols = grid.width();
  std::this_thread::sleep_for(10ms);
  std::cout << "\033[2J\033[1;1H";
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      std::cout << grid[i][j];
    }
    std::cout << '\n';
  }
}

int solve(Vector2D<char>& grid) {
  const int rows = grid.height(), cols = grid.width();
  auto [pos, direction] = find_guard(grid);

  int cur_r = pos.first, cur_c = pos.second;
  while (true) { // we assume the guard will eventually falls

    grid[cur_r][cur_c] = FILL;
    // print_grid_delayed(grid);
    const int next_r = cur_r + dr[direction], next_c = cur_c + dc[direction];
    if (!valid_spot(rows, cols, next_r, next_c)) {
      break;
    }

    if (grid[next_r][next_c] == WALL) {
      direction = (direction + 1) % DIRS;
      continue;
    }
    cur_r = next_r;
    cur_c = next_c;
  }

  int spots = 0;
  for (int i = 0; i < rows; ++i)
    for (int j = 0; j < cols; ++j)
      if (grid[i][j] == FILL)
        ++spots;

  return spots;
}

} // namespace

int main() {
  Vector2D<char> grid = read_input();
  std::cout << solve(grid) << '\n';
  return 0;
}
