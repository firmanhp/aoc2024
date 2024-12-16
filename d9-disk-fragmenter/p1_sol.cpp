#include <algorithm>
#include <cassert>
#include <chrono>
#include <deque>
#include <fstream>
#include <iostream>
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
    assert(block.id != Block::FREE);
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

long long solve(std::vector<Block> grid) {
  ChecksumProcessor checksum;
  std::deque<Block> blocklist;
  for (Block block : grid) {
    blocklist.push_back(std::move(block));
  }

  // pop block of size `size`.
  const auto pop_block = [&blocklist](int size) -> std::optional<Block> {
    while (!blocklist.empty() && blocklist.back().id == Block::FREE) {
      blocklist.pop_back();
    }

    if (blocklist.empty()) {
      return std::nullopt;
    }

    Block& block = blocklist.back();
    const int popped_size = std::min(size, block.size);
    const int id = block.id;

    block.size -= popped_size;
    if (block.size == 0) {
      blocklist.pop_back();
    }
    return Block{.id = id, .size = popped_size};
  };

  while (!blocklist.empty()) {
    Block& block = blocklist.front();
    if (block.id != Block::FREE) {
      checksum.feed(block);
      blocklist.pop_front();
      continue;
    }

    // free block
    auto popped_block = pop_block(block.size);
    if (!popped_block.has_value()) {
      // blocklist is no more
      break;
    }

    block.size -= popped_block->size;
    if (block.size == 0) {
      blocklist.pop_front();
    }
    blocklist.push_front(*std::move(popped_block));
  }

  return checksum.sum();
}

} // namespace

int main() {
  std::cout << solve(read_input()) << '\n';
  return 0;
}
