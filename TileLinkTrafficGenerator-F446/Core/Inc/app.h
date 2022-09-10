/*
 * app.h
 *
 *  Created on: Aug 22, 2022
 *      Author: TK
 */

#ifndef INC_APP_H_
#define INC_APP_H_


#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "stm32f4xx_hal.h"

#include "main.h"

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
  uint16_t tx_bit_offset;
  uint16_t tx_finished;
  uint16_t tx_pending;

  TileLinkFrame rx_frame;
  uint16_t rx_bit_offset;
  uint16_t rx_finished;
  uint16_t rx_pending;
} TL_Control;



void TL_serialize(TileLinkFrame *frame);

void TL_deserialize(TileLinkFrame *frame);

void APP_init();

void APP_main();

#endif /* INC_APP_H_ */
