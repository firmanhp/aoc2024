#include <algorithm>
#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <span>
#include <sstream>
#include <thread>
#include <tuple>
#include <unordered_set>

#include "input.h"
#include "util/thread_pool.h"

namespace {

using util::print;
using util::ThreadPool;
using util::valid_spot;
using util::Vector2D;
typedef long long ll;
typedef std::pair<int, int> pii;

#ifdef TEST
constexpr std::string INPUT = "p1_input.txt";
constexpr int N = 520;
#endif
#ifdef SAMPLE
constexpr std::string INPUT = "p1_sample.txt";
constexpr int N = 16;
#endif

std::ostream& operator<<(std::ostream& o, const std::vector<std::string>& v) {
  for (int i = 0; i < (int)v.size(); ++i) {
    if (i > 0)
      o << ',';
    o << v[i];
  }
  return o;
}

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
    adj_bitset_.resize(nodes_);
    edge_list_.reserve(edges.size());
    for (const auto edge : edges) {
      int u = idx_[edge.u], v = idx_[edge.v];
      adj_list_[u].push_back(v);
      adj_list_[v].push_back(u);
      edge_list_.push_back(std::make_pair(u, v));
      adj_bitset_[u].set(v);
      adj_bitset_[v].set(u);
    }
  }

  std::vector<std::vector<std::string>> get_triangles() const {
    std::vector<Triple> triangles = get_triangles_impl();

    std::vector<std::vector<std::string>> res;
    res.reserve(triangles.size());
    for (const Triple& t : triangles) {
      res.push_back(std::vector<std::string>(
          {reverse_idx_.at(t.i), reverse_idx_.at(t.j), reverse_idx_.at(t.k)}));
    }
    return res;
  }

  int nodes() const { return nodes_; }

  std::vector<std::string> get_max_clique() const {
    std::bitset<N> max_clique;
    std::mutex clique_mtx;

    std::vector<Triple> triangles = get_triangles_impl();
    {
      std::random_device rd;
      std::mt19937 g(rd());
      std::shuffle(triangles.begin(), triangles.end(), g);
    }
    std::cerr << "DEBUG: triangles: " << triangles.size() << '\n';
    ThreadPool pool(16);
    for (const Triple& triangle : triangles) {
      pool.add_job([this, &clique_mtx, &max_clique, triangle]() {
        std::cerr << "DEBUG: New thread spawned: Executing\n";
        const auto t1 = std::chrono::steady_clock::now();
        std::bitset<N> clique = max_clique_from_triangle(triangle);
        const auto t2 = std::chrono::steady_clock::now();
        const auto dur =
            std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

        std::lock_guard lock(clique_mtx);
        std::cerr << "DEBUG: Thread work done: " << dur << '\n';
        if (clique.count() > max_clique.count()) {
          max_clique = clique;
          auto clique_names = to_names(clique);
          std::cout << "NEW MAX CLIQUE: " << clique_names << '\n';
        }
      });
    }

    pool.wait_all();
    std::cerr << "Finished all clique searches.\n";
    std::cerr << "Found max clique of size " << max_clique.count() << '\n';
    return to_names(max_clique);
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

  std::vector<Triple> get_triangles_impl() const {
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

    std::vector<Triple> res;
    res.reserve(triangles.size());
    for (const auto t : triangles) {
      res.push_back(t);
    }
    return res;
  }

  std::bitset<N> max_clique_from_triangle(const Triple& triangle) const {
    std::unordered_set<std::bitset<N>> cliques;
    {
      std::bitset<N> t;
      t.set(triangle.i);
      t.set(triangle.j);
      t.set(triangle.k);
      cliques.insert(std::move(t));
    }
    bool rerun = true;
    while (rerun) {
      rerun = false;
      for (auto it = cliques.begin(); it != cliques.end(); ++it) {
        std::vector<std::bitset<N>> new_cliques;
        const auto& clique = *it;
        for (int v = 0; v < N; ++v) {
          if (clique.test(v))
            continue;
          if ((clique & adj_bitset_[v]) == clique) {
            new_cliques.emplace_back(clique);
            new_cliques.back().set(v);
          }
        }
        if (new_cliques.size() > 0) {
          rerun = true;
          cliques.erase(it);
          for (const auto& c : new_cliques) {
            cliques.insert(c);
          }
          break;
        }
      }
    }

    std::bitset<N> max_clique;
    for (const auto& clique : cliques) {
      if (clique.count() > max_clique.count())
        max_clique = clique;
    }
    return max_clique;
  }

  std::vector<std::string> to_names(const std::bitset<N>& clique) const {
    std::vector<std::string> res;
    res.reserve(clique.count());
    for (int i = 0; i < N; ++i) {
      if (!clique.test(i))
        continue;

      res.push_back(reverse_idx_.at(i));
    }
    std::sort(res.begin(), res.end());
    return res;
  }

  int nodes_;
  std::unordered_map<std::string, int> idx_;
  std::unordered_map<int, std::string> reverse_idx_;
  std::vector<std::vector<int>> adj_list_;
  std::vector<std::bitset<N>> adj_bitset_;
  std::vector<pii> edge_list_;
};

int solve(const std::vector<input::Edge>& input) {
  Graph g(input);
  std::cout << "Nodes: " << g.nodes() << '\n';
  assert(N == g.nodes()); // assertion from testcase

  std::vector<std::string> max_clique = g.get_max_clique();
  std::cout << max_clique << '\n';

  return 0;
}

} // namespace

int main() {
  auto input = input::read_input(INPUT);
  std::cout << solve(input) << '\n';
  return 0;
}
