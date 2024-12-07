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

constexpr std::string VALID[] = {
    "M.S"
    ".A."
    "M.S",

    "S.M"
    ".A."
    "S.M",

    "S.S"
    ".A."
    "M.M",

    "M.M"
    ".A."
    "S.S",
};

bool is_xmas(const Vector2D<char> grid, int s_row, int s_col) {
  const int rows = grid.height(), cols = grid.width();
  if (s_row + 2 >= rows || s_col + 2 >= cols)
    return false;

  for (const auto& candidate : VALID) {
    bool found = true;
    for (int i = 0; i < 3; ++i) {
      for (int j = 0; j < 3; ++j) {
        const char ch = candidate[i * 3 + j];
        if (ch == '.')
          continue;

        const int r = s_row + i, c = s_col + j;
        if (ch != grid[r][c]) {
          found = false;
          break;
        }
      }
      if (!found)
        break;
    }

    if (found)
      return true;
  }
  return false;
}

int count_xmas(const Vector2D<char> grid) {
  const int rows = grid.height(), cols = grid.width();
  int xmas_count = 0;
  for (int i = 0; i + 2 < rows; ++i) {
    for (int j = 0; j + 2 < cols; ++j) {
      if (is_xmas(grid, i, j))
        ++xmas_count;
    }
  }

  return xmas_count;
}

} // namespace

int main() {
  std::cout << count_xmas(read_input()) << '\n';
  return 0;
}
