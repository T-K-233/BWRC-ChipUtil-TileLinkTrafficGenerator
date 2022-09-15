
#include "oscibear_hal_gpio.h"

void HAL_GPIO_init(GPIO_TypeDef *GPIOx, GPIO_PIN pin) {
  SET_BITS(GPIOx->OUTPUT_EN, (uint32_t)pin);
}

uint8_t HAL_GPIO_readPin(GPIO_TypeDef *GPIOx, GPIO_PIN pin) {
  return READ_BITS(GPIOx->INPUT_VAL, (uint32_t)pin) ? 1 : 0;
}

void HAL_GPIO_writePin(GPIO_TypeDef *GPIOx, GPIO_PIN pin, uint8_t value) {
  if (value) {
    SET_BITS(GPIOx->OUTPUT_VAL, (uint32_t)pin);
  }
  else {
    CLEAR_BITS(GPIOx->OUTPUT_VAL, (uint32_t)pin);
  }
}


