#ifndef SATORI_RECYCLER_HPP
#define SATORI_RECYCLER_HPP

#include <deque>
#include <stack>
#include <tuple>
#include <type_traits>

namespace satori {

constexpr unsigned nextPow2(unsigned x) {
  unsigned n = 2;
  while (n < x) {
    n *= 2;
  }
  return n;
};

template <unsigned minBlockSize = 64> struct Block {
  static constexpr unsigned blockSize = nextPow2(minBlockSize);
  char data[blockSize];
  constexpr static size_t size() { return blockSize; }
};

template <class Block = Block<64>> struct BlockRecycler {

  std::deque<Block> blocks;
  std::stack<Block *> pool;

  BlockRecycler(unsigned prealloc = 4096 / Block::size() + 1)
    : blocks(prealloc) {
    for (auto &b : blocks) {
      pool.push(&b);
    }
  }

  void *allocate(size_t x) {
    if (x > Block::size())
      return nullptr;
    return allocate();
  }

  void *allocate() {
    if (pool.empty()) {
      blocks.emplace_back();
      return &blocks.back();
    }

    void *b = pool.top();
    pool.pop();
    return b;
  }

  void release(void *ptr) { pool.push((Block *)ptr); }
};

template <unsigned blockSize = 64> struct SmartBlock {

  struct Meta {
    unsigned free = 0;
  };

  char data[blockSize - sizeof(Meta)];
  Meta meta;

  constexpr static size_t size() { return blockSize - sizeof(Meta); }
};

template <unsigned minBlockSize = 64>
struct SmartRecycler : BlockRecycler<SmartBlock<minBlockSize>> {

  using Parent = BlockRecycler<SmartBlock<minBlockSize>>;
  using Block = SmartBlock<minBlockSize>;
  SmartRecycler(unsigned const &n) : BlockRecycler<Block>(n) {}

  template <class T, class... Xs> T *create(Xs &&... xs) {
    if (sizeof(T) > Block::size())
      return nullptr;
    auto block = (Block *)Parent::allocate();
    block->meta.free = Block::size() - sizeof(T);
    return new (block) T(std::forward<Xs>(xs)...);
  }

  template <class T, class... Xs>
  static T *createIntoBlock(void *b, Xs &&... xs) {
    Block *block = (Block *)b;
    if (block->meta.free >= sizeof(T)) {
      auto obj = new (&block->data[Block::size() - block->meta.free])
        T(std::forward<Xs>(xs)...);
      block->meta.free -= sizeof(T);
      return obj;
    }

    return new T(std::forward<Xs>(xs)...);
  }

  template <class T> void destroy(T *ptr) {
    ptr->~T();
    Parent::release(ptr);
  };
};

} // namespace satori

#endif
