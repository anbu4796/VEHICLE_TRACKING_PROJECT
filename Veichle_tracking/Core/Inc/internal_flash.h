/*
 * internal_flash.h
 *
 *  Created on: Feb 7, 2024
 *      Author: ram77
 */

#ifndef INC_INTERNAL_FLASH_H_
#define INC_INTERNAL_FLASH_H_

#include <stdint.h>
#include "stm32l4xx_hal.h"


void writeStringToFlash(uint64_t address,  char* str);
void readDataFromFlash(uint64_t address, char* buffer, uint64_t dataSize);
void eraseFlashPage(uint64_t pageAddress);


#endif /* INC_INTERNAL_FLASH_H_ */
