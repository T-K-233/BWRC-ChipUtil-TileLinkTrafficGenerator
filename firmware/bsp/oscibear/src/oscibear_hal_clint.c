
#include "oscibear_hal_clint.h"


void HAL_CLINT_triggerInterrupt(uint32_t hartid) {
  SET_BITS(CLINT->MSIP, 1U << (hartid));
}

uint64_t HAL_CLINT_getMTime() {
  uint32_t time_lo;
  uint32_t time_hi;

	do {
		time_hi = *((uint32_t *)(&CLINT->MTIME) + 1);
		time_lo = *((uint32_t *)(&CLINT->MTIME));
	} while (*((uint32_t *)(&CLINT->MTIME) + 1) != time_hi);

	return (((uint64_t)time_hi) << 32U) | time_lo;
}

void HAL_CLINT_setMTimeCmp(uint64_t time) {
  *((uint32_t *)(&CLINT->MTIMECMP) + 1) = 0xffffffff;
	*((uint32_t *)(&CLINT->MTIMECMP)) = (uint32_t)time;
	*((uint32_t *)(&CLINT->MTIMECMP) + 1) = (uint32_t)(time >> 32);
}
