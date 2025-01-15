#ifndef __24CXX_H
#define __24CXX_H

#include "common_inc.h"

#define AT24C02_Add 0xA0

u8 AT24Cxx_Write_nBytes(uint8_t *data, uint16_t addr, uint16_t length);
u8 AT24Cxx_Read_nBytes(uint8_t *data, uint16_t addr, uint16_t length);

extern I2C_HandleTypeDef hi2c1;

extern u8 AT24_WR_Flag , AT24_RD_Flag ;//读与写的标志位

extern u8 AT24_WR_Data[6];
//每个可读写8位0~255
extern u8 AT24_RD_Data[6];
//******************************************/


#endif
















