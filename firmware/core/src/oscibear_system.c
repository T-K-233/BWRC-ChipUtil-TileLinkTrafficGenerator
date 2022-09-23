
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

void UserSoftware_IRQn_Handler() {}
void SupervisorSoftware_IRQn_Handler() {}
void HypervisorSoftware_IRQn_Handler() {}
void MachineSoftware_IRQn_Handler() {}
void UserTimer_IRQn_Handler() {}
void SupervisorTimer_IRQn_Handler() {}
void HypervisorTimer_IRQn_Handler() {}
void MachineTimer_IRQn_Handler() {}
void UserExternal_IRQn_Handler() {}
void SupervisorExternal_IRQn_Handler() {}
void HypervisorExternal_IRQn_Handler() {}
void MachineExternal_IRQn_Handler() {}


// void __attribute__ ((interrupt)) trap_handler(void) {  
void trap_handler() {
  uint32_t m_cause;
  asm volatile("csrr %0, mcause" : "=r"(m_cause));

  uint8_t is_interrupt = READ_BITS(m_cause, 0x80000000) ? 1 : 0;

  if (is_interrupt) {
    if (m_cause == 0x80000003) {
      // machine software interrupt
      CLINT->MSIP = 0;
    }
    if (m_cause == 0x80000007) {
      // machine timer interrupt
      CLINT->MTIMECMP = 0xFFFFFFFFFFFFFFFF;
    }

      HAL_GPIO_writePin(GPIOA, GPIO_PIN_0, 1);
      char str[16];
      sprintf(str, "mcause: %x\n", m_cause);
      HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
  }
}

