
#include "oscibear_hal_core.h"


void HAL_CORE_enableInterrupt() {
  // Set MPIE
  uint32_t mask = (1U << 3U);
  asm volatile("csrs mstatus, %0" :: "r"(mask));
}

void HAL_CORE_enableIRQ(IRQn_Type IRQn) {
  uint32_t mask = (1U << (uint32_t)IRQn);
  asm volatile("csrs mie, %0" :: "r"(mask));
}
