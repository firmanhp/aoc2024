#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <list>
#include <span>
#include <sstream>

namespace {

using Stone = long long;
struct StoneState {
  Stone stone;
  int blink_count;

  bool operator==(const StoneState& other) const noexcept {
    return stone == other.stone && blink_count == other.blink_count;
  }
};

} // namespace

template <> struct std::hash<StoneState> {
  std::size_t operator()(const StoneState& p) const noexcept {
    auto h1 = std::hash<Stone>{}(p.stone);
    auto h2 = std::hash<int>{}(p.blink_count);
    return h1 ^ h2;
  }
};

namespace {

// LRU cache
template <typename K, typename V, size_t N> class Cache {
public:
  Cache() {};
  std::optional<V> get(const K& key) {
    auto it = container_.find(key);
    if (it == container_.end()) {
      return std::nullopt;
    }

    renew(it->second);
    return it->second.val;
  }

  void set(K key, V val) {
    auto it = container_.find(key);
    if (it != container_.end()) {
      it->second.val = val;
      renew(it->second);
      return;
    }

    if (iterator_list_.size() == N) {
      evict();
    }
    iterator_list_.push_back(key);
    container_.insert(std::make_pair(
        key, Entry{.val = val, .lru_it = std::prev(iterator_list_.end())}));
  }

  int used_size() const { return container_.size(); }

private:
  using LruList = std::list<K>;
  struct Entry {
    V val;
    LruList::iterator lru_it;
  };

  void renew(const Entry& entry) {
    iterator_list_.splice(iterator_list_.end(), iterator_list_, entry.lru_it,
                          std::next(entry.lru_it));
  }

  void evict() {
    if (iterator_list_.empty()) {
      return;
    }

    auto it = iterator_list_.begin();
    auto container_it = container_.find(*it);
    assert(container_it != container_.end());
    container_.erase(container_it);
    iterator_list_.erase(it);
  }

  std::unordered_map<K, Entry> container_;
  LruList iterator_list_;
};

int digit_count(Stone stone) {
  int count = 0;
  while (stone) {
    ++count;
    stone /= 10;
  }
  return count;
}

std::optional<std::pair<Stone, Stone>> try_split(Stone stone) {
  const int count = digit_count(stone);
  assert(count > 0);
  if (count % 2 != 0) {
    return std::nullopt;
  }

  const long long divider = std::pow(10, count / 2);
  return std::make_pair(stone / divider, stone % divider);
}

constexpr size_t CACHE_SIZE = 1e6;
Cache<StoneState, Stone, CACHE_SIZE> cache;
Stone blink_res(StoneState state) {
  if (state.blink_count == 0) {
    return 1;
  }

  if (auto c = cache.get(state); c.has_value()) {
    return *c;
  }

  Stone res = [state]() {
    if (state.stone == 0) {
      return blink_res(
          StoneState{.stone = 1, .blink_count = state.blink_count - 1});
    }
    if (auto split = try_split(state.stone); split.has_value()) {
      return blink_res(StoneState{.stone = split->first,
                                  .blink_count = state.blink_count - 1}) +
             blink_res(StoneState{.stone = split->second,
                                  .blink_count = state.blink_count - 1});
    }
    return blink_res(StoneState{.stone = state.stone * 2024,
                                .blink_count = state.blink_count - 1});
  }();
  cache.set(state, res);
  return res;
}

std::vector<Stone> read_input() {
  std::fstream file;
  file.open("p1_input.txt", std::ios_base::in);
  assert(file.is_open());

  std::vector<Stone> res;

  std::string s;
  while (std::getline(file, s)) {
    std::istringstream iss(std::move(s));
    Stone level;
    while (iss >> level) {
      res.push_back(level);
    }
  }

  return res;
}

Stone solve(const std::vector<Stone>& input) {
  constexpr int BLINK_COUNT = 75;
  long long res = 0;
  for (const Stone& stone : input) {
    res += blink_res(StoneState{.stone = stone, .blink_count = BLINK_COUNT});
  }

  std::cout << "Used cache size = " << cache.used_size() << '\n';
  return res;
}

} // namespace

int main() {
  std::cout << solve(read_input()) << '\n';
  return 0;
}
