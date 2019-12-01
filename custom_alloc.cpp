#include "alloc_statements.hpp"
#include "custom_alloc.hpp"
#include <iostream>
#include <list>

CustomAllocator::CustomAllocator(size_t count) {
  begin = static_cast<uint8_t *>(calloc(count, sizeof(uint8_t)));
  store.push_back(new block(true, 0, begin));
}

CustomAllocator::~CustomAllocator() { free(begin); }

void *CustomAllocator::mem_alloc(size_t size) {
  if (size <
      BYTE_ALIGNMENT) //если запрашиваемый адрес меньше BYTE_ALIGNMENT, т.е. от
                      //1 до 3 байт, вернуть адрес из fast_store (быстрый поиск)
    return fast_store_search(size);

  if (store.back()->next + size <
      begin + BUFSIZE -
          BYTE_ALIGNMENT) //если свободного адреса нет в fast_store или блок
                          //больше BYTE_ALIGNMENT, то выделить новый блок
                          //(проверка на то, что в буффере еще есть место)
    if (size % BYTE_ALIGNMENT ==
        0) { //если запрашиваемый адрес кратен BYTE_ALIGNMENT, то вернуть адрес
      void *addr = static_cast<void *>(store.back()->next);
      store.push_back(new block(true, size, store.back()->next));
      return addr;
    } else { //если запрашиваемый адрес не кратен BYTE_ALIGNMENT, то вернуть
             //адрес и записать в fast_store наявность свободной памяти, равной
             //от 1 до 3 байт
      void *addr = static_cast<void *>(store.back()->next);
      block *b = new block(true, size, store.back()->next);
      store.push_back(b);
      if (size < BYTE_ALIGNMENT)
        fast_store.push_back(b);
      b = new block(false, BYTE_ALIGNMENT - size % BYTE_ALIGNMENT,
                    store.back()->next);
      store.push_back(b);
      fast_store.push_back(b);
      return addr;
    }

  return nullptr; //адреса нет в fast_store и в буффере все занято, значит
                  //вернуть nullptr
}

void *CustomAllocator::mem_realloc(void *addr, size_t size) {
  if (size < BYTE_ALIGNMENT) // 0 вариант. если запрашиваемый адрес меньше
                             // BYTE_ALIGNMENT, т.е. от 1 до 3 байт, вернуть
                             // адрес из fast_store (быстрый поиск)
  {
    void *new_addr = fast_store_search(size);
    return mem_cpy(addr, new_addr);
  }

  // 1 вариант. уменьшить блок данных который в наличии и вернуть (или вернуть
  // если равен size)
  block *b = addr_to_block(addr);
  if (b->size >= size) {
    b->used = true;
    b->size = size;
    return addr;
  }

  // 2 вариант. на увеличение. найти неиспользуемый блок данных, либо несколько
  // неиспользуемых блоков подряд и склеить их
  block *ptr = addr_to_block(addr);
  block b_tmp(true, ptr->size, ptr->addr);
  void *new_addr =
      congregate_blocks(store_search_free_blocks_in_row(size), size);
  if (new_addr != nullptr)
    return mem_cpy(
        addr, new_addr, b_tmp.size,
        addr_to_block(new_addr)
            ->size); //здесь нужно использовать копирование с явным указанием
                     //размера, тк в исключительных ситуациях перезапись
                     //происходит в те же байты и из-за склейки блоков теряются
                     //данные о размере, mem_cpy с меньшим количеством
                     //аргументов верет nullptr

  // 3 вариант. выделить новый блок и скопировать в него содержимое
  new_addr = mem_alloc(size);
  return mem_cpy(addr, new_addr);

  return nullptr;
}

void CustomAllocator::mem_free(void *addr) {
  block *block_to_remove = nullptr;

  for (block *b : store)
    if (b->addr == addr && b->used == true) {
      b->size = static_cast<size_t>(b->next - b->addr);
      b->used = false;
      /*std::cout << "!!!!!!!" << std::endl;
      for (size_t i = 0; i < b->size; ++i)
      *(b->addr + i) = 0;*/
    }

  // store.remove(block_to_remove);
}

void *CustomAllocator::getBegin() { return begin; }

void CustomAllocator::mem_dump() {
  std::cout << "fast_store: " << std::endl;
  for (block *b : fast_store) {
    static int i = 0;
    std::cout << i << ": " << static_cast<void *>(b->addr) << " "
              << static_cast<void *>(b->next) << " " << b->size << " "
              << (bool)b->used << std::endl;
    ++i;
  }
  std::cout << std::endl;

  std::cout << "sizeof(type)= " << sizeof(uint64_t) << std::endl;
  for (size_t i = 0; i < BUFSIZE; ++i) {
    uint8_t *addr = begin + i;
    uint8_t val = *addr;
    std::cout << i << ": " << static_cast<void *>(addr) << " : ";
    std::cout << (int)val;
    for (block *b : store) {
      if (b->addr == addr) {
        std::cout << "     <- " << b->size << " " << b->used;
      }
    }
    std::cout << std::endl;
  }
}

