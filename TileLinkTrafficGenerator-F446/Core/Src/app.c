/*
 * app.c
 *
 *  Created on: Aug 22, 2022
 *      Author: TK
 */

#include "app.h"



#define TL_SERDES_TOTAL_SIZE      (TL_SERDES_CHANID_SIZE+TL_SERDES_OPCODE_SIZE+TL_SERDES_PARAM_SIZE \
                                  +TL_SERDES_SIZE_SIZE+TL_SERDES_SOURCE_SIZE+TL_SERDES_ADDRESS_SIZE+TL_SERDES_MASK_SIZE \
                                  +TL_SERDES_DATA_SIZE+TL_SERDES_CORRUPT_SIZE+TL_SERDES_LAST_SIZE)

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



extern TIM_HandleTypeDef htim4;
extern UART_HandleTypeDef huart2;




typedef struct {
  uint64_t chanid;
  uint64_t opcode;
  uint64_t param;
  uint64_t size;
  uint64_t source;
  uint64_t address;
  uint64_t data;
  uint64_t corrupt;
  uint64_t mask;
  uint64_t last;
} TileLinkFrame;



uint16_t tx_bit_offset = 0;

uint16_t rx_bit_offset = 0;

uint8_t tx_enabled = 0;
uint8_t tl_tx_buffer[130];
uint8_t tl_rx_buffer[130];


TileLinkFrame tx_frame;
TileLinkFrame rx_frame;

uint8_t rx_finished = 0;
uint8_t rx_pending = 0;


uint8_t TL_serialize(TileLinkFrame *frame, uint16_t offset) {
  if (offset < TL_SERDES_LAST_OFFSET) {
    return (tx_frame.last >> (offset)) & 0b1;
  }
  if (offset < TL_SERDES_CORRUPT_OFFSET) {
    return (tx_frame.corrupt >> (offset - TL_SERDES_LAST_OFFSET)) & 0b1;
  }
  if (offset < TL_SERDES_DATA_OFFSET) {
    return (tx_frame.data >> (offset - TL_SERDES_CORRUPT_OFFSET)) & 0b1;
  }
  if (offset < TL_SERDES_MASK_OFFSET) {
    return (tx_frame.mask >> (offset - TL_SERDES_DATA_OFFSET)) & 0b1;
  }
  if (offset < TL_SERDES_ADDRESS_OFFSET) {
    return (tx_frame.address >> (offset - TL_SERDES_MASK_OFFSET)) & 0b1;
  }
  if (offset < TL_SERDES_SOURCE_OFFSET) {
    return (tx_frame.source >> (offset - TL_SERDES_ADDRESS_OFFSET)) & 0b1;
  }
  if (offset < TL_SERDES_SIZE_OFFSET) {
    return (tx_frame.size >> (offset - TL_SERDES_SOURCE_OFFSET)) & 0b1;
  }
  if (offset < TL_SERDES_PARAM_OFFSET) {
    return (tx_frame.param >> (offset - TL_SERDES_SIZE_OFFSET)) & 0b1;
  }
  if (offset < TL_SERDES_OPCODE_OFFSET) {
    return (tx_frame.opcode >> (offset - TL_SERDES_PARAM_OFFSET)) & 0b1;
  }
  if (offset < TL_SERDES_CHANID_OFFSET) {
    return (tx_frame.chanid >> (offset - TL_SERDES_OPCODE_OFFSET)) & 0b1;
  }
  return 0;
}

