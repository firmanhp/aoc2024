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
#include "util/vector_2d.h"

namespace {

using util::print;
using util::valid_spot;
using util::Vector2D;
using pii = std::pair<int, int>;

#ifdef TEST
constexpr std::string INPUT = "p1_input.txt";
constexpr int GOOD_CHEAT_THRESHOLD_PS = 100;
#endif
#ifdef SAMPLE
constexpr std::string INPUT = "p1_sample.txt";
constexpr int GOOD_CHEAT_THRESHOLD_PS = 1;
#endif

constexpr int dr[] = {0, 1, 0, -1};
constexpr int dc[] = {1, 0, -1, 0};
constexpr int DIRS = 4;
constexpr int INF = 1e9;
constexpr int SPACE = '.';
constexpr int WALL = '#';
constexpr int START = 'S';
constexpr int END = 'E';

bool worth_to_cheat(const Vector2D<char>& grid, pii pos) {
  // TODO: this has a bug
  return true;

  assert(valid_spot(grid, pos));
  assert(grid[pos.first][pos.second] == WALL);
  bool ans = false;

  for (int d1 = 0; d1 < 4; ++d1) {
    pii pos1 = std::make_pair(pos.first + dr[d1], pos.first + dr[d1]);
    for (int d2 = 0; d2 < 4; ++d2) {
      if (d1 == d2)
        continue;
      pii pos2 = std::make_pair(pos.first + dr[d2], pos.second + dc[d2]);
      // only those that give way
      ans |= valid_spot(grid, pos1) && valid_spot(grid, pos2) &&
             grid[pos1.first][pos1.second] == SPACE &&
             grid[pos2.first][pos2.second] == SPACE;
    }
  }
  return ans;
}

int bfs_shortest_path(const Vector2D<char>& grid, pii from, pii to) {
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
    if (pos == to)
      break;

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

  return dist[to.first][to.second];
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
  const int cur_sp = bfs_shortest_path(grid, start, end);
  for (int i = 0; i < rows; ++i)
    for (int j = 0; j < cols; ++j) {
      if (grid[i][j] != WALL || !worth_to_cheat(grid, std::make_pair(i, j)))
        continue;

      grid[i][j] = SPACE;
      const int sp = bfs_shortest_path(grid, start, end);
      grid[i][j] = WALL;
      assert(sp <= cur_sp);
      const int saving = cur_sp - sp;
      ++cheat_counts[saving];
    }

  int ans = 0;
  for (const auto& [saving, count] : cheat_counts) {
    if (saving >= GOOD_CHEAT_THRESHOLD_PS)
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
