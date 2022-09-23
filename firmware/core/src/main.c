
#include "main.h"

int main() {
  HAL_init();

  UART_InitTypeDef UART_init_config;
  UART_init_config.baudrate = 10000;

  HAL_UART_init(UART0, &UART_init_config);
  HAL_GPIO_init(GPIOA, GPIO_PIN_0);


  HAL_CORE_enableInterrupt();
  HAL_CORE_enableIRQ(MachineSoftware_IRQn);
  
  uint64_t target_tick = HAL_getTick() + (3000 * MTIME_FREQ);
  HAL_CLINT_setMTimeCmp(target_tick);

  HAL_CORE_enableIRQ(MachineTimer_IRQn);

  while (1) {
    char str[64];
    sprintf(str, "time: %d\ttimecmp: %d\n", (uint32_t)CLINT->MTIME, (uint32_t)CLINT->MTIMECMP);
    HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);

    HAL_GPIO_writePin(GPIOA, GPIO_PIN_0, 0);
    HAL_delay(1000);
  }
}
