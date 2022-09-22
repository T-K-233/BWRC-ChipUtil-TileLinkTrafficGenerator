
#include "main.h"

#define INT_CODE_USER_SOFTWARE 0
#define INT_CODE_SUPERVISOR_SOFTWARE 1
#define INT_CODE_MACHINE_SOFTWARE 3
#define INT_CODE_USER_TIMER 4
#define INT_CODE_SUPERVISOR_TIMER 5
#define INT_CODE_MACHINE_TIMER 7
#define INT_CODE_USER_EXTERNAL 8
#define INT_CODE_SUPERVISOR_EXTERNAL 9
#define INT_CODE_MACHINE_EXTERNAL 11

#define TOHOST  *(uint32_t *)(0x80005000)

void enableInterrupt() {
  // Set MPIE
  uint32_t mask = (1 << 3);
  asm volatile("csrs mstatus, %0" :: "r"(mask));
}
void clint_machine_interrupt_enable(int code)
{
  uint32_t mask = 1 << code;
  asm volatile("csrs mie, %0" :: "r"(mask));
}

int main() {
  HAL_init();

  UART_InitTypeDef UART_init_config;
  UART_init_config.baudrate = 10000;

  HAL_UART_init(UART0, &UART_init_config);
  HAL_GPIO_init(GPIOA, GPIO_PIN_0);


  // enableInterrupt();
  clint_machine_interrupt_enable(INT_CODE_MACHINE_SOFTWARE);

  while (1) {
    char str[] = "hello world\n";
    HAL_UART_transmit(UART0, (uint8_t *)str, 12, 0);

    HAL_GPIO_writePin(GPIOA, GPIO_PIN_0, 0);
    HAL_delay(500);
  }
}