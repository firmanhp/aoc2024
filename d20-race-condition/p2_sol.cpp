#include <algorithm>
#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <span>
#include <sstream>
#include <thread>
#include <unordered_set>

#include "input.h"
#include "util/pair_extensions.h"
#include "util/thread_pool.h"
#include "util/vector_2d.h"

namespace {

using util::print;
using util::ThreadPool;
using util::valid_spot;
using util::Vector2D;
using pii = std::pair<int, int>;

#ifdef TEST
constexpr std::string INPUT = "p1_input.txt";
constexpr int GOOD_CHEAT_THRESHOLD_PS = 100;
#endif
#ifdef SAMPLE
constexpr std::string INPUT = "p1_sample.txt";
constexpr int GOOD_CHEAT_THRESHOLD_PS = 50;
#endif

constexpr int dr[] = {0, 1, 0, -1};
constexpr int dc[] = {1, 0, -1, 0};
constexpr int DIRS = 4;
constexpr int INF = 1e9;
constexpr int SPACE = '.';
constexpr int WALL = '#';
constexpr int START = 'S';
constexpr int END = 'E';
constexpr int MAX_CHEAT_PS = 20;
constexpr int MAX_JOBS = 15;

int manhattan_dist(pii from, pii to) {
  return std::abs(from.first - to.first) + std::abs(from.second - to.second);
}

Vector2D<int> bfs_sssp(const Vector2D<char>& grid, pii from) {
  const int rows = grid.height(), cols = grid.width();
  Vector2D<int> dist(rows, cols);
  for (int i = 0; i < rows; ++i)
    for (int j = 0; j < cols; ++j)
      dist[i][j] = INF;

  std::queue<pii> bfs_q;
  dist[from.first][from.second] = 0;
  bfs_q.push(from);
  while (!bfs_q.empty()) {
    auto pos = bfs_q.front();
    bfs_q.pop();

    for (int d = 0; d < DIRS; ++d) {
      auto next_pos = std::make_pair(pos.first + dr[d], pos.second + dc[d]);
      if (!valid_spot(grid, next_pos) ||
          grid[next_pos.first][next_pos.second] == WALL ||
          dist[next_pos.first][next_pos.second] < INF)
        continue;

      dist[next_pos.first][next_pos.second] = dist[pos.first][pos.second] + 1;
      bfs_q.push(next_pos);
    }
  }

  return dist;
}

int solve(Vector2D<char>& grid) {
  const int rows = grid.height(), cols = grid.width();
  pii start, end;
  for (int i = 0; i < rows; ++i)
    for (int j = 0; j < cols; ++j) {
      if (grid[i][j] == START)
        start = std::make_pair(i, j);
      if (grid[i][j] == END)
        end = std::make_pair(i, j);
    }

  std::map<int, int> cheat_counts;
  const Vector2D<int> from_start = bfs_sssp(grid, start);
  const Vector2D<int> to_end = bfs_sssp(grid, end);
  const int cur_sp = from_start[end];

  for (int i = 0; i < rows; ++i)
    for (int j = 0; j < cols; ++j)
      for (int i2 = 0; i2 < rows; ++i2)
        for (int j2 = 0; j2 < cols; ++j2) {
          // std::cout << rows << " " << cols << '\n';
          // std::cout << i << " " << j << " " << i2 << " " << j2 << '\n';
          const pii pos1 = std::make_pair(i, j);
          const pii pos2 = std::make_pair(i2, j2);
          const int cheat_cost = manhattan_dist(pos1, pos2);
          if (pos1 == pos2 || grid[pos1] == WALL || grid[pos2] == WALL ||
              cheat_cost > MAX_CHEAT_PS)
            continue;

          // go through the cheat path
          const int sp = from_start[pos1] + cheat_cost + to_end[pos2];
          if (sp > cur_sp)
            continue;
          const int saving = cur_sp - sp;
          ++cheat_counts[saving];
        }

  int ans = 0;
  for (const auto& [saving, count] : cheat_counts) {
    if (saving < GOOD_CHEAT_THRESHOLD_PS)
      continue;
    ans += count;
    std::cout << "Saving: " << saving << " - " << count << '\n';
  }

  return ans;
}

} // namespace

int main() {
  auto input = input::read_input(INPUT);
  std::cout << solve(input) << '\n';
  return 0;
}
