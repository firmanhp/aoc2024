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

struct PQEntry {
  int cost;
  pii pos;
  inline int operator<=>(const PQEntry& o) const { return cost - o.cost; }
};

int dijkstra_shortest_path(const Vector2D<char>& grid, pii from, pii to,
                           std::optional<pii> portal_from = std::nullopt,
                           std::optional<pii> portal_to = std::nullopt) {
  const int rows = grid.height(), cols = grid.width();
  Vector2D<int> dist(rows, cols);
  for (int i = 0; i < rows; ++i)
    for (int j = 0; j < cols; ++j)
      dist[i][j] = INF;

  std::priority_queue<PQEntry, std::vector<PQEntry>, std::greater<PQEntry>> pq;
  dist[from] = 0;
  pq.push(PQEntry{.cost = 0, .pos = from});
  while (!pq.empty()) {
    const auto e = pq.top();
    const int cost = e.cost;
    const pii pos = e.pos;
    pq.pop();
    if (cost > dist[pos])
      continue;
    if (pos == to)
      return cost;

    for (int d = 0; d < DIRS; ++d) {
      auto next_pos = std::make_pair(pos.first + dr[d], pos.second + dc[d]);
      auto next_cost = cost + 1;
      if (!valid_spot(grid, next_pos) || grid[next_pos] == WALL ||
          dist[next_pos] < next_cost)
        continue;

      dist[next_pos] = next_cost;
      pq.push(PQEntry{.cost = next_cost, .pos = next_pos});
    }

    if (pos == portal_from) {
      auto next_pos = *portal_to;
      auto next_cost = cost + manhattan_dist(pos, next_pos);
      if (!valid_spot(grid, next_pos) || grid[next_pos] == WALL ||
          dist[next_pos] < next_cost)
        continue;

      dist[next_pos.first][next_pos.second] = next_cost;
      pq.push(PQEntry{.cost = next_cost, .pos = next_pos});
    }
  }

  return dist[to];
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

  std::mutex mtx;
  std::map<int, int> cheat_counts;
  // this might take a while...
  const int cur_sp = dijkstra_shortest_path(grid, start, end);
  {
    ThreadPool pool(15);
    for (int i = 0; i < rows; ++i)
      for (int j = 0; j < cols; ++j)
        for (int i2 = 0; i2 < rows; ++i2)
          for (int j2 = 0; j2 < cols; ++j2) {
            // std::cout << rows << " " << cols << '\n';
            // std::cout << i << " " << j << " " << i2 << " " << j2 << '\n';
            const pii pos1 = std::make_pair(i, j);
            const pii pos2 = std::make_pair(i2, j2);
            if (pos1 == pos2 || grid[pos1] == WALL || grid[pos2] == WALL ||
                manhattan_dist(pos1, pos2) > MAX_CHEAT_PS)
              continue;

            pool.add_job([&grid, &cheat_counts, &mtx, &start, &end, pos1, pos2,
                          cur_sp]() {
              // std::cout << "Executing " << pos1 << " " << pos2 << '\n';
              const int sp =
                  dijkstra_shortest_path(grid, start, end, pos1, pos2);
              assert(sp <= cur_sp);
              const int saving = cur_sp - sp;
              {
                std::lock_guard<std::mutex> lock(mtx);
                ++cheat_counts[saving];
              }
            });
          }
    pool.wait_all();
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
