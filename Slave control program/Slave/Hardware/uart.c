#include "uart.h"

#include <stdio.h>
#include <math.h>
#include <string.h>

#define DATA_VALID_CHAR 'G'//֡ͷ 
#define NUMBERS_TO_EXTRACT 4//���ָ���

u8 RX2_Data[50] = {0};
u16 data[4]={0};


u8 uart_f=0;
u8 uart_f2=0;
u8 uart_cnt=0;
u8 uart2_cnt=0;
u8 rx1_index=0;
u8 rx_index=0;
// �����˴�����İ汾
int safe_sscanf(const char *src, const char *format, ...) 
{
    int result;
    va_list args;
    va_start(args, format);
    result = vsscanf(src, format, args);
    va_end(args);
    return result;
}

void uart_proc(void);

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) 
{
    if (huart == &huart2) 
	{
        uart_proc();

        // ��֤���ݴ������ս��ջ�����
        memset(RX2_Data, 0, sizeof(RX2_Data));
        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, RX2_Data, sizeof(RX2_Data));
        __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);
    }
}

// ����˶��������ݸ�ʽ����֤
void extract_numbers(const u8 *RX2_Data, u16 data[]) 
{
    if (RX2_Data[0] != DATA_VALID_CHAR) return; // ��֤������Ч�Ա�־
	
    int parsed_items = safe_sscanf(RX2_Data + 1, "%d,%d,%d,%d", &data[0], &data[1], &data[2], &data[3]);
    if (parsed_items != NUMBERS_TO_EXTRACT) return; // ����Ƿ�ɹ�����������Ԥ�ڵ�����
}

void uart_proc(void) 
{
    if (RX2_Data[0] == DATA_VALID_CHAR) 
	{
        extract_numbers(RX2_Data, data);

        if (Mode == 1) 
		{
//            PID_X.real = (atanf((float) Screen_Dis / (float) ((float) Green_Dis - (float) data[2])) -
//                          atanf((float) Screen_Dis / (float) ((float) Green_Dis - (float) data[0]))) * 180.0f / M_PI;
			PID_X.real = (atanf((float) data[2] / (float) Screen_Dis) - atanf((float) data[0] / (float) Screen_Dis)) * 180.0f / M_PI;
            PID_Y.real = (atanf((float) data[3] / (float) Screen_Dis) - atanf((float) data[1] / (float) Screen_Dis)) * 180.0f / M_PI;
            PID_X.target = 0;
            PID_Y.target = 0;

            float Error = sqrtf(powf((float) data[0] - (float) data[2], 2) + powf((float) data[1] - (float) data[3], 2));
            if (Error < 30) 
			{
                //LED_R_ON ;// �޸�LED����Ϊ��Ӧ����
            } else 
			{
                LED_R_OFF; // �޸�LED����Ϊ��Ӧ����
            }
        } else 
		{
           LED_R_OFF; // �޸�LED����Ϊ��Ӧ����
        }
    }
}
