#include <algorithm>
#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <span>
#include <sstream>
#include <thread>
#include <unordered_set>

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

constexpr int dr[] = {0, 1, 0, -1};
constexpr int dc[] = {1, 0, -1, 0};
constexpr int DIRS = 4;

int solve(const Vector2D<char>& grid) {
  using PQEntry = std::pair<int, std::pair<int, int>>;

  const int rows = grid.height(), cols = grid.width();

  // bottom up DP -- using PQ
  Vector2D<std::unordered_set<int>> dp(rows, cols);
  // fill from 9
  std::priority_queue<PQEntry, std::vector<PQEntry>, std::less<PQEntry>> pq;
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      pq.push(std::make_pair(grid[i][j] - '0', std::make_pair(i, j)));
    }
  }

  while (!pq.empty()) {
    const auto [val, pos] = pq.top();
    pq.pop();
    const auto [row, col] = pos;
    if (val == 9) {
      dp[row][col].insert(row * cols + col);
      continue;
    }

    for (int d = 0; d < DIRS; ++d) {
      const int nr = row + dr[d];
      const int nc = col + dc[d];
      if (valid_spot(rows, cols, nr, nc) && (grid[nr][nc] - '0' == val + 1)) {
        for (int poss : dp[nr][nc]) {
          dp[row][col].insert(poss);
        }
      }
    }
  }

  long long ans = 0;
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      if (grid[i][j] == '0')
        ans += dp[i][j].size();
    }
  }

  return ans;
}

} // namespace

int main() {
  Vector2D<char> grid = read_input();
  std::cout << solve(grid) << '\n';
  return 0;
}
