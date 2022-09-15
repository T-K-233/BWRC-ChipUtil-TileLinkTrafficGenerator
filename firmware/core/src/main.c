
#include "main.h"

int main() {
  HAL_init();

  HAL_UART_init(UART0);
  HAL_GPIO_init(GPIOA, GPIO_PIN_0);

  HAL_GPIO_writePin(GPIOA, GPIO_PIN_0, 0);

  while (1) {
    char str[] = " hello world\n";
    HAL_UART_transmit(UART0, (uint8_t *)str, 12, 0);
    
    HAL_delay(500);
  }
}