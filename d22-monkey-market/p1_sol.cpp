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

namespace {

using util::print;
using util::valid_spot;
using util::Vector2D;
typedef long long ll;

#ifdef TEST
constexpr std::string INPUT = "p1_input.txt";
#endif
#ifdef SAMPLE
constexpr std::string INPUT = "p1_sample.txt";
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

ll solve(const std::vector<ll>& input) {
  ll sum = 0;
  for (ll val : input) {
    ll secret = val;
    for (int i = 0; i < ITERS; ++i) {
      secret = next(secret);
    }

    std::cout << val << ": " << secret << '\n';
    sum += secret;
  }

  return sum;
}

} // namespace

int main() {
  auto input = input::read_input(INPUT);
  std::cout << solve(input) << '\n';
  return 0;
}
