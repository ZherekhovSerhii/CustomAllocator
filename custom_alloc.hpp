#pragma once
#include "block.hpp"

class CustomAllocator {
public:
  CustomAllocator(size_t size);
  ~CustomAllocator();

  void *mem_alloc(size_t size);
  void *mem_realloc(void *addr, size_t size);
  void mem_free(void *addr);

  void *getBegin();
  void mem_dump();
  void memToFree_dump();

  void *mem_cpy(void *from, void *to);
  void *mem_cpy(void *from, void *to, size_t from_size, size_t to_size);
  void *mem_set(void *addr, uint8_t value);

private:
  block *addr_to_block(void *addr);
  std::list<block *> store_search_free_blocks_in_row(size_t size);
  void *fast_store_search(size_t size);
  void *congregate_blocks(std::list<block *> &blocks_to_congr, size_t size);
  void clean_store();

  uint8_t *begin = nullptr;

  std::list<block *> store;
  std::list<block *> fast_store;
};