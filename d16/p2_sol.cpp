#include <algorithm>
#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <queue>
#include <span>
#include <sstream>
#include <thread>
#include <unordered_set>

#include "util/vector_2d.h"

namespace {

using util::print;
using util::Vector2D;
using pii = std::pair<int, int>;

enum class Direction : int { East = 0, South, West, North };

struct Cost {
  int direction[4];
};

struct PQState {
  int cost;
  pii pos;
  Direction direction;

  inline int operator<=>(const PQState& o) const { return cost - o.cost; }
};

constexpr char START = 'S';
constexpr char END = 'E';
constexpr char WALL = '#';
constexpr char BEST_PATH = 'O';
constexpr int COST_TURN = 1000;
constexpr int COST_MOVE = 1;
constexpr int INF = 2 * 1e9;

Direction cw(Direction d) {
  return static_cast<Direction>((static_cast<int>(d) + 1) % 4);
}

Direction ccw(Direction d) {
  return static_cast<Direction>((4 + static_cast<int>(d) - 1) % 4);
}

Vector2D<char> read_input() {
  std::fstream file;
  file.open("p1_input.txt", std::ios_base::in);
  assert(file.is_open());

  Vector2D<char> grid = Vector2D<char>::unsized();
  int inferred_row = 0, inferred_col = 0;
  std::string s;
  while (getline(file, s)) {
    if (inferred_col == 0)
      inferred_col = s.size();

    assert((int)s.size() == inferred_col);
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

Vector2D<Cost> exec_sssp(const Vector2D<char>& grid, pii start) {
  const int rows = grid.height(), cols = grid.width();
  Vector2D<Cost> res(rows, cols);
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j)
      res[i][j] = Cost{.direction = {INF, INF, INF, INF}};
  }

  std::priority_queue<PQState, std::vector<PQState>, std::greater<PQState>> pq;

  auto try_update = [&](PQState new_state) {
    const auto [x, y] = new_state.pos;
    if (res[x][y].direction[static_cast<int>(new_state.direction)] >
        new_state.cost) {
      res[x][y].direction[static_cast<int>(new_state.direction)] =
          new_state.cost;
      pq.push(std::move(new_state));
    }
  };

  res[start.first][start.second].direction[static_cast<int>(Direction::East)] =
      0;
  pq.push(PQState{.cost = 0, .pos = start, .direction = Direction::East});

  // too lazy to prune, it's monday
  while (!pq.empty()) {
    auto state = pq.top();
    pq.pop();

    if (res[state.pos.first][state.pos.second]
            .direction[static_cast<int>(state.direction)] < state.cost)
      continue;

    {
      Direction d = cw(state.direction);
      int cost = COST_TURN;
      while (d != state.direction) {
        const int d_idx = static_cast<int>(state.direction);
        try_update(PQState{
            .cost = state.cost + cost, .pos = state.pos, .direction = d});
        d = cw(d);
        cost += COST_TURN;
      }
    }
    {
      Direction d = ccw(state.direction);
      int cost = COST_TURN;
      while (d != state.direction) {
        try_update(PQState{
            .cost = state.cost + cost, .pos = state.pos, .direction = d});
        d = ccw(d);
        cost += COST_TURN;
      }
    }
    {
      const int d_idx = static_cast<int>(state.direction);
      const pii next_pos = std::make_pair(state.pos.first + dr[d_idx],
                                          state.pos.second + dc[d_idx]);
      if (!valid_spot(rows, cols, next_pos.first, next_pos.second) ||
          grid[next_pos.first][next_pos.second] == WALL)
        continue;

      const int next_cost = state.cost + COST_MOVE;
      try_update(PQState{
          .cost = next_cost, .pos = next_pos, .direction = state.direction});
    }
  }
  return res;
}

void annotate_best_path(Vector2D<char>& grid, const Vector2D<Cost> cost,
                        pii end) {
  const int rows = grid.height(), cols = grid.width();
  const int* min_cost =
      std::min_element(cost[end.first][end.second].direction,
                       cost[end.first][end.second].direction + 4);

  std::queue<PQState> bfs_q;
  auto try_update = [&](PQState new_state) {
    const auto [x, y] = new_state.pos;
    if (cost[x][y].direction[static_cast<int>(new_state.direction)] ==
        new_state.cost) {
      bfs_q.push(std::move(new_state));
    }
  };

  for (int i = 0; i < 4; ++i) {
    Direction dir = static_cast<Direction>(i);
    try_update(PQState{.cost = *min_cost, .pos = end, .direction = dir});
  }

  while (!bfs_q.empty()) {
    auto state = bfs_q.front();
    bfs_q.pop();
    grid[state.pos.first][state.pos.second] = BEST_PATH;
    {
      Direction d = cw(state.direction);
      int cost = COST_TURN;
      while (d != state.direction) {
        const int d_idx = static_cast<int>(state.direction);
        try_update(PQState{
            .cost = state.cost - cost, .pos = state.pos, .direction = d});
        d = cw(d);
        cost += COST_TURN;
      }
    }
    {
      Direction d = ccw(state.direction);
      int cost = COST_TURN;
      while (d != state.direction) {
        try_update(PQState{
            .cost = state.cost - cost, .pos = state.pos, .direction = d});
        d = ccw(d);
        cost += COST_TURN;
      }
    }
    {
      const int d_idx = static_cast<int>(state.direction);
      const pii next_pos = std::make_pair(state.pos.first - dr[d_idx],
                                          state.pos.second - dc[d_idx]);
      if (!valid_spot(rows, cols, next_pos.first, next_pos.second) ||
          grid[next_pos.first][next_pos.second] == WALL)
        continue;

      const int next_cost = state.cost - COST_MOVE;
      try_update(PQState{
          .cost = next_cost, .pos = next_pos, .direction = state.direction});
    }
  }
}

int solve(Vector2D<char>& grid) {
  const int rows = grid.height(), cols = grid.width();
  pii start = [&]() -> auto {
    for (int i = 0; i < rows; ++i) {
      for (int j = 0; j < cols; ++j) {
        if (grid[i][j] == START)
          return std::make_pair(i, j);
      }
    }
    assert(false);
    __builtin_unreachable();
  }();
  pii end = [&]() -> auto {
    for (int i = 0; i < rows; ++i) {
      for (int j = 0; j < cols; ++j) {
        if (grid[i][j] == END)
          return std::make_pair(i, j);
      }
    }
    assert(false);
    __builtin_unreachable();
  }();

  auto res = exec_sssp(grid, start);
  annotate_best_path(grid, res, end);
  print(grid);
  int best_paths = 0;
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      if (grid[i][j] == BEST_PATH)
        ++best_paths;
    }
  }
  return best_paths;
}

} // namespace

int main() {
  auto grid = read_input();
  std::cout << solve(grid) << '\n';
  return 0;
}
