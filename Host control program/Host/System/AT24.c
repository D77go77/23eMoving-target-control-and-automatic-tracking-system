#include "AT24.h"

// AT24Cxx
uint8_t AT24_WR_Flag = 0, AT24_RD_Flag = 0;
uint8_t AT24_WR_Data[6] = {0}; // 0 1: AS5600_X; 2 3: AS5600_Y; 4 5: Screen_Dis
uint8_t AT24_RD_Data[6];

uint8_t AT24Cxx_Write_nBytes(uint8_t *data, uint16_t addr, uint16_t length) 
{
    uint16_t Offset = 0;
    uint16_t WR_Length = length;
    uint8_t chunk_size = length > 16 ? 16 : WR_Length;
    while (WR_Length > 0) {
        if (HAL_I2C_Mem_Write(&hi2c1, AT24C02_Add, addr + Offset, I2C_MEMADD_SIZE_8BIT, data + Offset, chunk_size, 0xFF) != HAL_OK)
            return 1;
        Offset += chunk_size;
        if (WR_Length < chunk_size)
            chunk_size = WR_Length;
        WR_Length -= chunk_size;
        HAL_Delay(5);
    }
    return 0;
}

uint8_t AT24Cxx_Read_nBytes(uint8_t *data, uint16_t addr, uint16_t length) 
{
    if (HAL_I2C_Mem_Read(&hi2c1, AT24C02_Add, addr, I2C_MEMADD_SIZE_8BIT, data, length, 0xFF) != HAL_OK)
        return 1;
    else
        return 0;
}




