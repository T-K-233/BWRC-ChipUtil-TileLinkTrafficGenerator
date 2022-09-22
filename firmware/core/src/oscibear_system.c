
#include "oscibear_hal.h"

void system_init(void) {
//   asm("li t1, 0x80005000");
//   asm("csrr t0, mstatus");
//   asm("sw t0, 0(t1)");
//   asm("csrr t0, mtvec");
//   asm("sw t0, 4(t1)");
//   asm("csrr t0, mcause");
//   asm("sw t0, 8(t1)");
//   asm("li t0, 0b100");
//   asm("csrs mstatus, t0");
//   asm("csrr t0, mstatus");
//   asm("sw t0, 12(t1)");
}


// void __attribute__ ((interrupt)) trap_handler(void) {
  
void trap_handler() {
  HAL_GPIO_writePin(GPIOA, GPIO_PIN_0, 1);
  char str[] = "interrupt\n";
  HAL_UART_transmit(UART0, (uint8_t *)str, 10, 0);
}