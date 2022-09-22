/*
 * app.c
 *
 *  Created on: Aug 22, 2022
 *      Author: TK
 */

#include "app.h"


// running 200k TL clock
// period should be 5us (5e-06)
// TIM2 is counting 10M (1e-07)
// tick should be 50 ticks

#define PERIOD_TICKS        50


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

#define GPIO_BASE_ADDR              0x10012000


extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;
extern UART_HandleTypeDef huart2;

TileLinkController tl;

char str[128];

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
  __HAL_TIM_SET_COUNTER(&htim2, 0);

  if (tl.tx_pending) {
    tl.tx_bit_offset = 0;
    HAL_GPIO_WritePin(TL_MOSI_Data_GPIO_Port, TL_MOSI_Data_Pin, tl.tx_frame.buffer[tl.tx_bit_offset]);
    HAL_GPIO_WritePin(TL_MISO_Ready_GPIO_Port, TL_MISO_Ready_Pin, 1);
    HAL_GPIO_WritePin(TL_MOSI_Valid_GPIO_Port, TL_MOSI_Valid_Pin, 1);

    // transmit all packets
    while (tl.tx_bit_offset < TL_SERDES_TOTAL_SIZE) {
      // wait for next clock
      while (__HAL_TIM_GET_COUNTER(&htim2) < PERIOD_TICKS) {}
      __HAL_TIM_SET_COUNTER(&htim2, 0);

      HAL_GPIO_WritePin(TL_MOSI_Data_GPIO_Port, TL_MOSI_Data_Pin, tl.tx_frame.buffer[tl.tx_bit_offset]);
      tl.tx_bit_offset += 1;
    }

    HAL_GPIO_WritePin(TL_MOSI_Valid_GPIO_Port, TL_MOSI_Valid_Pin, 0);
    tl.tx_finished = 1;
    tl.tx_pending = 0;

    return;
  }


  if (tl.rx_pending) {
    tl.rx_bit_offset = 0;

    // wait for valid
    while (HAL_GPIO_ReadPin(TL_MISO_Valid_GPIO_Port, TL_MISO_Valid_Pin) != GPIO_PIN_SET) {}

    while (tl.rx_bit_offset < TL_SERDES_TOTAL_SIZE) {
      // wait for next clock
      while (__HAL_TIM_GET_COUNTER(&htim2) < PERIOD_TICKS) {}
      __HAL_TIM_SET_COUNTER(&htim2, 0);

      tl.rx_frame.buffer[tl.rx_bit_offset] = HAL_GPIO_ReadPin(TL_MISO_Data_GPIO_Port, TL_MISO_Data_Pin);
      tl.rx_bit_offset += 1;
    }

    HAL_GPIO_WritePin(TL_MISO_Ready_GPIO_Port, TL_MISO_Ready_Pin, 0);
    tl.rx_finished = 1;
    tl.rx_pending = 0;

    return;
  }
}

#define SERIAL_BUFFER_SIZE    64

uint8_t serial_rx_buffer[SERIAL_BUFFER_SIZE];
uint8_t serial_tx_buffer[SERIAL_BUFFER_SIZE];

uint8_t frame_pending = 0;

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size) {
  if (huart == &huart2) {

    tl.tx_frame.chanid  = *(serial_rx_buffer);
    tl.tx_frame.opcode  = (*(serial_rx_buffer + 1)) & 0b111;
    tl.tx_frame.param   = (*(serial_rx_buffer + 1)) >> 4;
    tl.tx_frame.size    = *(serial_rx_buffer + 2);
    tl.tx_frame.source  = 0;
    tl.tx_frame.address = *(uint32_t *)(serial_rx_buffer + 4);
    tl.tx_frame.data    = *(uint32_t *)(serial_rx_buffer + 8);
    tl.tx_frame.corrupt = (*(serial_rx_buffer + 1) >> 7) & 0b1;
    tl.tx_frame.mask    = *(serial_rx_buffer + 3);
    tl.tx_frame.last    = 1;

    frame_pending = 1;
  }

  HAL_UARTEx_ReceiveToIdle_DMA(&huart2, serial_rx_buffer, 20);
}


uint8_t APP_getUsrButton() {
  return HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) ? 0 : 1;
}

void APP_setLED(uint8_t state) {
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, state);
}

void APP_init() {
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_Base_Start(&htim2);
  HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1);

  HAL_UARTEx_ReceiveToIdle_DMA(&huart2, serial_rx_buffer, 20);
}


void APP_main() {
  if (frame_pending) {
    TL_transmit(&tl);
    while (!tl.rx_finished) {}
    TL_deserialize(&tl.rx_frame);
    frame_pending = 0;

    *(serial_tx_buffer) = tl.rx_frame.chanid;
    *(serial_tx_buffer + 1) = (tl.rx_frame.corrupt << 7) | (tl.rx_frame.param << 4) | tl.rx_frame.opcode;
    *(serial_tx_buffer + 2) = tl.rx_frame.size;
    *(serial_tx_buffer + 3) = tl.rx_frame.mask;
    *(uint32_t *)(serial_tx_buffer + 4) = tl.rx_frame.address;
    *(uint32_t *)(serial_tx_buffer + 8) = tl.rx_frame.data;

    HAL_UART_Transmit(&huart2, serial_tx_buffer, 12, 1000);
  }
}
