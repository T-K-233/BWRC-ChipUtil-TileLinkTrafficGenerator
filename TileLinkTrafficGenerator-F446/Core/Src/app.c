/*
 * app.c
 *
 *  Created on: Aug 22, 2022
 *      Author: TK
 */

#include "app.h"

#define BOOT_SELECT_ADDR            0x00002000
#define BOOTROM_BASE_ADDR           0x00010000
#define CLINT_MSIP_ADDR             0x02000000
#define CLINT_MTIME_ADDR            0x0200BFF8
#define DTIM_BASE_ADDR              0x80000000


extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim4;
extern UART_HandleTypeDef huart2;

TileLinkController tl;

char str[128];

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
  TL_update(&tl);
}

uint8_t APP_getUsrButton() {
  return HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) ? 0 : 1;
}

void APP_setLED(uint8_t state) {
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, state);
}

void APP_init() {
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1);
}

void GET(uint32_t address) {
  TL_GET(&tl, address);
  sprintf(str, "[TL Get] <address: 0x%08lx, size: %d>\r\n", tl.tx_frame.address, tl.tx_frame.size);
  HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen(str), 1000);
  sprintf(str, "[TL AccessAckData] <chanid: %d, opcode: %d, size: %d, source: 0x%08lx, denied: %d, corrupt: %d, data: 0x%08lx%08lx>\r\n", tl.rx_frame.chanid, tl.rx_frame.opcode, tl.rx_frame.size, tl.rx_frame.address, tl.rx_frame.mask, tl.rx_frame.corrupt, (uint32_t)(tl.rx_frame.data >> 32), (uint32_t)tl.rx_frame.data);
  HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen(str), 1000);
}

void PUTFULL(uint32_t address, uint64_t data) {
  TL_PUTFULLDATA(&tl, address, data);
  sprintf(str, "[TL PutFullData] <address: 0x%08lx, size: %d, data: data: 0x%08lx%08lx>\r\n", tl.tx_frame.address, tl.tx_frame.size, (uint32_t)(tl.tx_frame.data >> 32), (uint32_t)tl.tx_frame.data);
  HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen(str), 1000);
  sprintf(str, "[TL AccessAck] <chanid: %d, opcode: %d, size: %d, source: 0x%08lx, denied: %d, corrupt: %d>\r\n", tl.rx_frame.chanid, tl.rx_frame.opcode, tl.rx_frame.size, tl.rx_frame.address, tl.rx_frame.mask, tl.rx_frame.corrupt);
  HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen(str), 1000);
}

void APP_main() {
  uint8_t cmd;
  if (HAL_UART_Receive(&huart2, &cmd, 1, 1000) == HAL_OK) {
    if (cmd == 'b') {
      GET(BOOT_SELECT_ADDR);
    }
    if (cmd == 'i') {
      GET(CLINT_MSIP_ADDR);
    }
    if (cmd == 't') {
      GET(CLINT_MTIME_ADDR);
    }
    if (cmd == 'r') {
      GET(BOOTROM_BASE_ADDR);
    }
    if (cmd == 'd') {
      GET(DTIM_BASE_ADDR);
    }
    if (cmd == 'D') {
      PUTFULL(DTIM_BASE_ADDR, 0x0000000001010101);
    }
  }

  HAL_Delay(50);
}
