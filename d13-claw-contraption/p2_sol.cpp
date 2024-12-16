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

struct TestCase {
  pii button_a;
  pii button_b;
  pii target;
};

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

constexpr ll COST_A = 3;
constexpr ll COST_B = 1;
constexpr ll TARGET_EXTRA = 1e13;

Vector2D<ll> adjoint(const Vector2D<ll> mat) {
  const int w = mat.width(), h = mat.height();
  assert(w == h);
  assert(w == 2);

  Vector2D<ll> res(w, h);
  res[0][0] = mat[1][1];
  res[0][1] = -mat[0][1];
  res[1][0] = -mat[1][0];
  res[1][1] = mat[0][0];
  return res;
}

std::vector<ll> mul(const Vector2D<ll>& mat, std::vector<ll> vec) {
  const int w = mat.width(), h = mat.height();
  assert(w == h);
  assert(w == 2);

  assert(vec.size() == 2);
  std::vector<ll> res(2, 0);
  res[0] = mat[0][0] * vec[0] + mat[0][1] * vec[1];
  res[1] = mat[1][0] * vec[0] + mat[1][1] * vec[1];
  return res;
}

ll inv_det(const Vector2D<ll>& mat) {
  const int w = mat.width(), h = mat.height();
  assert(w == h);
  assert(w == 2);
  return mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
}

std::vector<TestCase> read_input() {
  std::fstream file;
  file.open("p1_input_parsed.txt", std::ios_base::in);
  assert(file.is_open());

  std::vector<TestCase> res;

  std::string s;
  while (std::getline(file, s)) {
    TestCase tc;
    std::istringstream iss(std::move(s));
    iss >> tc.button_a.first >> tc.button_a.second >> tc.button_b.first >>
        tc.button_b.second >> tc.target.first >> tc.target.second;
    res.push_back(tc);
  }

  return res;
}

ll find_tokens(const TestCase& testcase) {
  Vector2D<ll> mat(2, 2);
  mat[0][0] = testcase.button_a.first;
  mat[0][1] = testcase.button_b.first;
  mat[1][0] = testcase.button_a.second;
  mat[1][1] = testcase.button_b.second;

  std::vector<ll> res(2);
  res[0] = testcase.target.first + TARGET_EXTRA;
  res[1] = testcase.target.second + TARGET_EXTRA;

  ll det = inv_det(mat);
  std::vector<ll> adj_res = mul(adjoint(mat), res);

  if (adj_res[0] % det != 0 || adj_res[1] % det != 0) {
    // std::cout << "non integer result\n";
    return 0;
  }

  // if ((adj_res[0] / det > 100) || (adj_res[1] / det > 100)) {
  //   std::cout << "over 100\n";
  //   return 0;
  // }
  const ll token_a = COST_A * (adj_res[0] / det);
  const ll token_b = (adj_res[1] / det);
  // std::cout << token_a + token_b << '\n';
  return token_a + token_b;
}

ll solve(std::vector<TestCase> input) {

  long long sum = 0;
  for (const auto& testcase : input) {
    sum += find_tokens(testcase);
    // std::cout << '\n';
  }
  return sum;
}

} // namespace

int main() {
  std::cout << solve(read_input()) << '\n';
  return 0;
}