uint8_t TL_deserialize(TileLinkFrame *frame, uint16_t offset, uint8_t val) {
  if (offset < TL_SERDES_LAST_OFFSET) {
    rx_frame.last |= (val << (offset));
    return 0;
  }
  if (offset < TL_SERDES_CORRUPT_OFFSET) {
    rx_frame.corrupt |= (val << (offset - TL_SERDES_LAST_OFFSET));
    return 0;
  }
  if (offset < TL_SERDES_DATA_OFFSET) {
    rx_frame.data |= (val << (offset - TL_SERDES_CORRUPT_OFFSET));
    return 0;
  }
  if (offset < TL_SERDES_MASK_OFFSET) {
    rx_frame.mask |= (val << (offset - TL_SERDES_DATA_OFFSET));
    return 0;
  }
  if (offset < TL_SERDES_ADDRESS_OFFSET) {
    rx_frame.address |= (val << (offset - TL_SERDES_MASK_OFFSET));
    return 0;
  }
  if (offset < TL_SERDES_SOURCE_OFFSET) {
    rx_frame.source |= (val << (offset - TL_SERDES_ADDRESS_OFFSET));
    return 0;
  }
  if (offset < TL_SERDES_SIZE_OFFSET) {
    rx_frame.size |= (val << (offset - TL_SERDES_SOURCE_OFFSET));
    return 0;
  }
  if (offset < TL_SERDES_PARAM_OFFSET) {
    rx_frame.param |= (val << (offset - TL_SERDES_SIZE_OFFSET));
    return 0;
  }
  if (offset < TL_SERDES_OPCODE_OFFSET) {
    rx_frame.opcode |= (val << (offset - TL_SERDES_PARAM_OFFSET));
    return 0;
  }
  if (offset < TL_SERDES_CHANID_OFFSET) {
    rx_frame.chanid |= (val << (offset - TL_SERDES_OPCODE_OFFSET));

    return offset == TL_SERDES_CHANID_OFFSET-1;
//    return 0;
  };
  return 1;
}




void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
  if (tx_enabled) {
    HAL_GPIO_WritePin(TL_MOSI_Data_GPIO_Port, TL_MOSI_Data_Pin, TL_serialize(&tx_frame, tx_bit_offset));

    if (tx_bit_offset == 0) {
      HAL_GPIO_WritePin(TL_MISO_Ready_GPIO_Port, TL_MISO_Ready_Pin, 1);
      HAL_GPIO_WritePin(TL_MOSI_Valid_GPIO_Port, TL_MOSI_Valid_Pin, 1);
    }

    if (tx_bit_offset == TL_SERDES_TOTAL_SIZE) {
      HAL_GPIO_WritePin(TL_MOSI_Valid_GPIO_Port, TL_MOSI_Valid_Pin, 0);
      HAL_GPIO_WritePin(TL_MOSI_Data_GPIO_Port, TL_MOSI_Data_Pin, 0);
      tx_enabled = 0;
      tx_bit_offset = 0;
    }
    else {
      tx_bit_offset += 1;
    }
  }

  if (HAL_GPIO_ReadPin(TL_MISO_Valid_GPIO_Port, TL_MISO_Valid_Pin) == GPIO_PIN_SET) {
    if (!rx_pending) {
      rx_bit_offset = 0;
      rx_pending = 1;

      rx_frame.chanid  = 0;
      rx_frame.opcode  = 0;
      rx_frame.param   = 0;
      rx_frame.size    = 0;
      rx_frame.source  = 0;
      rx_frame.address = 0;
      rx_frame.data    = 0;
      rx_frame.corrupt = 0;
      rx_frame.mask    = 0;
      rx_frame.last    = 0;
    }

    uint8_t val = HAL_GPIO_ReadPin(TL_MISO_Data_GPIO_Port, TL_MISO_Data_Pin);
    if (TL_deserialize(&rx_frame, rx_bit_offset, val)) {
      rx_bit_offset = 0;
      rx_finished = 1;
    }
    else {
      rx_bit_offset += 1;
    }
  }
}

