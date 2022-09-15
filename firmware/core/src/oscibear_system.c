
#include "oscibear_hal.h"

void system_init(void) {
  asm("li t1, 0x80005000");
  asm("csrr t0, mstatus");
  asm("sw t0, 0(t1)");
  asm("csrr t0, mtvec");
  asm("sw t0, 4(t1)");
  asm("csrr t0, mcause");
  asm("sw t0, 8(t1)");
  asm("li t0, 0b100");
  asm("csrs mstatus, t0");
  asm("csrr t0, mstatus");
  asm("sw t0, 12(t1)");
}

void interrupt_handler(uintptr_t cause, uintptr_t epc, uintptr_t regs[32]) {
  HAL_GPIO_writePin(GPIOA, GPIO_PIN_0, 1);
    
  // if (cause & MCAUSE_INT_MASK) {
  //   // Handle interrupt
  //   int code = cause & MCAUSE_CODE_MASK;
  //   if (clint_handler_vector[code])
  //   {
  //     (*clint_handler_vector[code])();
  //   }
  // } else {
  //   // Handle exception
  //   printf("Exception encountered (cause = %x, epc = %x)\n", cause & MCAUSE_CODE_MASK, epc);
  //   for (int i = 1; i < 32; i++)
  //   {
  //     printf("x%d = 0x%x (%dd)\n", i, regs[i], regs[i]);
  //   }
  // }
  
  // return epc;
}