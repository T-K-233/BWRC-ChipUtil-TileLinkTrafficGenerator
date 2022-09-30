
#ifndef __OSCIBEAR_HAL_H
#define __OSCIBEAR_HAL_H


#ifdef __cplusplus
extern "C" {
#endif

#include "oscibear.h"
#include "oscibear_hal_core.h"
#include "oscibear_hal_clint.h"
#include "oscibear_hal_gpio.h"
#include "oscibear_hal_plic.h"
#include "oscibear_hal_rcc.h"
#include "oscibear_hal_uart.h"


#define SYS_CLK_FREQ  200000                    // Hz
#define MTIME_FREQ    (SYS_CLK_FREQ / 20000)    // tick per milliseconds

void HAL_init();

uint64_t HAL_getTick();

void HAL_delay(uint64_t time);

#ifdef __cplusplus
}
#endif

#endif /* __OSCIBEAR_HAL_H */
