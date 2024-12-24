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
  Graph(const input::Input& input) {
    fill_indices(input);
    construct_nodes(input);
  }

  int size() const { return nodes_.size(); }

  void evaluate() {
    std::queue<int> bfs_q;
    for (int i = 0; i < size(); ++i) {
      if (nodes_[i].evaluated())
        bfs_q.push(i);
    }

    while (!bfs_q.empty()) {
      const int u = bfs_q.front();
      bfs_q.pop();
      for (int v : affect_list_[u]) {
        if (!nodes_[v].evaluated() && dependencies_evaluated(nodes_[v])) {
          evaluate(nodes_[v]);
          bfs_q.push(v);
        }
      }
    }
  }

  std::vector<std::pair<std::string, bool>> pull_values() const {
    std::vector<std::pair<std::string, bool>> res;
    res.reserve(size());
    for (int i = 0; i < size(); ++i) {
      res.push_back(std::make_pair(reverse_idx_.at(i), nodes_[i].val.value()));
    }
    return res;
  }

private:
  enum class Op { AND, OR, XOR };

  struct Node {
    int dependency[2] = {-1, -1};
    Op op;
    std::optional<bool> val;

    bool evaluated() const { return val.has_value(); }
  };

  void fill_indices(const input::Input& input) {
    for (const auto& [name, unused] : input.initial) {
      idx_[name] = 0;
    }
    for (const input::Input::Gate& gate : input.gates) {
      idx_[gate.operand_1] = 0;
      idx_[gate.operand_2] = 0;
      idx_[gate.operand_result] = 0;
    }

    int cnt = 0;
    for (auto& [name, idx] : idx_) {
      idx = cnt++;
      reverse_idx_[idx] = name;
    }
  }

  void construct_nodes(const input::Input& input) {
    const int N = reverse_idx_.size();
    nodes_.resize(N);
    affect_list_.resize(N);
    for (const auto& [name, val] : input.initial) {
      const int idx = idx_[name];
      nodes_[idx].val = val;
    }

    for (const input::Input::Gate& gate : input.gates) {
      const int op1_idx = idx_[gate.operand_1], op2_idx = idx_[gate.operand_2],
                result_idx = idx_[gate.operand_result];
      Op op;
      if (gate.op == "AND")
        op = Op::AND;
      else if (gate.op == "OR")
        op = Op::OR;
      else if (gate.op == "XOR")
        op = Op::XOR;
      else
        assert("Unknown op" && false);
      nodes_[result_idx].dependency[0] = op1_idx;
      nodes_[result_idx].dependency[1] = op2_idx;
      nodes_[result_idx].op = op;

      affect_list_[op1_idx].push_back(result_idx);
      affect_list_[op2_idx].push_back(result_idx);
    }
  }

  bool dependencies_evaluated(const Node& node) const {
    return nodes_[node.dependency[0]].evaluated() &&
           nodes_[node.dependency[1]].evaluated();
  }

  void evaluate(Node& node) {
    assert(node.dependency[0] >= 0);
    assert(node.dependency[1] >= 0);
    const bool dep1 = nodes_[node.dependency[0]].val.value();
    const bool dep2 = nodes_[node.dependency[1]].val.value();
    switch (node.op) {
    case Op::AND:
      node.val = dep1 & dep2;
      return;
    case Op::OR:
      node.val = dep1 | dep2;
      return;
    case Op::XOR:
      node.val = dep1 ^ dep2;
      return;
    }
  }

  std::unordered_map<std::string, int> idx_;
  std::unordered_map<int, std::string> reverse_idx_;
  std::vector<std::vector<int>> affect_list_;
  std::vector<Node> nodes_;
};

ll solve(const input::Input& input) {
  Graph g(input);
  g.evaluate();
  std::vector<std::pair<std::string, bool>> res = g.pull_values();
  sort(res.begin(), res.end(), std::greater<std::pair<std::string, bool>>());

  ll ans = 0;
  for (const auto& [name, val] : res) {
    std::cout << name << " : " << val << '\n';
    if (name.front() == 'z')
      ans = (ans * 2) + static_cast<int>(val);
  }

  return ans;
}

} // namespace

int main() {
  auto input = input::read_input(INPUT);
  std::cout << solve(input) << '\n';
  return 0;
}
