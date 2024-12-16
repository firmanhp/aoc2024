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

long long bfs_area(const Vector2D<char>& grid, Vector2D<uint8_t>& visit_flag,
                   Vector2D<char>& border_grid, int row, int col) {
  const int rows = grid.height(), cols = grid.width();
  assert(valid_spot(rows, cols, row, col));
  const char ch = grid[row][col];
  long long area = 0;

  constexpr int dr[] = {0, 0, 1, -1, 1, 1, -1, -1};
  constexpr int dc[] = {1, -1, 0, 0, 1, -1, 1, -1};
  constexpr int DIRS = 4;
  constexpr int DIRS_ALL = 8;

  auto is_border = [&](int row, int col) {
    for (int d = 0; d < DIRS_ALL; ++d) {
      int nr = row + dr[d], nc = col + dc[d];
      if (!valid_spot(rows, cols, nr, nc) || grid[nr][nc] != ch) {
        // we found a border
        return true;
      }
    }
    return false;
  };

  std::queue<std::pair<int, int>> bfs_q;
  bfs_q.push(std::make_pair(row, col));
  ++area;
  visit_flag[row][col] = true;
  while (!bfs_q.empty()) {
    const auto [cr, cc] = bfs_q.front();
    bfs_q.pop();

    if (is_border(cr, cc)) {
      border_grid[cr][cc] = ch;
    }

    for (int d = 0; d < DIRS; ++d) {
      const int nr = cr + dr[d], nc = cc + dc[d];
      if (!valid_spot(rows, cols, nr, nc) || grid[nr][nc] != ch ||
          visit_flag[nr][nc]) {
        continue;
      }

      visit_flag[nr][nc] = true;
      ++area;
      bfs_q.push(std::make_pair(nr, nc));
    }
  }

  return area;
}

void print(const Vector2D<char>& grid) {
  const int rows = grid.height(), cols = grid.width();
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      if (grid[i][j] == 0)
        std::cout << '.';
      else
        std::cout << grid[i][j];
    }
    std::cout << '\n';
  }
  std::cout << '\n';
}

// clean a single perimeter.
// returns number of sides
int clear_perimeter(Vector2D<char>& border_grid, int row, int col) {
  const int rows = border_grid.height(), cols = border_grid.width();
  assert(valid_spot(rows, cols, row, col));
  const char ch = border_grid[row][col];
  assert(ch > 0);
  int sides = 0;

  // down, right, up, left
  constexpr int dr[] = {1, 0, -1, 0};
  constexpr int dc[] = {0, 1, 0, -1};
  constexpr int DIRS = std::size(dr);

  int cr = row, cc = col;
  int cd = 0;

  auto turn_right_ok = [&](int cur_r, int cur_c) {
    const int cur_right = (cd - 1 + DIRS) % DIRS;
    const int nr = cur_r + dr[cur_right], nc = cur_c + dc[cur_right];
    return valid_spot(rows, cols, nr, nc) && border_grid[nr][nc] == ch;
  };

  std::vector<std::pair<int, int>> to_clear;
  do {
    // std::cout << cr << " " << cc << '\n';
    to_clear.push_back(std::make_pair(cr, cc));
    if (turn_right_ok(cr, cc)) {
      // can turn right
      cd = (cd - 1 + DIRS) % DIRS;
      ++sides;
      cr = cr + dr[cd];
      cc = cc + dc[cd];
      continue;
    }

    const int nr = cr + dr[cd], nc = cc + dc[cd];
    if (!valid_spot(rows, cols, nr, nc) || border_grid[nr][nc] != ch) {
      // we cannot go further, need to turn left
      // this is one side.
      cd = (cd + 1) % DIRS;
      ++sides;
    }

    cr = cr + dr[cd];
    cc = cc + dc[cd];
  } while (!(cr == row && cc == col));

  // clear the visited borders.
  for (const auto& [r, c] : to_clear) {
    border_grid[r][c] = 0;
  }

  // the last side was not counted.
  return sides + 1;
}

int clear_borders(Vector2D<char>& border_grid, int row, int col) {
  const int rows = border_grid.height(), cols = border_grid.width();
  assert(valid_spot(rows, cols, row, col));
  const char ch = border_grid[row][col];
  assert(ch > 0);
  // find perimeters, and clear them
  int sides = 0;

  // print(border_grid);
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      if (border_grid[i][j] == ch) {
        sides += clear_perimeter(border_grid, i, j);
        // print(border_grid);
      }
    }
  }

  return sides;
}

// Scale twice from 1x1 -> (scale)x(scale);
Vector2D<char> scale(const Vector2D<char>& grid, int scale) {
  assert(scale > 1);
  const int rows = grid.height(), cols = grid.width();
  Vector2D<char> scaled(scale * rows, scale * cols);
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      const char ch = grid[i][j];
      for (int i2 = 0; i2 < scale; ++i2) {
        for (int j2 = 0; j2 < scale; ++j2) {
          scaled[i * scale + i2][j * scale + j2] = ch;
        }
      }
    }
  }

  return scaled;
}

long long solve(Vector2D<char>& grid) {
  constexpr int SCALE = 2;
  Vector2D<char> grid_scaled = scale(grid, SCALE);
  const int rows = grid_scaled.height(), cols = grid_scaled.width();
  // print(grid_scaled);
  Vector2D<uint8_t> visit_flag(rows, cols);
  Vector2D<char> border_grid(rows, cols);
  long long price = 0;
  for (int row = 0; row < rows; ++row) {
    for (int col = 0; col < cols; ++col) {
      if (!visit_flag[row][col]) {
        const long long area =
            bfs_area(grid_scaled, visit_flag, border_grid, row, col) /
            (SCALE * SCALE);
        const int sides = clear_borders(border_grid, row, col);
        std::cout << grid_scaled[row][col] << " -> " << area << " x " << sides
                  << '\n';
        assert(area > 0);
        assert(sides > 0);
        price += area * sides;
      }
    }
  }
  return price;
}

} // namespace

int main() {
  Vector2D<char> grid = read_input();
  std::cout << solve(grid) << '\n';
  return 0;
}
