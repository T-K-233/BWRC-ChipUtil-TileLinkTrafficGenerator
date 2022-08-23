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
#define TL_SERDES_SIZE_SIZE       3
#define TL_SERDES_SOURCE_SIZE     13
#define TL_SERDES_ADDRESS_SIZE    32
#define TL_SERDES_DATA_SIZE       64
#define TL_SERDES_CORRUPT_SIZE    1
#define TL_SERDES_MASK_SIZE       8
#define TL_SERDES_LAST_SIZE       1

extern UART_HandleTypeDef huart2;


uint8_t counter;



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


void TL_Transmit() {

  // @(posedge)

  // raise ready and valid
  HAL_GPIO_WritePin(TL_MOSI_Valid_GPIO_Port, TL_MOSI_Valid_Pin, 1);
  HAL_GPIO_WritePin(TL_MISO_Ready_GPIO_Port, TL_MISO_Ready_Pin, 1);

  uint64_t tl_chanid = 0;
  uint64_t tl_opcode = 0;
  uint64_t tl_param = 0;
  uint64_t tl_size = 1;
  uint64_t tl_source = 0;
  uint64_t tl_address = 0x00000000;
  uint64_t tl_data = 0x0000000010000000;
  uint64_t tl_corrupt = 0;
  uint64_t tl_mask = 0b11111111;
  uint64_t tl_last = 1;

  for (uint8_t i=0; i<TL_SERDES_LAST_SIZE; i+=1) {
    HAL_GPIO_WritePin(TL_MOSI_Data_GPIO_Port, TL_MOSI_Data_Pin, tl_last << i);
    TL_Clock();
  }
  for (uint8_t i=0; i<TL_SERDES_MASK_SIZE; i+=1) {
    HAL_GPIO_WritePin(TL_MOSI_Data_GPIO_Port, TL_MOSI_Data_Pin, tl_mask << i);
    TL_Clock();
  }
  for (uint8_t i=0; i<TL_SERDES_CORRUPT_SIZE; i+=1) {
    HAL_GPIO_WritePin(TL_MOSI_Data_GPIO_Port, TL_MOSI_Data_Pin, tl_corrupt << i);
    TL_Clock();
  }
  for (uint8_t i=0; i<TL_SERDES_DATA_SIZE; i+=1) {
    HAL_GPIO_WritePin(TL_MOSI_Data_GPIO_Port, TL_MOSI_Data_Pin, tl_data << i);
    TL_Clock();
  }
  for (uint8_t i=0; i<TL_SERDES_ADDRESS_SIZE; i+=1) {
    HAL_GPIO_WritePin(TL_MOSI_Data_GPIO_Port, TL_MOSI_Data_Pin, tl_address << i);
    TL_Clock();
  }
  for (uint8_t i=0; i<TL_SERDES_SOURCE_SIZE; i+=1) {
    HAL_GPIO_WritePin(TL_MOSI_Data_GPIO_Port, TL_MOSI_Data_Pin, tl_source << i);
    TL_Clock();
  }
  for (uint8_t i=0; i<TL_SERDES_SIZE_SIZE; i+=1) {
    HAL_GPIO_WritePin(TL_MOSI_Data_GPIO_Port, TL_MOSI_Data_Pin, tl_size << i);
    TL_Clock();
  }
  for (uint8_t i=0; i<TL_SERDES_PARAM_SIZE; i+=1) {
    HAL_GPIO_WritePin(TL_MOSI_Data_GPIO_Port, TL_MOSI_Data_Pin, tl_param << i);
    TL_Clock();
  }
  for (uint8_t i=0; i<TL_SERDES_OPCODE_SIZE; i+=1) {
    HAL_GPIO_WritePin(TL_MOSI_Data_GPIO_Port, TL_MOSI_Data_Pin, tl_opcode << i);
    TL_Clock();
  }
  for (uint8_t i=0; i<TL_SERDES_CHANID_SIZE; i+=1) {
    HAL_GPIO_WritePin(TL_MOSI_Data_GPIO_Port, TL_MOSI_Data_Pin, tl_chanid << i);
    TL_Clock();
  }

  HAL_GPIO_WritePin(TL_MOSI_Valid_GPIO_Port, TL_MOSI_Valid_Pin, 0);


  HAL_UART_Transmit(&huart2, (uint8_t *)"waiting for slave response\r\n", strlen("waiting for slave response\r\n"), 1000);
  while (HAL_GPIO_ReadPin(TL_MISO_Valid_GPIO_Port, TL_MISO_Valid_Pin)) { }
}


void APP_init() {

  HAL_UART_Transmit(&huart2, (uint8_t *)"clocking 100 clock cycles\r\n", strlen("clocking 100 clock cycles\r\n"), 1000);

  for (uint16_t i=0; i<400; i+=1) {
    TL_Clock();
  }
}

void APP_main() {

  if (APP_getUsrButton()) {
    TL_Transmit();
  }


  APP_setLED(counter % 2);

  counter += 1;

  HAL_Delay(100);
}
