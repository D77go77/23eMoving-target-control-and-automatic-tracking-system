#include "vofa.h"

uint8_t UART1_Rx = 0;
u8 RX1_Data[50] = {0};
char data_hex[9];

float hex_to_float(const char* hex_str) ;
/*
 * 解析出DataBuff中的数据
 * 返回解析得到的数据
 */
void Get_Data(void)
{
	if(RX1_Data[0]==0x2A && RX1_Data[5]==0x3E)  //P调节
	{
		snprintf(data_hex, sizeof(data_hex), "%02X%02X%02X%02X", 
             (unsigned char)RX1_Data[4], (unsigned char)RX1_Data[3], 
             (unsigned char)RX1_Data[2], (unsigned char)RX1_Data[1]);  //字符串拼凑
		// 转换为浮点数
    PID_X.kp = PID_Y.kp = hex_to_float(data_hex);
	}
	else if(RX1_Data[0]==0x2B && RX1_Data[5]==0x3E)  //I调节
	{
		snprintf(data_hex, sizeof(data_hex), "%02X%02X%02X%02X", 
             (unsigned char)RX1_Data[4], (unsigned char)RX1_Data[3], 
             (unsigned char)RX1_Data[2], (unsigned char)RX1_Data[1]);  //字符串拼凑
		// 转换为浮点数
    PID_X.ki = PID_Y.ki= hex_to_float(data_hex);
	}
	else if(RX1_Data[0]==0x2C && RX1_Data[5]==0x3E)  //目标速度调节
	{
		snprintf(data_hex, sizeof(data_hex), "%02X%02X%02X%02X", 
             (unsigned char)RX1_Data[4], (unsigned char)RX1_Data[3], 
             (unsigned char)RX1_Data[2], (unsigned char)RX1_Data[1]);  //字符串拼凑
		// 转换为浮点数
    PID_X.kd = PID_Y.kd= hex_to_float(data_hex);
	}
}



// 十六进制字符串转换为浮点数
float hex_to_float(const char* hex_str) 
{
    uint32_t temp;
    sscanf(hex_str, "%X", &temp);
    float value;
    memcpy(&value, &temp, sizeof(value));
    return value;
}

