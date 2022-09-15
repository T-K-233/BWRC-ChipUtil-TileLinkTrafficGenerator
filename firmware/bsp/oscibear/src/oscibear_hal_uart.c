
#include "oscibear_hal_uart.h"

void HAL_UART_init(UART_TypeDef *UARTx) {
  SET_BITS(UARTx->RXCTRL, UART_RXCTRL_RXEN_MSK);
  SET_BITS(UARTx->TXCTRL, UART_TXCTRL_TXEN_MSK);

  // baudrate setting
  // f_baud = f_sys / (div + 1)
}

uint8_t HAL_UART_getRXFIFODepth(UART_TypeDef *UARTx) {
  return READ_BITS(UARTx->RXCTRL, UART_RXCTRL_RXCNT_MSK) >> UART_RXCTRL_RXCNT_POS;
}

uint8_t HAL_UART_getTXFIFODepth(UART_TypeDef *UARTx) {
  return READ_BITS(UARTx->TXCTRL, UART_TXCTRL_TXCNT_MSK) >> UART_TXCTRL_TXCNT_POS;
}

Status HAL_UART_receive(UART_TypeDef *UARTx, uint8_t *data, uint16_t size, uint32_t timeout) {
  while (size > 0) {
    while (!HAL_UART_getRXFIFODepth(UARTx)) {
      // return TIMEOUT;
    }
    *data = UARTx->RXDATA;
    data += sizeof(uint8_t);
    size -= 1;
  }
  return OK;
}

Status HAL_UART_transmit(UART_TypeDef *UARTx, uint8_t *data, uint16_t size, uint32_t timeout) {
  while (size > 0) {
    while (HAL_UART_getTXFIFODepth(UARTx)) {
      // return TIMEOUT;
    }
    UARTx->TXDATA = *data;
    data += sizeof(uint8_t);
    size -= 1;
  }
  return OK;
}

