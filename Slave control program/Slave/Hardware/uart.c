#include "uart.h"

#include <stdio.h>
#include <math.h>
#include <string.h>

#define DATA_VALID_CHAR 'G'//帧头 
#define NUMBERS_TO_EXTRACT 4//数字个数

u8 RX2_Data[50] = {0};
u16 data[4]={0};


u8 uart_f=0;
u8 uart_f2=0;
u8 uart_cnt=0;
u8 uart2_cnt=0;
u8 rx1_index=0;
u8 rx_index=0;
// 增加了错误检查的版本
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

        // 保证数据处理后清空接收缓冲区
        memset(RX2_Data, 0, sizeof(RX2_Data));
        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, RX2_Data, sizeof(RX2_Data));
        __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);
    }
}

// 添加了对输入数据格式的验证
void extract_numbers(const u8 *RX2_Data, u16 data[]) 
{
    if (RX2_Data[0] != DATA_VALID_CHAR) return; // 验证数据有效性标志
	
    int parsed_items = safe_sscanf(RX2_Data + 1, "%d,%d,%d,%d", &data[0], &data[1], &data[2], &data[3]);
    if (parsed_items != NUMBERS_TO_EXTRACT) return; // 检查是否成功解析了所有预期的数字
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
                //LED_R_ON ;// 修改LED操作为对应函数
            } else 
			{
                LED_R_OFF; // 修改LED操作为对应函数
            }
        } else 
		{
           LED_R_OFF; // 修改LED操作为对应函数
        }
    }
}
