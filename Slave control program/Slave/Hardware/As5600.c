#include "As5600.h"
#include "math.h"
//x,y�ı������Ƕ�
AS5600 as5600_X, as5600_Y;
uint16_t AS5600_X0_DEG = 0, AS5600_Y0_DEG = 0;
uint8_t Return_0_CNT = 0;

float angle_prev=0; 
int full_rotations=0; // full rotation tracking;


////���͵��ֽ�ʱ��
//void AS5600_Write_Reg(unsigned char reg, unsigned char value)
//{
//	HAL_I2C_Mem_Write(&hi2c1, Slave_Addr, reg, I2C_MEMADD_SIZE_8BIT, &value, 1, 50);
//}

////���Ͷ��ֽ�ʱ��
//void AS5600_Write_Regs(unsigned char reg, unsigned char *value, unsigned char len)
//{
//	HAL_I2C_Mem_Write(&hi2c1, Slave_Addr, reg, I2C_MEMADD_SIZE_8BIT, value, len, 50);
//}

//IIC�����ֽ�
void AS5600_Read_Reg(u8 num,unsigned char reg, unsigned char* buf, unsigned short len)
{
	if(num==ANGLE_Y)
		HAL_I2C_Mem_Read(&hi2c1, Slave_Addr, reg, I2C_MEMADD_SIZE_8BIT, buf, len, 50);
	else
		HAL_I2C_Mem_Read(&hi2c2, Slave_Addr, reg, I2C_MEMADD_SIZE_8BIT, buf, len, 50);
}

//�õ������ƵĽǶȣ���Χ��0-6.28
float GetAngle_Without_Track(u8 num)
{   
	float angle_d;
	int16_t in_angle;
	uint8_t temp[2]={0};
	AS5600_Read_Reg(num,Angle_Hight_Register_Addr, temp, 2);
	in_angle = ((int16_t)temp[0] <<8) | (temp[1]);
	angle_d = (float)in_angle * (2.0f*PI) / 4096;
	if(num==ANGLE_X)
		as5600_X.Angle=angle_d/PI*180.0;
	else
		as5600_X.Angle=angle_d/PI*180.0;
	return angle_d;
	//angle_dΪ�����ƣ���Χ��0-6.28
}

float GetAngle(u8 num)
{
    float val = GetAngle_Without_Track(num);
    float d_angle = val - angle_prev;
    //������ת����Ȧ��
    //ͨ���жϽǶȱ仯�Ƿ����80%��һȦ(0.8f*6.28318530718f)���ж��Ƿ������������������ˣ���full_rotations����1�����d_angleС��0�������1�����d_angle����0����
    if(fabs(d_angle) > (0.8f*2.0f*PI) ) full_rotations += ( d_angle > 0 ) ? -1 : 1; 
    angle_prev = val;
    return ( (float)full_rotations * 6.28318530718f + angle_prev);
}



