#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include <span>
#include <sstream>
#include <unordered_set>

namespace {

using PrecedenceMap = std::map<int, std::unordered_set<int>>;
using PageSequence = std::vector<int>;

struct Input {
  PrecedenceMap precedence;
  std::vector<PageSequence> sequences;
};

Input read_input() {
  std::fstream file;
  file.open("p1_input.txt", std::ios_base::in);
  assert(file.is_open());

  PrecedenceMap precedence;
  std::vector<PageSequence> sequences;

  std::string s;
  while (std::getline(file, s) && !s.empty()) {
    std::replace(s.begin(), s.end(), '|', ' ');
    int from, to;
    std::stringstream(std::move(s)) >> from >> to;
    precedence[from].insert(to);
  }

  while (getline(file, s)) {
    PageSequence sequence;
    std::replace(s.begin(), s.end(), ',', ' ');
    std::stringstream ss(std::move(s));

    int page;
    while (ss >> page) {
      sequence.push_back(page);
    }
    sequences.push_back(std::move(sequence));
  }

  return Input{.precedence = std::move(precedence),
               .sequences = std::move(sequences)};
}

int solve_single_sequence(const PrecedenceMap& precedence,
                          const PageSequence& sequence) {
  std::unordered_map<int, int> index_lookup;
  const int N = sequence.size();
  for (int i = 0; i < N; ++i) {
    const int page = sequence[i];
    assert(!index_lookup.contains(page)); // no duplicates
    index_lookup[page] = i;
  }

  for (int page : sequence) {
    auto precedence_it = precedence.find(page);
    if (precedence_it == precedence.end())
      continue;

    const int page_idx = index_lookup[page];
    for (int next : precedence_it->second) {
      auto it = index_lookup.find(next);
      if (it == index_lookup.end())
        continue;

      if (page_idx > it->second) {
        return 0;
      }
    }
  }

  return sequence[N / 2];
}

int solve(Input& input) {
  std::vector<int> invalid_seq_idx;
  for (int i = 0; i < input.sequences.size(); ++i) {
    const PageSequence& sequence = input.sequences[i];
    if (!solve_single_sequence(input.precedence, sequence)) {
      invalid_seq_idx.push_back(i);
    }
  }

  long long ans = 0;
  for (int idx : invalid_seq_idx) {
    PageSequence& sequence = input.sequences[idx];
    std::sort(sequence.begin(), sequence.end(),
              [&input](int p1, int p2) -> bool {
                const PrecedenceMap& precedence = input.precedence;
                auto it = precedence.find(p1);
                if (it == precedence.end()) {
                  return false;
                }

                return it->second.contains(p2);
              });

    ans += sequence[sequence.size() / 2];
  }

  return ans;
}

} // namespace

int main() {
  Input input = read_input();
  std::cout << solve(input) << '\n';
  return 0;
}
