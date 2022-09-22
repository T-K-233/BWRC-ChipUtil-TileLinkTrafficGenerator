
#include "oscibear_hal_time.h"

uint64_t HAL_getTick() {
  uint32_t time_lo;
  uint32_t time_hi;

	do {
		time_hi = *((uint32_t *)(&CLINT->MTIME) + 1);
		time_lo = *((uint32_t *)(&CLINT->MTIME));
	} while (*((uint32_t *)(&CLINT->MTIME) + 1) != time_hi);

	return (((uint64_t)time_hi) << 32U) | time_lo;
}

void  HAL_setTickTarget(uint64_t time) {
  *((uint32_t *)(&CLINT->MTIME) + 1) = 0xffffffff;
	*((uint32_t *)(&CLINT->MTIME)) = (uint32_t)time;
	*((uint32_t *)(&CLINT->MTIME) + 1) = (uint32_t)(time >> 32);
}

void HAL_delay(uint64_t time) {
  uint64_t target_tick = HAL_getTick() + (time * MTIME_FREQ);
  while (HAL_getTick() < target_tick) {
    // asm("nop");
  }
}
