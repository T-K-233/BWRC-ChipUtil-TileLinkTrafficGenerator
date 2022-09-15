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

#define GPIO_BASE_ADDR              0x10012000


extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim4;
extern UART_HandleTypeDef huart2;

TileLinkController tl;

char str[128];

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
  TL_update(&tl);
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
//  uint8_t cmd;
//  if (HAL_UART_Receive(&huart2, &cmd, 1, 1000) == HAL_OK) {
//    switch (cmd) {
//      case 'b':
//        GET(BOOT_SELECT_ADDR);
//        break;
//      case 'B':
//        for (uint8_t i=0; i<0x40; i+=4) {
//          GET(BOOTROM_BASE_ADDR+i);
//        }
//        break;
//
//      case 'i':
//        GET(CLINT_MSIP_ADDR);
//        break;
//
//      case 't':
//        while (1) {
//          GET(CLINT_MTIME_ADDR);
//          HAL_Delay(1000);
//        }
//        break;
//
//      case 'U':
//        PUTFULL(UART_TXCTRL_ADDR, 0x1);
//        PUTFULL(UART_TXDATA_ADDR, 0xDEADBEEF);
//        break;
//
//
//      case 'd':
//        for (uint16_t i=0; i<0x40; i+=4) {
//          GET(DTIM_BASE_ADDR+i);
//        }
//        break;
//      case 'D':
//        GET(DTIM_BASE_ADDR);
//        break;
//
//      case 'P':
//
//        PUTFULL(DTIM_BASE_ADDR + 0, 0x93);
//        PUTFULL(DTIM_BASE_ADDR + 4, 0x113);
//        PUTFULL(DTIM_BASE_ADDR + 8, 0x193);
//        PUTFULL(DTIM_BASE_ADDR + 12, 0x213);
//        PUTFULL(DTIM_BASE_ADDR + 16, 0x293);
//        PUTFULL(DTIM_BASE_ADDR + 20, 0x313);
//        PUTFULL(DTIM_BASE_ADDR + 24, 0x393);
//        PUTFULL(DTIM_BASE_ADDR + 28, 0x413);
//        PUTFULL(DTIM_BASE_ADDR + 32, 0x493);
//        PUTFULL(DTIM_BASE_ADDR + 36, 0x513);
//        PUTFULL(DTIM_BASE_ADDR + 40, 0x593);
//        PUTFULL(DTIM_BASE_ADDR + 44, 0x613);
//        PUTFULL(DTIM_BASE_ADDR + 48, 0x693);
//        PUTFULL(DTIM_BASE_ADDR + 52, 0x713);
//        PUTFULL(DTIM_BASE_ADDR + 56, 0x793);
//        PUTFULL(DTIM_BASE_ADDR + 60, 0x813);
//        PUTFULL(DTIM_BASE_ADDR + 64, 0x893);
//        PUTFULL(DTIM_BASE_ADDR + 68, 0x913);
//        PUTFULL(DTIM_BASE_ADDR + 72, 0x993);
//        PUTFULL(DTIM_BASE_ADDR + 76, 0xa13);
//        PUTFULL(DTIM_BASE_ADDR + 80, 0xa93);
//        PUTFULL(DTIM_BASE_ADDR + 84, 0xb13);
//        PUTFULL(DTIM_BASE_ADDR + 88, 0xb93);
//        PUTFULL(DTIM_BASE_ADDR + 92, 0xc13);
//        PUTFULL(DTIM_BASE_ADDR + 96, 0xc93);
//        PUTFULL(DTIM_BASE_ADDR + 100, 0xd13);
//        PUTFULL(DTIM_BASE_ADDR + 104, 0xd93);
//        PUTFULL(DTIM_BASE_ADDR + 108, 0xe13);
//        PUTFULL(DTIM_BASE_ADDR + 112, 0xe93);
//        PUTFULL(DTIM_BASE_ADDR + 116, 0xf13);
//        PUTFULL(DTIM_BASE_ADDR + 120, 0xf93);
//        PUTFULL(DTIM_BASE_ADDR + 124, 0x800002b7);
//        PUTFULL(DTIM_BASE_ADDR + 128, 0x30529073);
//        PUTFULL(DTIM_BASE_ADDR + 132, 0x1197);
//        PUTFULL(DTIM_BASE_ADDR + 136, 0xd3818193);
//        PUTFULL(DTIM_BASE_ADDR + 140, 0x80004137);
//        PUTFULL(DTIM_BASE_ADDR + 144, 0xf1402573);
//        PUTFULL(DTIM_BASE_ADDR + 148, 0x100593);
//        PUTFULL(DTIM_BASE_ADDR + 152, 0x8000ef);
//        PUTFULL(DTIM_BASE_ADDR + 156, 0x6f);
//        PUTFULL(DTIM_BASE_ADDR + 160, 0xfe010113);
//        PUTFULL(DTIM_BASE_ADDR + 164, 0x112e23);
//        PUTFULL(DTIM_BASE_ADDR + 168, 0x812c23);
//        PUTFULL(DTIM_BASE_ADDR + 172, 0x2010413);
//        PUTFULL(DTIM_BASE_ADDR + 176, 0xe0000ef);
//        PUTFULL(DTIM_BASE_ADDR + 180, 0x54000537);
//        PUTFULL(DTIM_BASE_ADDR + 184, 0x33c000ef);
//        PUTFULL(DTIM_BASE_ADDR + 188, 0x100593);
//        PUTFULL(DTIM_BASE_ADDR + 192, 0x10012537);
//        PUTFULL(DTIM_BASE_ADDR + 196, 0xe8000ef);
//        PUTFULL(DTIM_BASE_ADDR + 200, 0x797);
//        PUTFULL(DTIM_BASE_ADDR + 204, 0x4f478793);
//        PUTFULL(DTIM_BASE_ADDR + 208, 0x7a603);
//        PUTFULL(DTIM_BASE_ADDR + 212, 0x47a683);
//        PUTFULL(DTIM_BASE_ADDR + 216, 0x87a703);
//        PUTFULL(DTIM_BASE_ADDR + 220, 0xfec42023);
//        PUTFULL(DTIM_BASE_ADDR + 224, 0xfed42223);
//        PUTFULL(DTIM_BASE_ADDR + 228, 0xfee42423);
//        PUTFULL(DTIM_BASE_ADDR + 232, 0xc7c783);
//        PUTFULL(DTIM_BASE_ADDR + 236, 0xfef40623);
//        PUTFULL(DTIM_BASE_ADDR + 240, 0xfe040793);
//        PUTFULL(DTIM_BASE_ADDR + 244, 0x693);
//        PUTFULL(DTIM_BASE_ADDR + 248, 0xc00613);
//        PUTFULL(DTIM_BASE_ADDR + 252, 0x78593);
//        PUTFULL(DTIM_BASE_ADDR + 256, 0x54000537);
//        PUTFULL(DTIM_BASE_ADDR + 260, 0x430000ef);
//        PUTFULL(DTIM_BASE_ADDR + 264, 0x100613);
//        PUTFULL(DTIM_BASE_ADDR + 268, 0x100593);
//        PUTFULL(DTIM_BASE_ADDR + 272, 0x10012537);
//        PUTFULL(DTIM_BASE_ADDR + 276, 0x110000ef);
//        PUTFULL(DTIM_BASE_ADDR + 280, 0xfa00513);
//        PUTFULL(DTIM_BASE_ADDR + 284, 0x593);
//        PUTFULL(DTIM_BASE_ADDR + 288, 0x1c0000ef);
//        PUTFULL(DTIM_BASE_ADDR + 292, 0x613);
//        PUTFULL(DTIM_BASE_ADDR + 296, 0x100593);
//        PUTFULL(DTIM_BASE_ADDR + 300, 0x10012537);
//        PUTFULL(DTIM_BASE_ADDR + 304, 0xf4000ef);
//        PUTFULL(DTIM_BASE_ADDR + 308, 0xfa00513);
//        PUTFULL(DTIM_BASE_ADDR + 312, 0x593);
//        PUTFULL(DTIM_BASE_ADDR + 316, 0x1a4000ef);
//        PUTFULL(DTIM_BASE_ADDR + 320, 0x13);
//        PUTFULL(DTIM_BASE_ADDR + 324, 0xf85ff06f);
//        PUTFULL(DTIM_BASE_ADDR + 328, 0xff010113);
//        PUTFULL(DTIM_BASE_ADDR + 332, 0x812623);
//        PUTFULL(DTIM_BASE_ADDR + 336, 0x1010413);
//        PUTFULL(DTIM_BASE_ADDR + 340, 0x13);
//        PUTFULL(DTIM_BASE_ADDR + 344, 0xc12403);
//        PUTFULL(DTIM_BASE_ADDR + 348, 0x1010113);
//        PUTFULL(DTIM_BASE_ADDR + 352, 0x8067);
//        PUTFULL(DTIM_BASE_ADDR + 356, 0xfe010113);
//        PUTFULL(DTIM_BASE_ADDR + 360, 0x812e23);
//        PUTFULL(DTIM_BASE_ADDR + 364, 0x2010413);
//        PUTFULL(DTIM_BASE_ADDR + 368, 0xfea42623);
//        PUTFULL(DTIM_BASE_ADDR + 372, 0xfeb42423);
//        PUTFULL(DTIM_BASE_ADDR + 376, 0xfec42223);
//        PUTFULL(DTIM_BASE_ADDR + 380, 0x13);
//        PUTFULL(DTIM_BASE_ADDR + 384, 0x78513);
//        PUTFULL(DTIM_BASE_ADDR + 388, 0x1c12403);
//        PUTFULL(DTIM_BASE_ADDR + 392, 0x2010113);
//        PUTFULL(DTIM_BASE_ADDR + 396, 0x8067);
//        PUTFULL(DTIM_BASE_ADDR + 400, 0xff010113);
//        PUTFULL(DTIM_BASE_ADDR + 404, 0x812623);
//        PUTFULL(DTIM_BASE_ADDR + 408, 0x1010413);
//        PUTFULL(DTIM_BASE_ADDR + 412, 0x13);
//        PUTFULL(DTIM_BASE_ADDR + 416, 0xc12403);
//        PUTFULL(DTIM_BASE_ADDR + 420, 0x1010113);
//        PUTFULL(DTIM_BASE_ADDR + 424, 0x8067);
//        PUTFULL(DTIM_BASE_ADDR + 428, 0xfe010113);
//        PUTFULL(DTIM_BASE_ADDR + 432, 0x812e23);
//        PUTFULL(DTIM_BASE_ADDR + 436, 0x2010413);
//        PUTFULL(DTIM_BASE_ADDR + 440, 0xfea42623);
//        PUTFULL(DTIM_BASE_ADDR + 444, 0xfeb42423);
//        PUTFULL(DTIM_BASE_ADDR + 448, 0xfec42783);
//        PUTFULL(DTIM_BASE_ADDR + 452, 0x87a703);
//        PUTFULL(DTIM_BASE_ADDR + 456, 0xfe842783);
//        PUTFULL(DTIM_BASE_ADDR + 460, 0xf76733);
//        PUTFULL(DTIM_BASE_ADDR + 464, 0xfec42783);
//        PUTFULL(DTIM_BASE_ADDR + 468, 0xe7a423);
//        PUTFULL(DTIM_BASE_ADDR + 472, 0x13);
//        PUTFULL(DTIM_BASE_ADDR + 476, 0x1c12403);
//        PUTFULL(DTIM_BASE_ADDR + 480, 0x2010113);
//        PUTFULL(DTIM_BASE_ADDR + 484, 0x8067);
//        PUTFULL(DTIM_BASE_ADDR + 488, 0xfe010113);
//        PUTFULL(DTIM_BASE_ADDR + 492, 0x812e23);
//        PUTFULL(DTIM_BASE_ADDR + 496, 0x2010413);
//        PUTFULL(DTIM_BASE_ADDR + 500, 0xfea42623);
//        PUTFULL(DTIM_BASE_ADDR + 504, 0xfeb42423);
//        PUTFULL(DTIM_BASE_ADDR + 508, 0xfec42783);
//        PUTFULL(DTIM_BASE_ADDR + 512, 0x7a703);
//        PUTFULL(DTIM_BASE_ADDR + 516, 0xfe842783);
//        PUTFULL(DTIM_BASE_ADDR + 520, 0xf777b3);
//        PUTFULL(DTIM_BASE_ADDR + 524, 0xf037b3);
//        PUTFULL(DTIM_BASE_ADDR + 528, 0xff7f793);
//        PUTFULL(DTIM_BASE_ADDR + 532, 0x78513);
//        PUTFULL(DTIM_BASE_ADDR + 536, 0x1c12403);
//        PUTFULL(DTIM_BASE_ADDR + 540, 0x2010113);
//        PUTFULL(DTIM_BASE_ADDR + 544, 0x8067);
//        PUTFULL(DTIM_BASE_ADDR + 548, 0xfe010113);
//        PUTFULL(DTIM_BASE_ADDR + 552, 0x812e23);
//        PUTFULL(DTIM_BASE_ADDR + 556, 0x2010413);
//        PUTFULL(DTIM_BASE_ADDR + 560, 0xfea42623);
//        PUTFULL(DTIM_BASE_ADDR + 564, 0xfeb42423);
//        PUTFULL(DTIM_BASE_ADDR + 568, 0x60793);
//        PUTFULL(DTIM_BASE_ADDR + 572, 0xfef403a3);
//        PUTFULL(DTIM_BASE_ADDR + 576, 0xfe744783);
//        PUTFULL(DTIM_BASE_ADDR + 580, 0x2078063);
//        PUTFULL(DTIM_BASE_ADDR + 584, 0xfec42783);
//        PUTFULL(DTIM_BASE_ADDR + 588, 0xc7a703);
//        PUTFULL(DTIM_BASE_ADDR + 592, 0xfe842783);
//        PUTFULL(DTIM_BASE_ADDR + 596, 0xf76733);
//        PUTFULL(DTIM_BASE_ADDR + 600, 0xfec42783);
//        PUTFULL(DTIM_BASE_ADDR + 604, 0xe7a623);
//        PUTFULL(DTIM_BASE_ADDR + 608, 0x200006f);
//        PUTFULL(DTIM_BASE_ADDR + 612, 0xfec42783);
//        PUTFULL(DTIM_BASE_ADDR + 616, 0xc7a703);
//        PUTFULL(DTIM_BASE_ADDR + 620, 0xfe842783);
//        PUTFULL(DTIM_BASE_ADDR + 624, 0xfff7c793);
//        PUTFULL(DTIM_BASE_ADDR + 628, 0xf77733);
//        PUTFULL(DTIM_BASE_ADDR + 632, 0xfec42783);
//        PUTFULL(DTIM_BASE_ADDR + 636, 0xe7a623);
//        PUTFULL(DTIM_BASE_ADDR + 640, 0x13);
//        PUTFULL(DTIM_BASE_ADDR + 644, 0x1c12403);
//        PUTFULL(DTIM_BASE_ADDR + 648, 0x2010113);
//        PUTFULL(DTIM_BASE_ADDR + 652, 0x8067);
//        PUTFULL(DTIM_BASE_ADDR + 656, 0xff010113);
//        PUTFULL(DTIM_BASE_ADDR + 660, 0x812623);
//        PUTFULL(DTIM_BASE_ADDR + 664, 0x1010413);
//        PUTFULL(DTIM_BASE_ADDR + 668, 0x13);
//        PUTFULL(DTIM_BASE_ADDR + 672, 0xc12403);
//        PUTFULL(DTIM_BASE_ADDR + 676, 0x1010113);
//        PUTFULL(DTIM_BASE_ADDR + 680, 0x8067);
//        PUTFULL(DTIM_BASE_ADDR + 684, 0xff010113);
//        PUTFULL(DTIM_BASE_ADDR + 688, 0x812623);
//        PUTFULL(DTIM_BASE_ADDR + 692, 0x1010413);
//        PUTFULL(DTIM_BASE_ADDR + 696, 0x2000737);
//        PUTFULL(DTIM_BASE_ADDR + 700, 0xc7b7);
//        PUTFULL(DTIM_BASE_ADDR + 704, 0xf707b3);
//        PUTFULL(DTIM_BASE_ADDR + 708, 0xff87a703);
//        PUTFULL(DTIM_BASE_ADDR + 712, 0xffc7a783);
//        PUTFULL(DTIM_BASE_ADDR + 716, 0x70513);
//        PUTFULL(DTIM_BASE_ADDR + 720, 0x78593);
//        PUTFULL(DTIM_BASE_ADDR + 724, 0xc12403);
//        PUTFULL(DTIM_BASE_ADDR + 728, 0x1010113);
//        PUTFULL(DTIM_BASE_ADDR + 732, 0x8067);
//        PUTFULL(DTIM_BASE_ADDR + 736, 0xfc010113);
//        PUTFULL(DTIM_BASE_ADDR + 740, 0x2112e23);
//        PUTFULL(DTIM_BASE_ADDR + 744, 0x2812c23);
//        PUTFULL(DTIM_BASE_ADDR + 748, 0x3212a23);
//        PUTFULL(DTIM_BASE_ADDR + 752, 0x3312823);
//        PUTFULL(DTIM_BASE_ADDR + 756, 0x3412623);
//        PUTFULL(DTIM_BASE_ADDR + 760, 0x3512423);
//        PUTFULL(DTIM_BASE_ADDR + 764, 0x4010413);
//        PUTFULL(DTIM_BASE_ADDR + 768, 0xfca42423);
//        PUTFULL(DTIM_BASE_ADDR + 772, 0xfcb42623);
//        PUTFULL(DTIM_BASE_ADDR + 776, 0xfa5ff0ef);
//        PUTFULL(DTIM_BASE_ADDR + 780, 0x50613);
//        PUTFULL(DTIM_BASE_ADDR + 784, 0x58693);
//        PUTFULL(DTIM_BASE_ADDR + 788, 0xfc842803);
//        PUTFULL(DTIM_BASE_ADDR + 792, 0xfcc42883);
//        PUTFULL(DTIM_BASE_ADDR + 796, 0x80713);
//        PUTFULL(DTIM_BASE_ADDR + 800, 0x88793);
//        PUTFULL(DTIM_BASE_ADDR + 804, 0x1f75593);
//        PUTFULL(DTIM_BASE_ADDR + 808, 0x179a93);
//        PUTFULL(DTIM_BASE_ADDR + 812, 0x155eab3);
//        PUTFULL(DTIM_BASE_ADDR + 816, 0x171a13);
//        PUTFULL(DTIM_BASE_ADDR + 820, 0xa0713);
//        PUTFULL(DTIM_BASE_ADDR + 824, 0xa8793);
//        PUTFULL(DTIM_BASE_ADDR + 828, 0x1070533);
//        PUTFULL(DTIM_BASE_ADDR + 832, 0x50313);
//        PUTFULL(DTIM_BASE_ADDR + 836, 0xe33333);
//        PUTFULL(DTIM_BASE_ADDR + 840, 0x11785b3);
//        PUTFULL(DTIM_BASE_ADDR + 844, 0xb307b3);
//        PUTFULL(DTIM_BASE_ADDR + 848, 0x78593);
//        PUTFULL(DTIM_BASE_ADDR + 852, 0x50713);
//        PUTFULL(DTIM_BASE_ADDR + 856, 0x58793);
//        PUTFULL(DTIM_BASE_ADDR + 860, 0x1e75593);
//        PUTFULL(DTIM_BASE_ADDR + 864, 0x279993);
//        PUTFULL(DTIM_BASE_ADDR + 868, 0x135e9b3);
//        PUTFULL(DTIM_BASE_ADDR + 872, 0x271913);
//        PUTFULL(DTIM_BASE_ADDR + 876, 0x90713);
//        PUTFULL(DTIM_BASE_ADDR + 880, 0x98793);
//        PUTFULL(DTIM_BASE_ADDR + 884, 0x70513);
//        PUTFULL(DTIM_BASE_ADDR + 888, 0x78593);
//        PUTFULL(DTIM_BASE_ADDR + 892, 0xa60733);
//        PUTFULL(DTIM_BASE_ADDR + 896, 0x70813);
//        PUTFULL(DTIM_BASE_ADDR + 900, 0xc83833);
//        PUTFULL(DTIM_BASE_ADDR + 904, 0xb687b3);
//        PUTFULL(DTIM_BASE_ADDR + 908, 0xf806b3);
//        PUTFULL(DTIM_BASE_ADDR + 912, 0x68793);
//        PUTFULL(DTIM_BASE_ADDR + 916, 0xfce42c23);
//        PUTFULL(DTIM_BASE_ADDR + 920, 0xfcf42e23);
//        PUTFULL(DTIM_BASE_ADDR + 924, 0x13);
//        PUTFULL(DTIM_BASE_ADDR + 928, 0xf0dff0ef);
//        PUTFULL(DTIM_BASE_ADDR + 932, 0x50713);
//        PUTFULL(DTIM_BASE_ADDR + 936, 0x58793);
//        PUTFULL(DTIM_BASE_ADDR + 940, 0xfdc42683);
//        PUTFULL(DTIM_BASE_ADDR + 944, 0x78613);
//        PUTFULL(DTIM_BASE_ADDR + 948, 0xfed666e3);
//        PUTFULL(DTIM_BASE_ADDR + 952, 0xfdc42683);
//        PUTFULL(DTIM_BASE_ADDR + 956, 0x78613);
//        PUTFULL(DTIM_BASE_ADDR + 960, 0xc69863);
//        PUTFULL(DTIM_BASE_ADDR + 964, 0xfd842683);
//        PUTFULL(DTIM_BASE_ADDR + 968, 0x70793);
//        PUTFULL(DTIM_BASE_ADDR + 972, 0xfcd7eae3);
//        PUTFULL(DTIM_BASE_ADDR + 976, 0x13);
//        PUTFULL(DTIM_BASE_ADDR + 980, 0x3c12083);
//        PUTFULL(DTIM_BASE_ADDR + 984, 0x3812403);
//        PUTFULL(DTIM_BASE_ADDR + 988, 0x3412903);
//        PUTFULL(DTIM_BASE_ADDR + 992, 0x3012983);
//        PUTFULL(DTIM_BASE_ADDR + 996, 0x2c12a03);
//        PUTFULL(DTIM_BASE_ADDR + 1000, 0x2812a83);
//        PUTFULL(DTIM_BASE_ADDR + 1004, 0x4010113);
//        PUTFULL(DTIM_BASE_ADDR + 1008, 0x8067);
//        PUTFULL(DTIM_BASE_ADDR + 1012, 0xfe010113);
//        PUTFULL(DTIM_BASE_ADDR + 1016, 0x812e23);
//        PUTFULL(DTIM_BASE_ADDR + 1020, 0x2010413);
//        PUTFULL(DTIM_BASE_ADDR + 1024, 0xfea42623);
//        PUTFULL(DTIM_BASE_ADDR + 1028, 0xfec42783);
//        PUTFULL(DTIM_BASE_ADDR + 1032, 0xc7a783);
//        PUTFULL(DTIM_BASE_ADDR + 1036, 0x17e713);
//        PUTFULL(DTIM_BASE_ADDR + 1040, 0xfec42783);
//        PUTFULL(DTIM_BASE_ADDR + 1044, 0xe7a623);
//        PUTFULL(DTIM_BASE_ADDR + 1048, 0xfec42783);
//        PUTFULL(DTIM_BASE_ADDR + 1052, 0x87a783);
//        PUTFULL(DTIM_BASE_ADDR + 1056, 0x17e713);
//        PUTFULL(DTIM_BASE_ADDR + 1060, 0xfec42783);
//        PUTFULL(DTIM_BASE_ADDR + 1064, 0xe7a423);
//        PUTFULL(DTIM_BASE_ADDR + 1068, 0x13);
//        PUTFULL(DTIM_BASE_ADDR + 1072, 0x1c12403);
//        PUTFULL(DTIM_BASE_ADDR + 1076, 0x2010113);
//        PUTFULL(DTIM_BASE_ADDR + 1080, 0x8067);
//        PUTFULL(DTIM_BASE_ADDR + 1084, 0xfe010113);
//        PUTFULL(DTIM_BASE_ADDR + 1088, 0x812e23);
//        PUTFULL(DTIM_BASE_ADDR + 1092, 0x2010413);
//        PUTFULL(DTIM_BASE_ADDR + 1096, 0xfea42623);
//        PUTFULL(DTIM_BASE_ADDR + 1100, 0xfec42783);
//        PUTFULL(DTIM_BASE_ADDR + 1104, 0xc7a783);
//        PUTFULL(DTIM_BASE_ADDR + 1108, 0x107d793);
//        PUTFULL(DTIM_BASE_ADDR + 1112, 0xff7f793);
//        PUTFULL(DTIM_BASE_ADDR + 1116, 0x77f793);
//        PUTFULL(DTIM_BASE_ADDR + 1120, 0xff7f793);
//        PUTFULL(DTIM_BASE_ADDR + 1124, 0x78513);
//        PUTFULL(DTIM_BASE_ADDR + 1128, 0x1c12403);
//        PUTFULL(DTIM_BASE_ADDR + 1132, 0x2010113);
//        PUTFULL(DTIM_BASE_ADDR + 1136, 0x8067);
//        PUTFULL(DTIM_BASE_ADDR + 1140, 0xfe010113);
//        PUTFULL(DTIM_BASE_ADDR + 1144, 0x812e23);
//        PUTFULL(DTIM_BASE_ADDR + 1148, 0x2010413);
//        PUTFULL(DTIM_BASE_ADDR + 1152, 0xfea42623);
//        PUTFULL(DTIM_BASE_ADDR + 1156, 0xfec42783);
//        PUTFULL(DTIM_BASE_ADDR + 1160, 0x87a783);
//        PUTFULL(DTIM_BASE_ADDR + 1164, 0x107d793);
//        PUTFULL(DTIM_BASE_ADDR + 1168, 0xff7f793);
//        PUTFULL(DTIM_BASE_ADDR + 1172, 0x77f793);
//        PUTFULL(DTIM_BASE_ADDR + 1176, 0xff7f793);
//        PUTFULL(DTIM_BASE_ADDR + 1180, 0x78513);
//        PUTFULL(DTIM_BASE_ADDR + 1184, 0x1c12403);
//        PUTFULL(DTIM_BASE_ADDR + 1188, 0x2010113);
//        PUTFULL(DTIM_BASE_ADDR + 1192, 0x8067);
//        PUTFULL(DTIM_BASE_ADDR + 1196, 0xfe010113);
//        PUTFULL(DTIM_BASE_ADDR + 1200, 0x112e23);
//        PUTFULL(DTIM_BASE_ADDR + 1204, 0x812c23);
//        PUTFULL(DTIM_BASE_ADDR + 1208, 0x2010413);
//        PUTFULL(DTIM_BASE_ADDR + 1212, 0xfea42623);
//        PUTFULL(DTIM_BASE_ADDR + 1216, 0xfeb42423);
//        PUTFULL(DTIM_BASE_ADDR + 1220, 0x60793);
//        PUTFULL(DTIM_BASE_ADDR + 1224, 0xfed42023);
//        PUTFULL(DTIM_BASE_ADDR + 1228, 0xfef41323);
//        PUTFULL(DTIM_BASE_ADDR + 1232, 0x440006f);
//        PUTFULL(DTIM_BASE_ADDR + 1236, 0x13);
//        PUTFULL(DTIM_BASE_ADDR + 1240, 0xfec42503);
//        PUTFULL(DTIM_BASE_ADDR + 1244, 0xf61ff0ef);
//        PUTFULL(DTIM_BASE_ADDR + 1248, 0x50793);
//        PUTFULL(DTIM_BASE_ADDR + 1252, 0xfe078ae3);
//        PUTFULL(DTIM_BASE_ADDR + 1256, 0xfec42783);
//        PUTFULL(DTIM_BASE_ADDR + 1260, 0x47a783);
//        PUTFULL(DTIM_BASE_ADDR + 1264, 0xff7f713);
//        PUTFULL(DTIM_BASE_ADDR + 1268, 0xfe842783);
//        PUTFULL(DTIM_BASE_ADDR + 1272, 0xe78023);
//        PUTFULL(DTIM_BASE_ADDR + 1276, 0xfe842783);
//        PUTFULL(DTIM_BASE_ADDR + 1280, 0x178793);
//        PUTFULL(DTIM_BASE_ADDR + 1284, 0xfef42423);
//        PUTFULL(DTIM_BASE_ADDR + 1288, 0xfe645783);
//        PUTFULL(DTIM_BASE_ADDR + 1292, 0xfff78793);
//        PUTFULL(DTIM_BASE_ADDR + 1296, 0xfef41323);
//        PUTFULL(DTIM_BASE_ADDR + 1300, 0xfe645783);
//        PUTFULL(DTIM_BASE_ADDR + 1304, 0xfa079ee3);
//        PUTFULL(DTIM_BASE_ADDR + 1308, 0x793);
//        PUTFULL(DTIM_BASE_ADDR + 1312, 0x78513);
//        PUTFULL(DTIM_BASE_ADDR + 1316, 0x1c12083);
//        PUTFULL(DTIM_BASE_ADDR + 1320, 0x1812403);
//        PUTFULL(DTIM_BASE_ADDR + 1324, 0x2010113);
//        PUTFULL(DTIM_BASE_ADDR + 1328, 0x8067);
//        PUTFULL(DTIM_BASE_ADDR + 1332, 0xfe010113);
//        PUTFULL(DTIM_BASE_ADDR + 1336, 0x112e23);
//        PUTFULL(DTIM_BASE_ADDR + 1340, 0x812c23);
//        PUTFULL(DTIM_BASE_ADDR + 1344, 0x2010413);
//        PUTFULL(DTIM_BASE_ADDR + 1348, 0xfea42623);
//        PUTFULL(DTIM_BASE_ADDR + 1352, 0xfeb42423);
//        PUTFULL(DTIM_BASE_ADDR + 1356, 0x60793);
//        PUTFULL(DTIM_BASE_ADDR + 1360, 0xfed42023);
//        PUTFULL(DTIM_BASE_ADDR + 1364, 0xfef41323);
//        PUTFULL(DTIM_BASE_ADDR + 1368, 0x440006f);
//        PUTFULL(DTIM_BASE_ADDR + 1372, 0x13);
//        PUTFULL(DTIM_BASE_ADDR + 1376, 0xfec42503);
//        PUTFULL(DTIM_BASE_ADDR + 1380, 0xf11ff0ef);
//        PUTFULL(DTIM_BASE_ADDR + 1384, 0x50793);
//        PUTFULL(DTIM_BASE_ADDR + 1388, 0xfe079ae3);
//        PUTFULL(DTIM_BASE_ADDR + 1392, 0xfe842783);
//        PUTFULL(DTIM_BASE_ADDR + 1396, 0x7c783);
//        PUTFULL(DTIM_BASE_ADDR + 1400, 0x78713);
//        PUTFULL(DTIM_BASE_ADDR + 1404, 0xfec42783);
//        PUTFULL(DTIM_BASE_ADDR + 1408, 0xe7a023);
//        PUTFULL(DTIM_BASE_ADDR + 1412, 0xfe842783);
//        PUTFULL(DTIM_BASE_ADDR + 1416, 0x178793);
//        PUTFULL(DTIM_BASE_ADDR + 1420, 0xfef42423);
//        PUTFULL(DTIM_BASE_ADDR + 1424, 0xfe645783);
//        PUTFULL(DTIM_BASE_ADDR + 1428, 0xfff78793);
//        PUTFULL(DTIM_BASE_ADDR + 1432, 0xfef41323);
//        PUTFULL(DTIM_BASE_ADDR + 1436, 0xfe645783);
//        PUTFULL(DTIM_BASE_ADDR + 1440, 0xfa079ee3);
//        PUTFULL(DTIM_BASE_ADDR + 1444, 0x793);
//        PUTFULL(DTIM_BASE_ADDR + 1448, 0x78513);
//        PUTFULL(DTIM_BASE_ADDR + 1452, 0x1c12083);
//        PUTFULL(DTIM_BASE_ADDR + 1456, 0x1812403);
//        PUTFULL(DTIM_BASE_ADDR + 1460, 0x2010113);
//        PUTFULL(DTIM_BASE_ADDR + 1464, 0x8067);
//        PUTFULL(DTIM_BASE_ADDR + 1468, 0x6c6c6568);
//        PUTFULL(DTIM_BASE_ADDR + 1472, 0x6f77206f);
//        PUTFULL(DTIM_BASE_ADDR + 1476, 0xa646c72);
//        PUTFULL(DTIM_BASE_ADDR + 1480, 0x0);
//
//
//
//        HAL_Delay(100);
//        PUTFULL(CLINT_MSIP_ADDR, 1);
//
//        HAL_Delay(100);
//        GET(0x80001000);
//        break;
//
//      case '0':
//        for (uint16_t i=0; i<0x44; i+=4) {
//          GET(GPIO_BASE_ADDR+i);
//        }
//        break;
//
//
//      case '1':
//        PUTFULL(GPIO_BASE_ADDR + 0x08, 1);
//        PUTFULL(GPIO_BASE_ADDR + 0x0C, 1);
//        break;
//      case '2':
//        PUTFULL(GPIO_BASE_ADDR + 0x08, 1);
//        PUTFULL(GPIO_BASE_ADDR + 0x0C, 0);
//        break;
//
//      case 'u':
//        GET(UART_TXDATA_ADDR);
//        GET(UART_RXDATA_ADDR);
//        GET(UART_TXCTRL_ADDR);
//        GET(UART_RXCTRL_ADDR);
//        GET(UART_DIV_ADDR);
//        break;
//    }
//  }

  HAL_Delay(50);
}
