/*
 * app.c
 *
 *  Created on: Aug 22, 2022
 *      Author: TK
 */

#include "app.h"



#define TL_SERDES_CHANID_SIZE     3
#define TL_SERDES_OPCODE_SIZE     3
#define TL_SERDES_PARAM_SIZE      3
#define TL_SERDES_SIZE_SIZE       4
#define TL_SERDES_SOURCE_SIZE     4
#define TL_SERDES_ADDRESS_SIZE    32
#define TL_SERDES_MASK_SIZE       8
#define TL_SERDES_DATA_SIZE       64
#define TL_SERDES_CORRUPT_SIZE    1
#define TL_SERDES_LAST_SIZE       1
#define TL_SERDES_TOTAL_SIZE      (TL_SERDES_CHANID_SIZE+TL_SERDES_OPCODE_SIZE+TL_SERDES_PARAM_SIZE \
                                  +TL_SERDES_SIZE_SIZE+TL_SERDES_SOURCE_SIZE+TL_SERDES_ADDRESS_SIZE+TL_SERDES_MASK_SIZE \
                                  +TL_SERDES_DATA_SIZE+TL_SERDES_CORRUPT_SIZE+TL_SERDES_LAST_SIZE)

extern TIM_HandleTypeDef htim4;
extern UART_HandleTypeDef huart2;


uint8_t offset;
uint8_t tx_enabled = 0;
uint8_t tl_buffer[130];

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
  if (tx_enabled) {
    HAL_GPIO_WritePin(TL_MOSI_Valid_GPIO_Port, TL_MOSI_Valid_Pin, 1);
    HAL_GPIO_WritePin(TL_MISO_Ready_GPIO_Port, TL_MISO_Ready_Pin, 1);
    HAL_GPIO_WritePin(TL_MOSI_Data_GPIO_Port, TL_MOSI_Data_Pin, tl_buffer[TL_SERDES_TOTAL_SIZE-offset-1]);

    offset += 1;

    if (offset > TL_SERDES_TOTAL_SIZE) {
      HAL_GPIO_WritePin(TL_MOSI_Valid_GPIO_Port, TL_MOSI_Valid_Pin, 0);
      HAL_GPIO_WritePin(TL_MOSI_Data_GPIO_Port, TL_MOSI_Data_Pin, 0);
      tx_enabled = 0;
    }
  }
}

void TL_startTransmit() {
  offset = 0;

  uint64_t tl_chanid  = 0;
  uint64_t tl_opcode  = 0;
  uint64_t tl_param   = 0;
  uint64_t tl_size    = 2;
  uint64_t tl_source  = 0;
  uint64_t tl_address = 0x80000000;
  uint64_t tl_data    = 0x0000000000000001;
  uint64_t tl_corrupt = 0;
  uint64_t tl_mask    = 0b00001111;
  uint64_t tl_last    = 1;

  for (uint8_t i=0; i<TL_SERDES_CHANID_SIZE; i+=1) {
    tl_buffer[offset+i] = (tl_chanid >> i) & 0b1;
  }
  offset += TL_SERDES_CHANID_SIZE;

  for (uint8_t i=0; i<TL_SERDES_OPCODE_SIZE; i+=1) {
    tl_buffer[offset+i] = (tl_opcode >> i) & 0b1;
  }
  offset += TL_SERDES_OPCODE_SIZE;

  for (uint8_t i=0; i<TL_SERDES_PARAM_SIZE; i+=1) {
    tl_buffer[offset+i] = (tl_param >> i) & 0b1;
  }
  offset += TL_SERDES_PARAM_SIZE;

  for (uint8_t i=0; i<TL_SERDES_SIZE_SIZE; i+=1) {
    tl_buffer[offset+i] = (tl_size >> i) & 0b1;
  }
  offset += TL_SERDES_SIZE_SIZE;

  for (uint8_t i=0; i<TL_SERDES_SOURCE_SIZE; i+=1) {
    tl_buffer[offset+i] = (tl_source >> i) & 0b1;
  }
  offset += TL_SERDES_SOURCE_SIZE;

  for (uint8_t i=0; i<TL_SERDES_ADDRESS_SIZE; i+=1) {
    tl_buffer[offset+i] = (tl_address >> i) & 0b1;
  }
  offset += TL_SERDES_ADDRESS_SIZE;

  for (uint8_t i=0; i<TL_SERDES_MASK_SIZE; i+=1) {
    tl_buffer[offset+i] = (tl_mask >> i) & 0b1;
  }
  offset += TL_SERDES_MASK_SIZE;

  for (uint8_t i=0; i<TL_SERDES_DATA_SIZE; i+=1) {
    tl_buffer[offset+i] = (tl_data >> i) & 0b1;
  }
  offset += TL_SERDES_DATA_SIZE;

  for (uint8_t i=0; i<TL_SERDES_CORRUPT_SIZE; i+=1) {
    tl_buffer[offset+i] = (tl_corrupt >> i) & 0b1;
  }
  offset += TL_SERDES_CORRUPT_SIZE;

  for (uint8_t i=0; i<TL_SERDES_LAST_SIZE; i+=1) {
    tl_buffer[offset+i] = (tl_last >> i) & 0b1;
  }
  offset += TL_SERDES_LAST_SIZE;


  offset = 0;
  tx_enabled = 1;
}


uint8_t APP_getUsrButton() {
  return !HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);
}

void APP_setLED(uint8_t state) {
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, state);
}

void TL_Clock() {
  HAL_GPIO_WritePin(TL_Clk_GPIO_Port, TL_Clk_Pin, 1);
  HAL_Delay(1);
  HAL_GPIO_WritePin(TL_Clk_GPIO_Port, TL_Clk_Pin, 0);
  HAL_Delay(1);
}

void APP_init() {
  HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1);
}

void APP_main() {
  if (APP_getUsrButton()) {
    TL_startTransmit();
    APP_setLED(1);
    HAL_Delay(50);
  }

  HAL_Delay(100);
}
