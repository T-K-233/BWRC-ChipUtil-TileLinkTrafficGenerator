
#include "oscibear_hal.h"

#include "main.h"


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

void MachineExternal_IRQn_Handler() {
  uint32_t m_cause;
  char str[16];
  sprintf(str, "interrupt: %x\n", m_cause);
  HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
}


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
    if (m_cause == 0x8000000B) {
      // machine external interrupt
      
    }
    
    uint32_t irqSource = plic_claim_irq(0);
  
    char str[128];

    sprintf(str, "intr %d\n", irqSource);
    HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
  
    if (irqSource == 6) {
      sprintf(str, "** RX Error Message: %u\n", baseband_rxerror_message());
    }
    if (irqSource == 7) {
      sprintf(str, "** RX Start\n");
    }
    if (irqSource == 8) {
      sprintf(str, "** Bytes Read: %u\n", baseband_rxfinish_message());
    }
    if (irqSource == 9) {
      sprintf(str, "TX Operation Failed. Error message: %u\n", baseband_txerror_message());
    }
    if (irqSource == 10) {
      sprintf(str, "TX Operation Finished. Check above for any errors.\n");
    }

    HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
    plic_complete_irq(0, irqSource);

    // HAL_GPIO_writePin(GPIOA, GPIO_PIN_0, 1);
    // sprintf(str, "mcause: %x\n", m_cause);
  }
}

