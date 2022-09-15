/*
 * tilelink.h
 *
 *  Created on: Sep 9, 2022
 *      Author: TK
 */

#ifndef INC_TILELINK_H_
#define INC_TILELINK_H_

#include "stm32f4xx_hal.h"
#include "main.h"

#define TL_CH_A_OPCODE_GET        0x04
#define TL_CH_A_OPCODE_PULFULLDATA        0x00

#define TL_SERDES_LAST_SIZE       1
#define TL_SERDES_LAST_OFFSET     (TL_SERDES_LAST_SIZE)
#define TL_SERDES_MASK_SIZE       8
#define TL_SERDES_MASK_OFFSET     (TL_SERDES_LAST_OFFSET + TL_SERDES_MASK_SIZE)
#define TL_SERDES_CORRUPT_SIZE    1
#define TL_SERDES_CORRUPT_OFFSET  (TL_SERDES_MASK_OFFSET + TL_SERDES_CORRUPT_SIZE)
#define TL_SERDES_DATA_SIZE       64
#define TL_SERDES_DATA_OFFSET     (TL_SERDES_CORRUPT_OFFSET + TL_SERDES_DATA_SIZE)
#define TL_SERDES_ADDRESS_SIZE    32
#define TL_SERDES_ADDRESS_OFFSET  (TL_SERDES_DATA_OFFSET + TL_SERDES_ADDRESS_SIZE)
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


typedef struct {
  uint8_t chanid;
  uint8_t opcode;
  uint8_t param;
  uint8_t size;
  uint8_t source;
  uint32_t address;
  uint64_t data;
  uint8_t corrupt;
  uint8_t mask;
  uint8_t last;
  uint8_t buffer[256];
} TileLinkFrame;

typedef struct {
  TileLinkFrame tx_frame;
  TileLinkFrame rx_frame;

  uint16_t tx_bit_offset;
  uint16_t tx_finished;
  uint16_t tx_pending;

  uint16_t rx_bit_offset;
  uint16_t rx_finished;
  uint16_t rx_pending;
} TileLinkController;

void TL_update(TileLinkController *tl);

void TL_serialize(TileLinkFrame *frame);

void TL_deserialize(TileLinkFrame *frame);

void TL_transmit(TileLinkController *tl);

void TL_GET(TileLinkController *tl, uint32_t address);

void TL_PUTFULLDATA(TileLinkController *tl, uint32_t address, uint64_t data);

#endif /* INC_TILELINK_H_ */
