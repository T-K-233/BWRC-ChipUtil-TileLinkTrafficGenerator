/*
 * app.c
 *
 *  Created on: Aug 22, 2022
 *      Author: TK
 */

#include "app.h"


#define MTIME_REG_ADDR            0x0200BFF8




#define TL_SERDES_LAST_SIZE       1
#define TL_SERDES_LAST_OFFSET     (TL_SERDES_LAST_SIZE)
#define TL_SERDES_CORRUPT_SIZE    1
#define TL_SERDES_CORRUPT_OFFSET  (TL_SERDES_LAST_OFFSET + TL_SERDES_CORRUPT_SIZE)
#define TL_SERDES_DATA_SIZE       64
#define TL_SERDES_DATA_OFFSET     (TL_SERDES_CORRUPT_OFFSET + TL_SERDES_DATA_SIZE)
#define TL_SERDES_MASK_SIZE       8
#define TL_SERDES_MASK_OFFSET     (TL_SERDES_DATA_OFFSET + TL_SERDES_MASK_SIZE)
#define TL_SERDES_ADDRESS_SIZE    32
#define TL_SERDES_ADDRESS_OFFSET  (TL_SERDES_MASK_OFFSET + TL_SERDES_ADDRESS_SIZE)
#define TL_SERDES_SOURCE_SIZE     4
#define TL_SERDES_SOURCE_OFFSET   (TL_SERDES_ADDRESS_OFFSET + TL_SERDES_SOURCE_SIZE)
#define TL_SERDES_SIZE_SIZE       4
#define TL_SERDES_SIZE_OFFSET     (TL_SERDES_SOURCE_OFFSET + TL_SERDES_SIZE_SIZE)
#define TL_SERDES_PARAM_SIZE      3
#define TL_SERDES_PARAM_OFFSET    (TL_SERDES_SIZE_OFFSET + TL_SERDES_PARAM_SIZE)
#define TL_SERDES_OPCODE_SIZE     3
#define TL_SERDES_OPCODE_OFFSET   (TL_SERDES_PARAM_OFFSET + TL_SERDES_OPCODE_SIZE)
#define TL_SERDES_CHANID_SIZE     3
#define TL_SERDES_CHANID_OFFSET   (TL_SERDES_OPCODE_OFFSET + TL_SERDES_CHANID_SIZE)
#define TL_SERDES_TOTAL_SIZE      TL_SERDES_CHANID_OFFSET


extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim4;
extern UART_HandleTypeDef huart2;

TL_Control tl;
char str[128];

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
  if (tl.tx_pending) {
    HAL_GPIO_WritePin(TL_MOSI_Data_GPIO_Port, TL_MOSI_Data_Pin, tl.tx_frame.buffer[tl.tx_bit_offset]);

    if (tl.tx_bit_offset == 0) {
      HAL_GPIO_WritePin(TL_MISO_Ready_GPIO_Port, TL_MISO_Ready_Pin, 1);
      HAL_GPIO_WritePin(TL_MOSI_Valid_GPIO_Port, TL_MOSI_Valid_Pin, 1);
    }


    if (tl.tx_bit_offset == TL_SERDES_TOTAL_SIZE) {
      HAL_GPIO_WritePin(TL_MOSI_Valid_GPIO_Port, TL_MOSI_Valid_Pin, 0);
      tl.tx_pending = 0;
      tl.tx_finished = 1;
    }

    tl.tx_bit_offset += 1;
  }

  if (tl.rx_pending) {
    if (tl.rx_finished) {
      HAL_GPIO_WritePin(TL_MISO_Ready_GPIO_Port, TL_MISO_Ready_Pin, 0);
      tl.rx_pending = 0;
    }
    if (HAL_GPIO_ReadPin(TL_MISO_Valid_GPIO_Port, TL_MISO_Valid_Pin) == GPIO_PIN_SET) {
      tl.rx_frame.buffer[tl.rx_bit_offset] = HAL_GPIO_ReadPin(TL_MISO_Data_GPIO_Port, TL_MISO_Data_Pin);

      tl.rx_bit_offset += 1;

      if (tl.rx_bit_offset == TL_SERDES_TOTAL_SIZE) {
        tl.rx_finished = 1;
      }
    }
  }
}

