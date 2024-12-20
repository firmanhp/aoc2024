#pragma once

#include <vector>

namespace util {

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

  const T& operator[](std::pair<int, int> pos) const {
    return (*this)[pos.first][pos.second];
  }

  T& operator[](std::pair<int, int> pos) {
    return (*this)[pos.first][pos.second];
  }

  int height() const { return row_; }
  int width() const { return col_; }

private:
  Vector2D() {}

  int row_, col_;
  std::vector<T> container;
};

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

inline bool valid_spot(const Vector2D<char>& grid, std::pair<int, int> pos) {
  const int rows = grid.height(), cols = grid.width();
  return pos.first >= 0 && pos.first < rows && pos.second >= 0 &&
         pos.second < cols;
}

} // namespace util
