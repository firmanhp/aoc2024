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

std::pair<long long, long long> bfs_measure(const Vector2D<char>& grid,
                                            Vector2D<uint8_t>& visit_flag,
                                            int row, int col) {
  const int rows = grid.height(), cols = grid.width();
  assert(valid_spot(rows, cols, row, col));
  const char ch = grid[row][col];
  long long area = 0, perimeter = 0;

  constexpr int dr[] = {0, 0, 1, -1};
  constexpr int dc[] = {1, -1, 0, 0};
  constexpr int DIRS = std::size(dr);
  std::queue<std::pair<int, int>> bfs_q;
  bfs_q.push(std::make_pair(row, col));
  ++area;
  visit_flag[row][col] = true;
  while (!bfs_q.empty()) {
    const auto [cr, cc] = bfs_q.front();
    bfs_q.pop();
    for (int d = 0; d < DIRS; ++d) {
      const int nr = cr + dr[d], nc = cc + dc[d];
      if (!valid_spot(rows, cols, nr, nc) || grid[nr][nc] != ch) {
        // we found a border
        ++perimeter;
        continue;
      }
      if (!visit_flag[nr][nc]) {
        visit_flag[nr][nc] = true;
        ++area;
        bfs_q.push(std::make_pair(nr, nc));
      }

      // a visited pos of the same plant is not a border.
    }
  }

  return std::make_pair(area, perimeter);
}

long long solve(Vector2D<char>& grid) {
  const int rows = grid.height(), cols = grid.width();
  Vector2D<uint8_t> visit_flag(rows, cols);
  long long price = 0;
  for (int row = 0; row < rows; ++row) {
    for (int col = 0; col < cols; ++col) {
      if (!visit_flag[row][col]) {
        const auto [area, perimeter] = bfs_measure(grid, visit_flag, row, col);
        std::cout << grid[row][col] << " -> " << area << " x " << perimeter
                  << '\n';
        assert(area > 0);
        assert(perimeter > 0);
        price += area * perimeter;
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