void TL_serialize(TileLinkFrame *frame) {
  for (uint16_t i=0; i<TL_SERDES_LAST_SIZE; i+=1) {
    frame->buffer[i] = (frame->last >> i) & 0b1;
  }
  for (uint16_t i=0; i<TL_SERDES_CORRUPT_SIZE; i+=1) {
    frame->buffer[i+TL_SERDES_LAST_OFFSET] = (frame->corrupt >> i) & 0b1;
  }
  for (uint16_t i=0; i<TL_SERDES_DATA_SIZE; i+=1) {
    frame->buffer[i+TL_SERDES_CORRUPT_OFFSET] = (frame->data >> i) & 0b1;
  }
  for (uint16_t i=0; i<TL_SERDES_MASK_SIZE; i+=1) {
    frame->buffer[i+TL_SERDES_DATA_OFFSET] = (frame->mask >> i) & 0b1;
  }
  for (uint16_t i=0; i<TL_SERDES_ADDRESS_SIZE; i+=1) {
    frame->buffer[i+TL_SERDES_MASK_OFFSET] = (frame->address >> i) & 0b1;
  }
  for (uint16_t i=0; i<TL_SERDES_SOURCE_SIZE; i+=1) {
    frame->buffer[i+TL_SERDES_ADDRESS_OFFSET] = (frame->source >> i) & 0b1;
  }
  for (uint16_t i=0; i<TL_SERDES_SIZE_SIZE; i+=1) {
    frame->buffer[i+TL_SERDES_SOURCE_OFFSET] = (frame->size >> i) & 0b1;
  }
  for (uint16_t i=0; i<TL_SERDES_PARAM_SIZE; i+=1) {
    frame->buffer[i+TL_SERDES_SIZE_OFFSET] = (frame->param >> i) & 0b1;
  }
  for (uint16_t i=0; i<TL_SERDES_OPCODE_SIZE; i+=1) {
    frame->buffer[i+TL_SERDES_PARAM_OFFSET] = (frame->opcode >> i) & 0b1;
  }
  for (uint16_t i=0; i<TL_SERDES_CHANID_SIZE; i+=1) {
    frame->buffer[i+TL_SERDES_OPCODE_OFFSET] = (frame->chanid >> i) & 0b1;
  }
}

void TL_deserialize(TileLinkFrame *frame) {
  frame->chanid = 0;
  frame->opcode = 0;
  frame->param = 0;
  frame->size = 0;
  frame->source = 0;
  frame->address = 0;
  frame->data = 0;
  frame->corrupt = 0;
  frame->mask = 0;
  frame->last = 0;

  for (uint16_t i=0; i<TL_SERDES_LAST_SIZE; i+=1) {
    frame->last |= ((frame->buffer[i] & 0b1) << i);
  }
  for (uint16_t i=0; i<TL_SERDES_CORRUPT_SIZE; i+=1) {
    frame->corrupt |= ((frame->buffer[i+TL_SERDES_LAST_OFFSET] & 0b1) << i);
  }
  for (uint16_t i=0; i<TL_SERDES_DATA_SIZE; i+=1) {
    frame->data |= ((frame->buffer[i+TL_SERDES_CORRUPT_OFFSET] & 0b1) << i);
  }
  for (uint16_t i=0; i<TL_SERDES_MASK_SIZE; i+=1) {
    frame->mask |= ((frame->buffer[i+TL_SERDES_DATA_OFFSET] & 0b1) << i);
  }
  for (uint16_t i=0; i<TL_SERDES_ADDRESS_SIZE; i+=1) {
    frame->address |= ((frame->buffer[i+TL_SERDES_MASK_OFFSET] & 0b1) << i);
  }
  for (uint16_t i=0; i<TL_SERDES_SOURCE_SIZE; i+=1) {
    frame->source |= ((frame->buffer[i+TL_SERDES_ADDRESS_OFFSET] & 0b1) << i);
  }
  for (uint16_t i=0; i<TL_SERDES_SIZE_SIZE; i+=1) {
    frame->size |= ((frame->buffer[i+TL_SERDES_SOURCE_OFFSET] & 0b1) << i);
  }
  for (uint16_t i=0; i<TL_SERDES_PARAM_SIZE; i+=1) {
    frame->param |= ((frame->buffer[i+TL_SERDES_SIZE_OFFSET] & 0b1) << i);
  }
  for (uint16_t i=0; i<TL_SERDES_OPCODE_SIZE; i+=1) {
    frame->opcode |= ((frame->buffer[i+TL_SERDES_PARAM_OFFSET] & 0b1) << i);
  }
  for (uint16_t i=0; i<TL_SERDES_CHANID_SIZE; i+=1) {
    frame->chanid |= ((frame->buffer[i+TL_SERDES_OPCODE_OFFSET] & 0b1) << i);
  }
}

