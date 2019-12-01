#pragma once

class block {
public:
  block(bool U = false, size_t S = 0, uint8_t *A = nullptr) {
    used = U;
    size = S;
    addr = A;
    next = addr + S;
    // std::cout << "next: " << (void *)next << " size: " << S << std::endl;
  }

  bool used;
  size_t size;
  uint8_t *addr;
  uint8_t *next;

  void info() {
    std::cout << "b->info(); " << static_cast<void *>(addr) << " "
              << static_cast<void *>(next) << " " << size << " " << used
              << std::endl;
    ;
  }
};
