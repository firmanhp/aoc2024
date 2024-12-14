#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <span>
#include <sstream>

namespace {

using pii = std::pair<int, int>;
using ll = long long;

struct Robot {
  pii starting;
  pii velocity;
};

constexpr int WIDTH = 101;
constexpr int HEIGHT = 103;
constexpr int SECONDS = 100;

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

int solve(std::vector<Robot> input) {
  pii grid_size = std::make_pair(WIDTH, HEIGHT);
  const int qdivide_x = WIDTH / 2;
  const int qdivide_y = HEIGHT / 2;
  long long quadrants[2][2];
  memset(quadrants, 0, sizeof(quadrants));

  for (const auto& robot : input) {
    pii pos = final_pos(robot, grid_size, SECONDS);
    if (pos.first == qdivide_x || pos.second == qdivide_y)
      continue;

    ++quadrants[pos.first > qdivide_x][pos.second > qdivide_y];
  }

  const long long sum =
      quadrants[0][0] * quadrants[0][1] * quadrants[1][0] * quadrants[1][1];
  return sum;
}

void test_positions() {
  Robot robot = Robot{.starting = std::make_pair(2, 4),
                      .velocity = std::make_pair(2, -3)};
  pii grid_size = std::make_pair(11, 7);
  std::cout << "Running sample test.\n";
  {
    pii result = final_pos(robot, grid_size, 0);
    std::cout << result.first << ", " << result.second << '\n';
    assert(result.first == 2);
    assert(result.second == 4);
  }
  {
    pii result = final_pos(robot, grid_size, 1);
    std::cout << result.first << ", " << result.second << '\n';
    assert(result.first == 4);
    assert(result.second == 1);
  }
  {
    pii result = final_pos(robot, grid_size, 2);
    std::cout << result.first << ", " << result.second << '\n';
    assert(result.first == 6);
    assert(result.second == 5);
  }
  {
    pii result = final_pos(robot, grid_size, 3);
    std::cout << result.first << ", " << result.second << '\n';
    assert(result.first == 8);
    assert(result.second == 2);
  }
  {
    pii result = final_pos(robot, grid_size, 4);
    std::cout << result.first << ", " << result.second << '\n';
    assert(result.first == 10);
    assert(result.second == 6);
  }
  {
    pii result = final_pos(robot, grid_size, 5);
    std::cout << result.first << ", " << result.second << '\n';
    assert(result.first == 1);
    assert(result.second == 3);
  }
  std::cout << "Test OK.\n";
}

} // namespace

int main() {
  test_positions();
  std::cout << solve(read_input()) << '\n';
  return 0;
}
