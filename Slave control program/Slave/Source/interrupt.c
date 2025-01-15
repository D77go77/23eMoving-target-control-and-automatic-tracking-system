#include "interrupt.h"

extern int UART_printf(uint32_t ui32Base, const char *fmt, ...);

u8 f_50ms_oled=0;
float angle_x,angle_y=0;
u8 Uart_RX_Flag=0;
//********************=0;***
void tim_proc(void);
//***********************
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)//��ʱ���ж�
{
	if(htim->Instance == TIM1)
	{
		tim_proc();
	}
}
//***********************
void tim_proc(void)
{
	static u8 cnt_10ms = 0;
	static u8 cnt_50ms = 0;
	static u16 cnt_100ms=0;	
	if(++cnt_10ms>=10)
	{
		ComKey_Porc();//����ɨ��
//		angle_x=GetAngle(ANGLE_X)/PI*180.0;//��������ȡ
//		angle_y=GetAngle(ANGLE_Y)/PI*180.0;
		
		cnt_10ms=0;

	}
	if(++cnt_50ms>=50)
	{
		cnt_50ms=0;
		f_50ms_oled=1;
		
	}
	if(++cnt_100ms>=100)
	{
		led_proc();
		cnt_100ms=0;

	}
	if(uart_f)
	{
		if(++uart_cnt>=10)
		{
			rx1_index=0;
			uart_cnt=0;
			Get_Data();
			memset(RX1_Data,0,sizeof(RX1_Data));
		}

	}


}

//***********************
/**
 * @brief PWM ��������ص�����
 * 
 * �˺����� PWM ���岥����ɺ󱻵��ã�����ִ���ض��Ĳ�����
 * 
 */
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	
    if (htim == stepX.phtim) 
	{
        HAL_TIM_PWM_Stop_DMA(stepX.phtim, stepX.channel);
        if (stepX.state != Stop) // δ�˶���Ŀ����������
            Step_X_Flag = 1;
        else 
		{ // ���˶���Ŀ����������
            Step_Unlock(&stepX);
        }
    } else if (htim == stepY.phtim) 
	{
        HAL_TIM_PWM_Stop_DMA(stepY.phtim, stepY.channel);
        if (stepY.state != Stop)
            Step_Y_Flag = 1;
        else 
		{
            Step_Unlock(&stepY);
        }
    }
}



//***********************
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	static u8 rx_index=0;


	if (huart->Instance == huart1.Instance)
	{
		
		RX1_Data[rx1_index++]=UART1_Rx;
		uart_f=1;
		HAL_UART_Receive_IT(&huart1, &UART1_Rx, 1); // ����������һ����
	}
    
}





