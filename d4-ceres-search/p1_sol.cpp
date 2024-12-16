#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <span>
#include <sstream>

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

constexpr std::string XMAS = "XMAS";

int count_xmas(const Vector2D<char> grid) {
  const int rows = grid.height(), cols = grid.width();

  constexpr int dr[] = {0, 0, 1, -1, 1, -1, 1, -1};
  constexpr int dc[] = {1, -1, 0, 0, 1, -1, -1, 1};
  constexpr int DIRS = std::size(dr);

  int xmas_count = 0;
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      // Prune
      if (grid[i][j] != 'X')
        continue;

      for (int d = 0; d < DIRS; ++d) {
        std::string candidate;
        candidate.reserve(4);
        for (int r = i, c = j, k = 0; valid_spot(rows, cols, r, c) && k < 4;
             ++k, r += dr[d], c += dc[d]) {
          candidate.push_back(grid[r][c]);
        }
        if (candidate == XMAS) {
          ++xmas_count;
        }
      }
    }
  }

  return xmas_count;
}

} // namespace

int main() {
  std::cout << count_xmas(read_input()) << '\n';
  return 0;
}