void TL_decode(TileLinkFrame *frame) {
//  uint16_t offset = 0;
//
//  frame->chanid = 0;
//  frame->opcode = 0;
//  frame->param = 0;
//  frame->size = 0;
//  frame->source = 0;
//  frame->address = 0;
//  frame->data = 0;
//  frame->corrupt = 0;
//  frame->mask = 0;
//  frame->last = 0;
//
//  for (uint8_t i=0; i<TL_SERDES_CHANID_SIZE; i+=1) {
//    frame->chanid |= (tl_rx_buffer[offset+i] << (TL_SERDES_CHANID_SIZE-i-1));
//  }
//  offset += TL_SERDES_CHANID_SIZE;
//
//  for (uint8_t i=0; i<TL_SERDES_OPCODE_SIZE; i+=1) {
//    frame->opcode |= (tl_rx_buffer[offset+i] << (TL_SERDES_OPCODE_SIZE-i-1));
//  }
//  offset += TL_SERDES_OPCODE_SIZE;
//
//  for (uint8_t i=0; i<TL_SERDES_PARAM_SIZE; i+=1) {
//    frame->tl_param |= (tl_rx_buffer[offset+i] << (TL_SERDES_PARAM_SIZE-i-1));
//  }
//  offset += TL_SERDES_PARAM_SIZE;
//
//  for (uint8_t i=0; i<TL_SERDES_SIZE_SIZE; i+=1) {
//    frame->tl_size |= (tl_rx_buffer[offset+i] << (TL_SERDES_SIZE_SIZE-i-1));
//  }
//  offset += TL_SERDES_SIZE_SIZE;
//
//  for (uint8_t i=0; i<TL_SERDES_SOURCE_SIZE; i+=1) {
//    frame->tl_source |= (tl_rx_buffer[offset+i] << (TL_SERDES_SOURCE_SIZE-i-1));
//  }
//  offset += TL_SERDES_SOURCE_SIZE;
//
//  for (uint8_t i=0; i<TL_SERDES_ADDRESS_SIZE; i+=1) {
//    frame->tl_address |= (tl_rx_buffer[offset+i] << (TL_SERDES_ADDRESS_SIZE-i-1));
//  }
//  offset += TL_SERDES_ADDRESS_SIZE;
//
//  for (uint8_t i=0; i<TL_SERDES_MASK_SIZE; i+=1) {
//    frame->tl_mask |= (tl_rx_buffer[offset+i] << (TL_SERDES_MASK_SIZE-i-1));
//  }
//  offset += TL_SERDES_MASK_SIZE;
//
//  for (uint8_t i=0; i<TL_SERDES_DATA_SIZE; i+=1) {
//    frame->tl_data |= (tl_rx_buffer[offset+i] << (TL_SERDES_DATA_SIZE-i-1));
//  }
//  offset += TL_SERDES_DATA_SIZE;
//
//  for (uint8_t i=0; i<TL_SERDES_CORRUPT_SIZE; i+=1) {
//    frame->tl_corrupt |= (tl_rx_buffer[offset+i] << (TL_SERDES_CORRUPT_SIZE-i-1));
//  }
//  offset += TL_SERDES_CORRUPT_SIZE;
//
//  for (uint8_t i=0; i<TL_SERDES_LAST_SIZE; i+=1) {
//    frame->tl_last |= (tl_rx_buffer[offset+i] << (TL_SERDES_LAST_SIZE-i-1));
//  }
//  offset += TL_SERDES_LAST_SIZE;
}


