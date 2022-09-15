
#ifndef __OSCIBEAR_HAL_TIME_H
#define __OSCIBEAR_HAL_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include "oscibear.h"

#define SYS_CLK_FREQ  250000                  // Hz
#define MTIME_FREQ    (SYS_CLK_FREQ / 20000)     // tick per milliseconds

uint64_t HAL_getTick();

void HAL_delay(uint64_t time);

#ifdef __cplusplus
}
#endif

#endif /* __OSCIBEAR_HAL_TIME_H */