void CustomAllocator::memToFree_dump() {
  size_t memToFreeCounter = 0;

  for (block *b : store) {
    if (b->used == false)
      memToFreeCounter += static_cast<size_t>(b->next - b->addr);

    if (b->used == true)
      memToFreeCounter += static_cast<size_t>(b->next - b->addr) -
                          static_cast<int16_t>(b->size);
  }

  std::cout << "\nmemory to free: " << memToFreeCounter << std::endl;
}

void *CustomAllocator::mem_cpy(void *from, void *to) {
  block *b1 = addr_to_block(from);
  block *b2 = addr_to_block(to);

  if (b1 == nullptr || b2 == nullptr)
    return nullptr;

  size_t middle = (b1->size < b2->size) ? b1->size : b2->size;
  size_t end = (b1->size > b2->size) ? b1->size : b2->size;

  for (size_t i = 0; i < end; ++i) {
    *(b2->addr + i) = (i < middle) ? *(b1->addr + i) : 0;
  }

  return to;
}

void *CustomAllocator::mem_cpy(void *from, void *to, size_t from_size,
                               size_t to_size) {
  size_t middle = (from_size < to_size) ? from_size : to_size;
  size_t end = (from_size > to_size) ? from_size : to_size;

  for (size_t i = 0; i < end; ++i)
    *(static_cast<uint8_t *>(to) + i) =
        (i < middle) ? *(static_cast<uint8_t *>(from) + i) : 0;

  return to;
}

void *CustomAllocator::mem_set(void *addr, uint8_t value) {
  block *b = addr_to_block(addr);

  for (size_t i = 0; i < b->size; ++i)
    *(b->addr + i) = value;

  return addr;
}

////////////private section
block *CustomAllocator::addr_to_block(void *addr) {
  for (block *b : store) {
    if (b == store.front())
      continue;
    if (b->addr == addr)
      return b;
  }

  return nullptr;
}

std::list<block *>
CustomAllocator::store_search_free_blocks_in_row(size_t size) {
  size_t S = 0;
  std::list<block *> free_blocks_in_row;

  for (block *b : store) {
    if (b->used == true) {
      S = 0;
      free_blocks_in_row.clear();
      continue;
    }

    if (b->used == false) {
      S += static_cast<size_t>(b->next - b->addr);
      free_blocks_in_row.push_back(b);

      if (S >= size + ((size % BYTE_ALIGNMENT)
                           ? BYTE_ALIGNMENT - size % BYTE_ALIGNMENT
                           : 0)) {
        break;
      }
    }
  }

  if (S < size + ((size % BYTE_ALIGNMENT)
                      ? BYTE_ALIGNMENT - size % BYTE_ALIGNMENT
                      : 0))
    free_blocks_in_row.clear();
  for (block *b : free_blocks_in_row) {
    b->info();
  }
  return free_blocks_in_row;
}

void *CustomAllocator::fast_store_search(size_t size) {
  for (block *b : fast_store)
    if (b->used == false && b->size >= size && b->size < BYTE_ALIGNMENT) {
      void *addr = static_cast<void *>(b->addr);
      b->size = size;
      b->used = true;
      return addr;
    }
}

void *CustomAllocator::congregate_blocks(std::list<block *> &blocks_to_congr,
                                         size_t size_of_new_block) {
  if (blocks_to_congr.empty() == true)
    return nullptr;

  uint8_t *block_addr_begin =
      static_cast<uint8_t *>(blocks_to_congr.front()->addr);
  uint8_t *block_addr_next =
      static_cast<uint8_t *>(blocks_to_congr.back()->next);
  size_t block_full_size =
      static_cast<size_t>(block_addr_next - block_addr_begin);

  if (size_of_new_block > block_full_size)
    return nullptr;

  uint16_t counter = 0;

  for (block *b : blocks_to_congr) {
    if (size_of_new_block == block_full_size) {
      if (counter == 0) {
        b->addr = block_addr_begin;
        b->next = block_addr_next;
        b->size = block_full_size;
        b->used = true;
      } else {
        b->used = false;
        b->size = 0;
      }
    }

    if (size_of_new_block < block_full_size) {
      if (counter == 0) {
        b->addr = block_addr_begin;
        b->next = block_addr_begin + size_of_new_block;
        b->size = size_of_new_block;
        b->used = true;
      } else if (counter == 1) {
        b->addr = block_addr_begin + size_of_new_block;
        b->next = block_addr_begin + block_full_size;
        b->size = block_full_size - size_of_new_block;
        b->used = false;
      } else {
        b->used = false;
        b->size = 0;
      }
    }
    ++counter;
  }

  clean_store();

  return block_addr_begin;
}

void CustomAllocator::clean_store() {
  for (auto i = ++(store.begin()); i != store.end();) {
    if ((*i)->size == 0) {
      delete *i;
      i = store.erase(i);
    } else
      ++i;
  }
}
