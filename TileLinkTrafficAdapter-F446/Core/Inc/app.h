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
#include "tilelink.h"


void APP_init();

void APP_main();

#endif /* INC_APP_H_ */
