#include <algorithm>
#include <cassert>
#include <chrono>
#include <deque>
#include <fstream>
#include <iostream>
#include <list>
#include <span>
#include <sstream>
#include <thread>

namespace {

struct Block {
  static constexpr int FREE = -1;
  int id;
  int size;
};

class ChecksumProcessor {
public:
  void feed(const Block& block) {
    if (block.id == Block::FREE) {
      cur_id_ += block.size;
      return;
    }
    // can be optimized
    for (int i = 0; i < block.size; ++i, ++cur_id_) {
      sum_ += cur_id_ * block.id;
    }
  }
  long long sum() const { return sum_; }

private:
  long long sum_ = 0;
  long long cur_id_ = 0;
};

std::vector<Block> read_input() {
  std::fstream file;
  file.open("p1_input.txt", std::ios_base::in);
  assert(file.is_open());

  std::vector<Block> res;
  bool is_free = false;
  int id = 0;
  // this needs to be limited by size
  std::string s;
  while (getline(file, s)) {
    for (char c : s) {
      int num = c - '0';
      if (!is_free) {
        res.push_back(Block{.id = id++, .size = num});
      } else {
        res.push_back(Block{.id = Block::FREE, .size = num});
      }
      is_free = !is_free; // alternatingly
    }
  }

  return res;
}

void print(const std::list<Block> blocklist) {
  for (const auto& b : blocklist) {
    for (int i = 0; i < b.size; ++i) {
      if (b.id == Block::FREE)
        std::cout << '.';
      else
        std::cout << b.id;
    }
  }
  std::cout << '\n';
}

long long solve(std::vector<Block> grid) {
  ChecksumProcessor checksum;
  std::list<Block> blocklist;
  for (Block block : grid) {
    blocklist.push_back(std::move(block));
  }

  // Simple o(n^2)
  for (auto r_it = blocklist.rbegin(); r_it != blocklist.rend(); ++r_it) {
    // print(blocklist);
    if (r_it->id == Block::FREE)
      continue;

    // std::cout << "handle " << r_it->id << '\n';
    // find any empty blocks that can fit *r_it;
    auto to_put =
        std::find_if(blocklist.begin(), std::next(r_it).base(),
                     [size = r_it->size](const auto& block) {
                       return block.id == Block::FREE && block.size >= size;
                     });

    if (to_put == std::next(r_it).base())
      continue;
    // std::cout << "spot " << to_put->size << '\n';

    if (to_put->size == r_it->size) {
      to_put->id = r_it->id;
      auto f_it = std::next(r_it).base();
      blocklist.insert(f_it, Block{.id = Block::FREE, .size = r_it->size});
      r_it = decltype(r_it)(blocklist.erase(f_it, std::next(f_it)));
    } else {
      to_put->size -= r_it->size;
      auto f_it = std::next(r_it).base();
      blocklist.insert(f_it, Block{.id = Block::FREE, .size = r_it->size});
      blocklist.splice(to_put, blocklist, f_it, std::next(f_it));
    }
  }

  for (const auto& block : blocklist) {
    checksum.feed(block);
  }

  return checksum.sum();
}

} // namespace

int main() {
  std::cout << solve(read_input()) << '\n';
  return 0;
}
