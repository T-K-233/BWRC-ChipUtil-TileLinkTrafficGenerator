
#ifndef __OSCIBEAR_HAL_GPIO_H
#define __OSCIBEAR_HAL_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "oscibear.h"

typedef enum {
  GPIO_PIN_0 = 0b0001U,
  GPIO_PIN_1 = 0b0010U,
  GPIO_PIN_2 = 0b0100U
} GPIO_PIN;

void HAL_GPIO_init(GPIO_TypeDef *GPIOx, GPIO_PIN pin);

uint8_t HAL_GPIO_readPin(GPIO_TypeDef *GPIOx, GPIO_PIN pin);

void HAL_GPIO_writePin(GPIO_TypeDef *GPIOx, GPIO_PIN pin, uint8_t value);

#ifdef __cplusplus
}
#endif

#endif /* __OSCIBEAR_HAL_GPIO_H */
