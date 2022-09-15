
#ifndef __OSCIBEAR_HAL_H
#define __OSCIBEAR_HAL_H


#ifdef __cplusplus
extern "C" {
#endif

#include "oscibear.h"
#include "oscibear_hal_gpio.h"
#include "oscibear_hal_time.h"
#include "oscibear_hal_uart.h"

void HAL_init(); 

#ifdef __cplusplus
}
#endif

#endif /* __OSCIBEAR_HAL_H */