void TL_startTransmit() {
  uint16_t offset = 0;

  uint64_t tl_chanid  = 0;
  uint64_t tl_opcode  = 0;// get  //0;// putfull
  uint64_t tl_param   = 0;
  uint64_t tl_size    = 2;
  uint64_t tl_source  = 0;
  uint64_t tl_address = 0x80000000;
  uint64_t tl_data    = 0x0000000000000001;
  uint64_t tl_corrupt = 0;
  uint64_t tl_mask    = 0b00001111;
  uint64_t tl_last    = 1;

  for (uint8_t i=0; i<TL_SERDES_CHANID_SIZE; i+=1) {
    tl_tx_buffer[offset+i] = (tl_chanid >> (TL_SERDES_CHANID_SIZE-i-1)) & 0b1;
  }
  offset += TL_SERDES_CHANID_SIZE;

  for (uint8_t i=0; i<TL_SERDES_OPCODE_SIZE; i+=1) {
    tl_tx_buffer[offset+i] = (tl_opcode >> (TL_SERDES_OPCODE_SIZE-i-1)) & 0b1;
  }
  offset += TL_SERDES_OPCODE_SIZE;

  for (uint8_t i=0; i<TL_SERDES_PARAM_SIZE; i+=1) {
    tl_tx_buffer[offset+i] = (tl_param >> (TL_SERDES_PARAM_SIZE-i-1)) & 0b1;
  }
  offset += TL_SERDES_PARAM_SIZE;

  for (uint8_t i=0; i<TL_SERDES_SIZE_SIZE; i+=1) {
    tl_tx_buffer[offset+i] = (tl_size >> (TL_SERDES_SIZE_SIZE-i-1)) & 0b1;
  }
  offset += TL_SERDES_SIZE_SIZE;

  for (uint8_t i=0; i<TL_SERDES_SOURCE_SIZE; i+=1) {
    tl_tx_buffer[offset+i] = (tl_source >> (TL_SERDES_SOURCE_SIZE-i-1)) & 0b1;
  }
  offset += TL_SERDES_SOURCE_SIZE;

  for (uint8_t i=0; i<TL_SERDES_ADDRESS_SIZE; i+=1) {
    tl_tx_buffer[offset+i] = (tl_address >> (TL_SERDES_ADDRESS_SIZE-i-1)) & 0b1;
  }
  offset += TL_SERDES_ADDRESS_SIZE;

  for (uint8_t i=0; i<TL_SERDES_MASK_SIZE; i+=1) {
    tl_tx_buffer[offset+i] = (tl_mask >> (TL_SERDES_MASK_SIZE-i-1)) & 0b1;
  }
  offset += TL_SERDES_MASK_SIZE;

  for (uint8_t i=0; i<TL_SERDES_DATA_SIZE; i+=1) {
    tl_tx_buffer[offset+i] = (tl_data >> (TL_SERDES_DATA_SIZE-i-1)) & 0b1;
  }
  offset += TL_SERDES_DATA_SIZE;

  for (uint8_t i=0; i<TL_SERDES_CORRUPT_SIZE; i+=1) {
    tl_tx_buffer[offset+i] = (tl_corrupt >> (TL_SERDES_CORRUPT_SIZE-i-1)) & 0b1;
  }
  offset += TL_SERDES_CORRUPT_SIZE;

  for (uint8_t i=0; i<TL_SERDES_LAST_SIZE; i+=1) {
    tl_tx_buffer[offset+i] = (tl_last >> (TL_SERDES_LAST_SIZE-i-1)) & 0b1;
  }
  offset += TL_SERDES_LAST_SIZE;


//  tx_offset = 0;
//  tx_enabled = 1;
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
  uint8_t cmd;
  if (HAL_UART_Receive(&huart2, &cmd, 1, 1000) == HAL_OK) {
    if (cmd == 'w') {
      tx_frame.chanid  = 0;
      tx_frame.opcode  = 0;// get  //0;// putfull
      tx_frame.param   = 0;
      tx_frame.size    = 2;
      tx_frame.source  = 0;
      tx_frame.address = 0x80000000;
      tx_frame.data    = 0x0000000000000001;
      tx_frame.corrupt = 0;
      tx_frame.mask    = 0b00001111;
      tx_frame.last    = 1;

      tx_bit_offset = 0;
      tx_enabled = 1;
      HAL_Delay(50);
    }
    if (cmd == 'r') {
      tx_frame.chanid  = 0;
      tx_frame.opcode  = 4;// get  //0;// putfull
      tx_frame.param   = 0;
      tx_frame.size    = 2;
      tx_frame.source  = 0;
      tx_frame.address = 0x80000000;
      tx_frame.data    = 0x0000000000000001;
      tx_frame.corrupt = 0;
      tx_frame.mask    = 0b00001111;
      tx_frame.last    = 1;

      tx_bit_offset = 0;
      tx_enabled = 1;
      HAL_Delay(50);
    }
  }

  if (rx_finished) {
    char str[128];

    sprintf(str, "[RX] chanid: %d opcode: %d size: %d address: %x data: %x\r\n", rx_frame.chanid, rx_frame.opcode, rx_frame.size, rx_frame.address, rx_frame.data);
    HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen(str), 1000);
    rx_finished = 0;
  }

  HAL_Delay(50);
}
