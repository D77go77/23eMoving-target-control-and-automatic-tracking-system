#include "vofa.h"

uint8_t UART1_Rx = 0;
u8 RX1_Data[50] = {0};
char data_hex[9];

float hex_to_float(const char* hex_str) ;
/*
 * ������DataBuff�е�����
 * ���ؽ����õ�������
 */
void Get_Data(void)
{
	if(RX1_Data[0]==0x2A && RX1_Data[5]==0x3E)  //P����
	{
		snprintf(data_hex, sizeof(data_hex), "%02X%02X%02X%02X", 
             (unsigned char)RX1_Data[4], (unsigned char)RX1_Data[3], 
             (unsigned char)RX1_Data[2], (unsigned char)RX1_Data[1]);  //�ַ���ƴ��
		// ת��Ϊ������
    PID_X.kp = PID_Y.kp = hex_to_float(data_hex);
	}
	else if(RX1_Data[0]==0x2B && RX1_Data[5]==0x3E)  //I����
	{
		snprintf(data_hex, sizeof(data_hex), "%02X%02X%02X%02X", 
             (unsigned char)RX1_Data[4], (unsigned char)RX1_Data[3], 
             (unsigned char)RX1_Data[2], (unsigned char)RX1_Data[1]);  //�ַ���ƴ��
		// ת��Ϊ������
    PID_X.ki = PID_Y.ki= hex_to_float(data_hex);
	}
	else if(RX1_Data[0]==0x2C && RX1_Data[5]==0x3E)  //Ŀ���ٶȵ���
	{
		snprintf(data_hex, sizeof(data_hex), "%02X%02X%02X%02X", 
             (unsigned char)RX1_Data[4], (unsigned char)RX1_Data[3], 
             (unsigned char)RX1_Data[2], (unsigned char)RX1_Data[1]);  //�ַ���ƴ��
		// ת��Ϊ������
    PID_X.kd = PID_Y.kd= hex_to_float(data_hex);
	}
}



// ʮ�������ַ���ת��Ϊ������
float hex_to_float(const char* hex_str) 
{
    uint32_t temp;
    sscanf(hex_str, "%X", &temp);
    float value;
    memcpy(&value, &temp, sizeof(value));
    return value;
}