void TL_transmit(TileLinkFrame *frame) {
  TL_serialize(frame);

  // reset state
  tl.tx_bit_offset = 0;
  tl.rx_bit_offset = 0;
  tl.tx_finished = 0;
  tl.rx_finished = 0;

  // enable TX RX
  tl.rx_pending = 1;
  tl.tx_pending = 1;
}

//TL_Control *tl
void TL_issueGet(uint32_t address) {
  tl.tx_frame.chanid  = 0;
  tl.tx_frame.opcode  = 0x4;  // get
  tl.tx_frame.param   = 0;
  tl.tx_frame.size    = 2;
  tl.tx_frame.source  = 0;
  tl.tx_frame.address = address;
  tl.tx_frame.data    = 0x0000000000000000;
  tl.tx_frame.corrupt = 0;
  tl.tx_frame.mask    = 0b00001111;
  tl.tx_frame.last    = 1;

  TL_transmit(&tl.tx_frame);

  while (!tl.rx_finished) {}

  TL_deserialize(&tl.rx_frame);

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

void APP_main() {
  uint8_t cmd;
  if (HAL_UART_Receive(&huart2, &cmd, 1, 1000) == HAL_OK) {
    if (cmd == 't') {
      TL_issueGet(MTIME_REG_ADDR);

      sprintf(str, "[TX] GET address: 0x%08lx size: %d\r\n", tl.tx_frame.address, tl.tx_frame.size);
      HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen(str), 1000);

      sprintf(str, "[RX] chanid: %d opcode: %d size: %d source: 0x%08lx denied: %d corrupt: %d  data: 0x%08lx%08lx\r\n", tl.rx_frame.chanid, tl.rx_frame.opcode, tl.rx_frame.size, tl.rx_frame.address, tl.rx_frame.mask, tl.rx_frame.corrupt, (uint32_t)(tl.rx_frame.data >> 32), (uint32_t)tl.rx_frame.data);
      HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen(str), 1000);
    }
    if (cmd == 'r') {

      TL_issueGet(0x10000);

      sprintf(str, "[TX] GET address: 0x%08lx size: %d\r\n", tl.tx_frame.address, tl.tx_frame.size);
      HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen(str), 1000);

      sprintf(str, "[RX] chanid: %d opcode: %d size: %d source: 0x%08lx denied: %d corrupt: %d  data: 0x%08lx%08lx\r\n", tl.rx_frame.chanid, tl.rx_frame.opcode, tl.rx_frame.size, tl.rx_frame.address, tl.rx_frame.mask, tl.rx_frame.corrupt, (uint32_t)(tl.rx_frame.data >> 32), (uint32_t)tl.rx_frame.data);
      HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen(str), 1000);

    }
  }

  HAL_Delay(50);
}
