#include <algorithm>
#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <span>
#include <sstream>
#include <thread>
#include <tuple>
#include <unordered_set>

#include "input.h"

namespace {

using util::print;
using util::valid_spot;
using util::Vector2D;
typedef long long ll;
typedef std::pair<int, int> pii;

#ifdef TEST
constexpr std::string INPUT = "p1_input.txt";
#endif
#ifdef SAMPLE
constexpr std::string INPUT = "p1_sample.txt";
#endif

class Graph {
public:
  Graph(std::span<const input::Edge> edges) {
    for (const auto edge : edges) {
      idx_[edge.u] = 0;
      idx_[edge.v] = 0;
    }
    fill_indices();
    nodes_ = idx_.size();
    adj_list_.resize(nodes_);
    edge_list_.reserve(edges.size());
    for (const auto edge : edges) {
      int u = idx_[edge.u], v = idx_[edge.v];
      adj_list_[u].push_back(v);
      adj_list_[v].push_back(u);
      edge_list_.push_back(std::make_pair(u, v));
    }
  }

  std::vector<std::vector<std::string>> get_triangles() const {
    std::unordered_set<Triple, Triple::Hash> triangles;
    for (auto [u, v] : edge_list_) {
      std::unordered_set<int> neighbors;
      for (int w : adj_list_[u]) {
        neighbors.insert(w);
      }
      for (int w : adj_list_[v]) {
        if (neighbors.contains(w)) {
          // we found a triangle
          triangles.insert(Triple(u, v, w));
        }
      }
    }

    std::vector<std::vector<std::string>> res;
    res.reserve(triangles.size());
    for (const Triple& t : triangles) {
      res.push_back(std::vector<std::string>(
          {reverse_idx_.at(t.i), reverse_idx_.at(t.j), reverse_idx_.at(t.k)}));
    }
    return res;
  }

private:
  struct Triple {
    int i, j, k;
    Triple(int a, int b, int c) : i(a), j(b), k(c) {
      // babba sort
      if (i > j)
        std::swap(i, j);
      if (j > k)
        std::swap(j, k);
      if (i > j)
        std::swap(i, j);
    }

    bool operator==(const Triple& o) const {
      return i == o.i && j == o.j && k == o.k;
    }

    struct Hash {
      size_t operator()(const Triple& t) const {
        size_t res = 0;
        res = (res << 20) | t.i;
        res = (res << 20) | t.j;
        res = (res << 20) | t.k;
        return res;
      }
    };
  };

  void fill_indices() {
    int idx = 0;
    for (auto& [name, id] : idx_) {
      id = idx++;
      reverse_idx_[id] = name;
    }
  }

  int nodes_;
  std::unordered_map<std::string, int> idx_;
  std::unordered_map<int, std::string> reverse_idx_;
  std::vector<std::vector<int>> adj_list_;
  std::vector<pii> edge_list_;
};

int solve(const std::vector<input::Edge>& input) {
  Graph g(input);
  std::vector<std::vector<std::string>> triangles = g.get_triangles();
  int count = 0;
  for (const auto& triangle : triangles) {
    for (const auto& node : triangle) {
      if (node.front() == 't') {
        count++;
        break;
      }
    }
  }
  return count;
}

} // namespace

int main() {
  auto input = input::read_input(INPUT);
  std::cout << solve(input) << '\n';
  return 0;
}
