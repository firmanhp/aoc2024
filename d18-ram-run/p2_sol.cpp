#include <algorithm>
#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <span>
#include <sstream>
#include <thread>
#include <unordered_set>

#include "input.h"
#include "util/vector_2d.h"

namespace {

using util::print;
using util::Vector2D;
using pii = std::pair<int, int>;

#ifdef TEST
constexpr int GRID_DIM = 71;
constexpr int SIMULATE = 1024;
constexpr std::string INPUT = "p1_input.txt";
#endif
#ifdef SAMPLE
constexpr int GRID_DIM = 7;
constexpr int SIMULATE = 12;
constexpr std::string INPUT = "p1_sample.txt";
#endif

inline bool valid_spot(int cur_row, int cur_col) {
  return cur_row >= 0 && cur_row < GRID_DIM && cur_col >= 0 &&
         cur_col < GRID_DIM;
}

constexpr int dr[] = {0, 1, 0, -1};
constexpr int dc[] = {1, 0, -1, 0};
constexpr int DIRS = 4;
constexpr int INF = 1e9;

int min_cost[GRID_DIM][GRID_DIM];
bool blocked[GRID_DIM][GRID_DIM];

int try_solve(const std::vector<pii>& input, int max_simulate) {
  for (int i = 0; i < GRID_DIM; ++i) {
    for (int j = 0; j < GRID_DIM; ++j) {
      min_cost[i][j] = INF;
      blocked[i][j] = false;
    }
  }

  for (int i = 0; i < max_simulate; ++i) {
    const pii& pos = input[i];
    // input has cartesian style order
    blocked[pos.second][pos.first] = true;
  }

  std::queue<pii> bfs_q;
  min_cost[0][0] = 0;
  bfs_q.push(std::make_pair(0, 0));
  while (!bfs_q.empty()) {
    auto entry = bfs_q.front();
    bfs_q.pop();
    for (int d = 0; d < DIRS; ++d) {
      const pii np = std::make_pair(entry.first + dr[d], entry.second + dc[d]);
      if (!valid_spot(np.first, np.second) ||
          min_cost[np.first][np.second] < INF || blocked[np.first][np.second])
        continue;

      min_cost[np.first][np.second] = min_cost[entry.first][entry.second] + 1;
      bfs_q.push(np);
    }
  }

  return min_cost[GRID_DIM - 1][GRID_DIM - 1];
}

int solve(const std::vector<pii>& input) {
  // binary search
  int l = 0, r = input.size();
  int ans = INF;
  while (l <= r) {
    const int mid = (l + r) >> 1;
    if (try_solve(input, /*max_simulate=*/mid) == INF) {
      ans = std::min(mid, ans);
      r = mid - 1;
    } else {
      l = mid + 1;
    }
  }

  // our answer is already 1-based
  return ans;
}

} // namespace

int main() {
  auto input = input::read_input(INPUT);
  std::cout << solve(input) << '\n';
  return 0;
}
