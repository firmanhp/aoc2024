#include <algorithm>
#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <ostream>
#include <span>
#include <sstream>
#include <thread>
#include <unordered_set>

#include "input.h"

namespace {

using util::print;
using util::valid_spot;
using util::Vector2D;
typedef long long ll;

#ifdef TEST
constexpr std::string INPUT = "p1_input.txt";
#endif
#ifdef SAMPLE
constexpr std::string INPUT = "p2_sample.txt";
#endif
constexpr ll MOD = 16777216;
constexpr int ITERS = 2000;

ll next(ll secret) {
  secret = secret ^ (secret << 6);
  secret %= MOD;
  secret = secret ^ (secret >> 5);
  secret %= MOD;
  secret = secret ^ (secret << 11);
  secret %= MOD;
  return secret;
}

struct Diffseq {
  int s[4];

  inline bool operator==(const Diffseq& o) const {
    return s[0] == o.s[0] && s[1] == o.s[1] && s[2] == o.s[2] && s[3] == o.s[3];
  }
};

inline std::ostream& operator<<(std::ostream& o, const Diffseq& d) {
  for (int i = 0; i < 4; ++i) {
    o << d.s[i] << ",";
  }
  return o;
}

} // namespace

template <> struct std::hash<Diffseq> {
  std::size_t operator()(const Diffseq& p) const noexcept {
    std::size_t hash = 0;
    for (int i = 0; i < 4; ++i) {
      hash <<= 6;
      hash |= p.s[i] + 10;
    }
    return hash;
  }
};

namespace {

template <typename T, int N> void shift(T (&data)[N]) {
  for (int i = 0; i < N - 1; ++i) {
    data[i] = data[i + 1];
  }
}

std::unordered_map<Diffseq, ll> profits;
void gen_seq(ll start_val) {
  std::unordered_set<Diffseq> diff_seq_visit;
  ll val_seq[4] = {start_val};
  Diffseq diff_seq;

  ll secret = start_val;
  // val_seq[3] = secret;
  // diff_seq.s[3] = (val_seq[3] % 10) - (start_val % 10);

  for (int i = 0; i < ITERS; ++i) {
    secret = next(secret);
    shift(val_seq);
    val_seq[3] = secret;
    shift(diff_seq.s);
    diff_seq.s[3] = (val_seq[3] % 10) - (val_seq[2] % 10);

    if (i >= 3) {
      if (diff_seq_visit.contains(diff_seq))
        continue;
      diff_seq_visit.insert(diff_seq);
      profits[diff_seq] += val_seq[3] % 10;
    }
  }
}

ll solve(const std::vector<ll>& input) {
  for (ll val : input) {
    gen_seq(val);
  }

  ll ans = 0;
  for (const auto& [diff_seq, profit] : profits) {
    // std::cout << "Sequence " << diff_seq << " --> " << profit << '\n';
    ans = std::max(ans, profit);
  }
  return ans;
}

} // namespace

int main() {
  auto input = input::read_input(INPUT);
  std::cout << solve(input) << '\n';
  return 0;
}
