
#include "oscibear_hal_time.h"

uint64_t HAL_getTick() {
  return CLINT->MTIME;
}

void HAL_delay(uint64_t time) {
  uint64_t target_tick = HAL_getTick() + (time * MTIME_FREQ);
  while (HAL_getTick() < target_tick) {
    // asm("nop");
  }
}
