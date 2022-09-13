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

#define UART_TXDATA_ADDR            0x54000000
#define UART_RXDATA_ADDR            0x54000004
#define UART_TXCTRL_ADDR            0x54000008
#define UART_RXCTRL_ADDR            0x5400000C
#define UART_IE_ADDR                0x54000010
#define UART_IP_ADDR                0x54000014
#define UART_DIV_ADDR               0x54000018

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
    switch (cmd) {
      case 'b':
        GET(BOOT_SELECT_ADDR);
        break;
      case 'B':
        for (uint8_t i=0; i<0x40; i+=4) {
          GET(BOOTROM_BASE_ADDR+i);
        }
        break;

      case 'i':
        GET(CLINT_MSIP_ADDR);
        break;

      case 't':
        GET(CLINT_MTIME_ADDR);
        break;

      case 'U':
        PUTFULL(UART_TXCTRL_ADDR, 0x1);
        PUTFULL(UART_TXDATA_ADDR, 0xDEADBEEF);
        break;


      case 'd':
        for (uint16_t i=0; i<0x40; i+=4) {
          GET(DTIM_BASE_ADDR+i);
        }
        break;

      case 'P':
        PUTFULL(DTIM_BASE_ADDR + 0, 0x54000437);
        PUTFULL(DTIM_BASE_ADDR + 4, 0x00040413);
        PUTFULL(DTIM_BASE_ADDR + 8, 0x00100493);
        PUTFULL(DTIM_BASE_ADDR + 12, 0x00942423);
        PUTFULL(DTIM_BASE_ADDR + 16, 0x06800293);
        PUTFULL(DTIM_BASE_ADDR + 20, 0x00542023);
        PUTFULL(DTIM_BASE_ADDR + 24, 0x06500293);
        PUTFULL(DTIM_BASE_ADDR + 28, 0x00542023);
        PUTFULL(DTIM_BASE_ADDR + 32, 0x06C00293);
        PUTFULL(DTIM_BASE_ADDR + 36, 0x00542023);
        PUTFULL(DTIM_BASE_ADDR + 40, 0x06C00293);
        PUTFULL(DTIM_BASE_ADDR + 44, 0x00542023);
        PUTFULL(DTIM_BASE_ADDR + 48, 0x06F00293);
        PUTFULL(DTIM_BASE_ADDR + 52, 0x00542023);
        PUTFULL(DTIM_BASE_ADDR + 56, 0x02000293);
        PUTFULL(DTIM_BASE_ADDR + 60, 0x00542023);
        PUTFULL(DTIM_BASE_ADDR + 64, 0x07700293);
        PUTFULL(DTIM_BASE_ADDR + 68, 0x00542023);
        PUTFULL(DTIM_BASE_ADDR + 72, 0x06F00293);
        PUTFULL(DTIM_BASE_ADDR + 76, 0x00542023);
        PUTFULL(DTIM_BASE_ADDR + 80, 0x07200293);
        PUTFULL(DTIM_BASE_ADDR + 84, 0x00542023);
        PUTFULL(DTIM_BASE_ADDR + 88, 0x06C00293);
        PUTFULL(DTIM_BASE_ADDR + 92, 0x00542023);
        PUTFULL(DTIM_BASE_ADDR + 96, 0x06400293);
        PUTFULL(DTIM_BASE_ADDR + 100, 0x00542023);
        PUTFULL(DTIM_BASE_ADDR + 104, 0x02000293);
        PUTFULL(DTIM_BASE_ADDR + 108, 0x00542023);
        PUTFULL(DTIM_BASE_ADDR + 112, 0x02000293);
        PUTFULL(DTIM_BASE_ADDR + 116, 0x00542023);
        PUTFULL(DTIM_BASE_ADDR + 120, 0x02D00293);
        PUTFULL(DTIM_BASE_ADDR + 124, 0x00542023);
        PUTFULL(DTIM_BASE_ADDR + 128, 0x02D00293);
        PUTFULL(DTIM_BASE_ADDR + 132, 0x00542023);
        PUTFULL(DTIM_BASE_ADDR + 136, 0x04F00293);
        PUTFULL(DTIM_BASE_ADDR + 140, 0x00542023);
        PUTFULL(DTIM_BASE_ADDR + 144, 0x05300293);
        PUTFULL(DTIM_BASE_ADDR + 148, 0x00542023);
        PUTFULL(DTIM_BASE_ADDR + 152, 0x04300293);
        PUTFULL(DTIM_BASE_ADDR + 156, 0x00542023);
        PUTFULL(DTIM_BASE_ADDR + 160, 0x04900293);
        PUTFULL(DTIM_BASE_ADDR + 164, 0x00542023);
        PUTFULL(DTIM_BASE_ADDR + 168, 0x00A00293);
        PUTFULL(DTIM_BASE_ADDR + 172, 0x00542023);
        PUTFULL(DTIM_BASE_ADDR + 176, 0xF61FF06F);

        HAL_Delay(100);
        PUTFULL(CLINT_MSIP_ADDR, 1);

        HAL_Delay(100);
        GET(0x80001000);
        break;
      case 'R':
        PUTFULL(0x80001000, 0);
        break;
      case 'r':
        GET(0x80001000);
        break;
      case '2':
        GET(CLINT_MSIP_ADDR);
        break;

      case 'u':
        GET(UART_TXDATA_ADDR);
        GET(UART_RXDATA_ADDR);
        GET(UART_TXCTRL_ADDR);
        GET(UART_RXCTRL_ADDR);
        GET(UART_DIV_ADDR);
        break;
    }
  }

  HAL_Delay(50);
}
