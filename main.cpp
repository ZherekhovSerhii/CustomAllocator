#include "alloc_statements.hpp"
#include "custom_alloc.hpp"
#include <iostream>
#include <list>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  CustomAllocator buffer(BUFSIZE);

  uint64_t *g;
  g = static_cast<uint64_t *>(buffer.mem_alloc(sizeof(uint64_t)));
  *g = uint64_t(std::numeric_limits<uint64_t>::max());

  for (int i = 0; i < 10; ++i) {
    uint8_t *h;
    h = static_cast<uint8_t *>(buffer.mem_alloc(sizeof(uint8_t) * 10));
    *h = uint8_t(std::numeric_limits<uint8_t>::max());
  }

  /*uint16_t * h1;
  h1 = static_cast<uint16_t *>(buffer.mem_alloc(sizeof(uint16_t)));
  *h1 = uint16_t(std::numeric_limits<uint16_t>::max());


  uint32_t * h;
  h = static_cast<uint32_t *>(buffer.mem_alloc(sizeof(uint32_t)));
  *h = uint32_t(std::numeric_limits<uint32_t>::max());
  */

  uint8_t arr[5];
  uint8_t *i = arr;
  i = static_cast<uint8_t *>(buffer.mem_alloc(sizeof(uint8_t) * 5));
  *(i + 0) = uint8_t(3);
  *(i + 1) = uint8_t(3);
  *(i + 2) = uint8_t(3);
  *(i + 3) = uint8_t(3);
  *(i + 4) = uint8_t(3);

  i = static_cast<uint8_t *>(buffer.mem_alloc(sizeof(uint8_t) * 5));
  *(i + 0) = uint8_t(3);
  *(i + 1) = uint8_t(3);
  *(i + 2) = uint8_t(3);
  *(i + 3) = uint8_t(3);
  *(i + 4) = uint8_t(3);

  buffer.mem_free(i);

  uint16_t *h2;
  h2 = static_cast<uint16_t *>(buffer.mem_alloc(sizeof(uint8_t) * 4));
  *h2 = uint16_t(std::numeric_limits<uint16_t>::max());

  uint16_t *h3;
  h3 = static_cast<uint16_t *>(buffer.mem_alloc(sizeof(uint8_t) * 3));
  *h3 = uint16_t(std::numeric_limits<uint16_t>::max());

  buffer.mem_free(h2);

  uint64_t *g64;
  g64 = static_cast<uint64_t *>(buffer.mem_alloc(sizeof(uint64_t)));
  *g64 = uint64_t(std::numeric_limits<uint64_t>::max());

  buffer.mem_free(g64);
  buffer.mem_free(h3);

  buffer.mem_realloc(h2, sizeof(uint8_t) * 7);
  buffer.mem_free(h2);

  buffer.mem_realloc(g64, sizeof(uint8_t) * 17);
  buffer.mem_free(g64);

  buffer.mem_dump();
  buffer.memToFree_dump();

  _getch();
  return 0;
}