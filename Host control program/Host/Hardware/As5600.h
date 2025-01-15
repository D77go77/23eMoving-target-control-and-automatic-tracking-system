#ifndef __AS5600_H__
#define __AS5600_H__

#include "common_inc.h"

#define AS5600_X 81.6
#define AS5600_Y 285.6

#define PI										3.14159265358979f
#define Slave_Addr                0x36<<1		//设备从地址
#define Write_Bit                 0	   			//写标记
#define Read_Bit                  1    			//读标记
#define Angle_Hight_Register_Addr 0x0C 			//寄存器高位地址
#define Angle_Low_Register_Addr   0x0D 			//寄存器低位地址

#define ANGLE_X 100//读取哪一个iic
#define ANGLE_Y 50

void AS5600_Read_Reg(u8 num,unsigned char reg, unsigned char* buf, unsigned short len);
void AS5600_Write_Reg(unsigned char reg, unsigned char value);

float GetAngle_Without_Track(u8 num);
float GetAngle(u8 num);

typedef struct AS5600 
{
    I2C_HandleTypeDef Use_i2c;
    uint16_t Original_Value;
    float Angle;
} AS5600;

extern AS5600 as5600_X, as5600_Y;
extern u16 AS5600_X0_DEG , AS5600_Y0_DEG;
extern u8 Return_0_CNT ;


#endif


