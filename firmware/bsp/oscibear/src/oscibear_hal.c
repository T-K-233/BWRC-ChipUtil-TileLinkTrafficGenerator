
#include "oscibear_hal.h"

void HAL_init() {
  
}

uint64_t HAL_getTick() {
  return HAL_CLINT_getMTime();
}

void HAL_delay(uint64_t time) {
  uint64_t target_tick = HAL_getTick() + (time * MTIME_FREQ);
  while (HAL_getTick() < target_tick) {
    // asm("nop");
  }
}
