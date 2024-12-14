#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <span>
#include <sstream>
#include <thread>

#include "util.h"
// Requires imagemagick
#define cimg_display 0
#include "CImg.h"

namespace {

using pii = std::pair<int, int>;
using ll = long long;

struct Robot {
  pii starting;
  pii velocity;
};

using util::Vector2D;

constexpr int WIDTH = 101;
constexpr int HEIGHT = 103;
constexpr int SECONDS = 100;
constexpr std::string OUT_DIR = "draws/";

std::vector<Robot> read_input() {
  std::fstream file;
  file.open("p1_input_parsed.txt", std::ios_base::in);
  assert(file.is_open());

  std::vector<Robot> res;

  std::string s;
  while (std::getline(file, s)) {
    Robot robot;
    std::istringstream iss(std::move(s));
    iss >> robot.starting.first >> robot.starting.second  //
        >> robot.velocity.first >> robot.velocity.second; //
    res.push_back(std::move(robot));
  }

  return res;
}

// fix 1 dimension position
int fix_up_dim(int pos, int dim_len) {
  int final_pos = pos % dim_len;
  if (final_pos < 0)
    final_pos += dim_len;
  return final_pos;
}

pii final_pos(const Robot& robot, pii grid_size, int seconds) {
  pii res =
      std::make_pair(robot.starting.first + (robot.velocity.first * seconds),
                     robot.starting.second + (robot.velocity.second * seconds));
  res.first = fix_up_dim(res.first, grid_size.first);
  res.second = fix_up_dim(res.second, grid_size.second);
  return res;
}

Vector2D<char> to_grid(const std::vector<pii>& pos) {
  Vector2D<char> res(HEIGHT, WIDTH);
  for (int i = 0; i < HEIGHT; ++i) {
    for (int j = 0; j < WIDTH; ++j) {
      res[i][j] = '.';
    }
  }

  for (const auto& [col, row] : pos) {
    res[row][col] = '*';
  }

  return res;
}

void print(const Vector2D<char>& grid) {
  using namespace std::chrono_literals;
  const int rows = grid.height(), cols = grid.width();
  std::this_thread::sleep_for(50ms);
  std::cout << "\033[2J\033[1;1H";
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      std::cout << grid[i][j];
    }
    std::cout << '\n';
  }
}

void draw_to_img(const Vector2D<char> grid, int idx) {
  using namespace cimg_library;
  const int height = grid.height(), width = grid.width();
  CImg<uint8_t> image(width, height, 1, 1, 0);
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      image(x, y) = (grid[y][x] == '*') * 255;
    }
  }

  std::string output = OUT_DIR + std::to_string(idx) + ".png";
  image.save_png(output.c_str());
}

int solve(const std::vector<Robot>& input) {
  const pii grid_size = std::make_pair(WIDTH, HEIGHT);
  int seconds = 1;

  auto is_same = [&](const std::vector<pii>& poss) -> bool {
    assert(poss.size() == input.size());
    const int N = poss.size();
    for (int i = 0; i < N; ++i) {
      if (poss[i] != input[i].starting) {
        return false;
      }
    }
    return true;
  };

  while (true) {
    std::vector<pii> poss;
    poss.reserve(input.size());
    for (const auto& robot : input) {
      poss.push_back(final_pos(robot, grid_size, seconds));
    }

    if (is_same(poss)) {
      break;
    }

    auto grid = to_grid(poss);
    draw_to_img(grid, seconds);

    if (seconds % 5000 == 0) {
      std::cout << seconds << "s passed.\n";
    }
    ++seconds;
  }

  std::cout << "Loop back in " << seconds << "s\n";
  return 0;
}

} // namespace

int main() {
  auto input = read_input();
  std::cout << solve(input) << '\n';
  return 0;
}
