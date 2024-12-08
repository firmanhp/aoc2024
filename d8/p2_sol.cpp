#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <numeric>
#include <span>
#include <sstream>
#include <unordered_set>
#include <utility>

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

struct Rational {
  int numerator, denominator;
  Rational(int n, int d) : numerator(n), denominator(d) {}
  inline bool operator==(const Rational& other) const {
    return numerator == other.numerator && denominator == other.denominator;
  }

  inline Rational operator*(int mult) const {
    return Rational(numerator * mult, denominator * mult);
  }

  Rational simplify() const {
    int g = std::gcd(numerator, denominator);
    return Rational(numerator / g, denominator / g);
  }
};

struct Point {
  int x, y;

  inline Rational operator-(const Point& other) const {
    return Rational(y - other.y, x - other.x);
  }

  inline bool operator==(const Point& other) const {
    return x == other.x && y == other.y;
  }

  inline int operator<=>(const Point& other) const {
    if (x == other.x) {
      return y - other.y;
    }
    return x - other.x;
  }
};

inline std::ostream& operator<<(std::ostream& stream, const Point& p) {
  return stream << "(" << p.x << " " << p.y << ")";
}

} // namespace

template <> struct std::hash<Point> {
  std::size_t operator()(const Point& p) const noexcept {
    auto h1 = std::hash<int>{}(p.x);
    auto h2 = std::hash<int>{}(p.y);
    return h1 ^ h2;
  }
};

namespace {

using Antinodes = std::unordered_set<Point>;

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

void fill_antinodes(int rows, int cols, std::span<const Point> antenna_pos,
                    Antinodes& res) {
  const int N = antenna_pos.size();
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      Point possible_antinode(i, j);
      if (res.contains(possible_antinode))
        continue;
      bool found = false;
      for (int a1 = 0; !found && a1 < N; ++a1) {
        if (antenna_pos[a1] == possible_antinode)
          continue;
        for (int a2 = 0; !found && a2 < N; ++a2) {
          if (a1 == a2 || antenna_pos[a2] == possible_antinode)
            continue;

          Rational distance = antenna_pos[a1] - possible_antinode;
          Rational distance_2 = antenna_pos[a2] - possible_antinode;
          if (distance.simplify() == distance_2.simplify()) {
            // std::cout << "found antinode " << possible_antinode << " from pos
            // "
            //           << antenna_pos[a1] << " " << antenna_pos[a2] << '\n';
            found = true;
            res.insert(possible_antinode);
          }
        }
      }
    }
  }

  if (N > 1) {
    // This means that some of the new antinodes will occur at the position of
    // each antenna (unless that antenna is the only one of its frequency).
    for (const Point antenna : antenna_pos) {
      res.insert(antenna);
    }
  }
}

void draw_grid(const Vector2D<char>& grid, const Antinodes& antinodes) {
  const int rows = grid.height(), cols = grid.width();
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      if (antinodes.contains(Point(i, j)))
        std::cout << '#';
      else
        std::cout << grid[i][j];
    }
    std::cout << '\n';
  }
}

constexpr char SPACE = '.';
int solve(const Vector2D<char>& grid) {
  std::unordered_map<char, std::vector<Point>> antennas;
  const int rows = grid.height(), cols = grid.width();
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      if (grid[i][j] == SPACE)
        continue;
      const char ch = grid[i][j];
      antennas[ch].push_back(Point(i, j));
    }
  }

  Antinodes total_antinodes;
  for (const auto& [_, pos] : antennas) {
    // std::cout << "check " << _ << '\n';
    fill_antinodes(rows, cols, pos, total_antinodes);
  }

  // draw_grid(grid, total_antinodes);
  return total_antinodes.size();
}

} // namespace

int main() {
  std::cout << solve(read_input()) << '\n';
  return 0;
}
