#include "common_inc.h"

//urat_send
extern int UART_printf(uint32_t ui32Base, const char *fmt, ...);

//u8g2
u8g2_t u8g2; // 显示器初始化结构体
u8 Mode=0;//1开始寻点
u8 Re_Start_Flag=0;
u8 Start_cnt=2;
u8 f_trace=0;
//************************************8
void main_init(void);

void AT24_mainloop(void);
void u8g2_mainloop(void);
void move_mainloop(void);
void task_mainloop(void);
void IS_DEBUG(void);
//***********************
void mainloop(void)
{
	main_init();

	while(1)
	{
		u8g2_mainloop();
		move_mainloop();
		UART_printf((uint32_t)&huart1, "%d,%d,%d,%d,%f,%f,%f,%f,%f\n",
				data[2],data[0],
				data[3],data[1],
				PID_X.kp,PID_X.ki,PID_X.kd,
				PID_X.output,PID_Y.output);
		task_mainloop();//主控
//		printf("%f,%f,%f,%f,%f,%f,%f\n",
//				PID_X.real,PID_X.target,  
//				PID_Y.real,PID_Y.target,  
//				PID_X.kp,PID_X.ki,PID_X.kd);
	}
}
//************************
void main_init(void)
{	
	//uart2 DMA
	HAL_UARTEx_ReceiveToIdle_DMA(&huart2,RX2_Data,sizeof(RX2_Data));  //开启接收中断
	__HAL_DMA_DISABLE_IT(&hdma_usart2_rx,DMA_IT_HT);  //关闭过半中断
	HAL_UART_Receive_IT(&huart1, &UART1_Rx, 1);
	//TIM
	HAL_TIM_Base_Start_IT(&htim1); 
	
	//U8G2
	u8g2Init(&u8g2);  //显示器调用初始化函数

	//ComKey
    ComKey_Init(&Key_0, 10);
	ComKey_Init(&Key_1, 10);
	ComKey_Init(&Key_2, 10);
	ComKey_Init(&Key_3, 10);
	ComKey_Init(&Key_4, 10);
	
    // PID
    INC_PID_Init(&PID_X, 0.58, 0.00, 0.54);
    INC_PID_Init(&PID_Y, 0.58, 0.00, 0.54);
    INC_PID_Set_Target(&PID_X, 0);
    INC_PID_Set_Target(&PID_Y, 0);

	// StepMotor
    Step_Init(&stepX, &htim3, TIM_CHANNEL_4,
              DIR_X_GPIO_Port, DIR_X_Pin, 500, 500, 500);
    Step_Init(&stepY, &htim2, TIM_CHANNEL_1,
              DIR_Y_GPIO_Port, DIR_Y_Pin, 500, 500, 500);
	
	AT24_RD_Flag=1;
}
//**************************************
//连击函数(单双击)
void ComKey_MultipleClickCallback(comkey_t *key) 
{
    if (key->clickCnt == 1) 
	{ // 单击
        LED_B = 1;
        if (key == &Key_1) 
		{
            if (Mode == 0) 
			{
                Mode = 1;
                Stop_Flag = 0;
                Laser_Wait_CNT = 3;
            } else if (Mode == 1) 
			{
                Mode = 0;
            }
        }
        if (Mode == 1) 
		{ // 开寻！
            if (key == &Key_4) 
			{
                if (Stop_Flag == 0)
                    Stop_CNT = 1;
                else
                    Stop_Flag = 0;
            }
        }
    }
}
//**************************************
//长按函数
void ComKey_HoldTriggerCallback(comkey_t *key)
{
	LED_G=1;
    if (Mode == 0) 
	{
        uint8_t index;
        for (uint8_t i = 0; i < 14; i++)
            if (Green_Dis_Val[i] == Green_Dis)
                index = i;
        if (key == &Key_3 && index < 13)
            index++;
        if (key == &Key_4 && index > 0)
            index--;
        Green_Dis = Green_Dis_Val[index];
    }
}
//**************************************
void AT24_mainloop(void)
{
//	// AT24Cxx
//	//write
	if (AT24_WR_Flag == 1) 
	{
		AT24Cxx_Write_nBytes(AT24_WR_Data, 0x00, 6);		
		AT24_WR_Flag = 0;
	}
	
	//read
	if (AT24_RD_Flag == 1) 
	{
		//读取AT24到DATA数组
		AT24Cxx_Read_nBytes(AT24_RD_Data, 0x00, 6);
		//赋值前先清零
		AT24_RD_Flag = 0;
	}
}
//***********************************
void u8g2_mainloop(void)
{
	if(!f_50ms_oled) return;
	f_50ms_oled=0;
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_4);//防止程序卡死
	IS_DEBUG();
	
	//显示程序
	u8g2_FirstPage(&u8g2);
	draw(&u8g2);
	u8g2_NextPage(&u8g2);
}
//***********************************
void move_mainloop(void)
{
	if (Step_X_Flag) 
	{
		if (Step_IsBuffRdy(&stepX)) 
		{
			HAL_TIM_PWM_Start_DMA(stepX.phtim, stepX.channel,
								  (uint32_t *) Step_GetCurBuffer(&stepX),
								  Step_BuffUsedLength(&stepX));
			Step_BufferUsed(&stepX);
		}
		Step_BuffFill(&stepX);
		Step_X_Flag = 0;
	}
	if (Step_Y_Flag) 
	{
		if (Step_IsBuffRdy(&stepY)) 
		{
			HAL_TIM_PWM_Start_DMA(stepY.phtim, stepY.channel,
								  (uint32_t *) Step_GetCurBuffer(&stepY),
								  Step_BuffUsedLength(&stepY));
			Step_BufferUsed(&stepY);
		}
		Step_BuffFill(&stepY);
		Step_Y_Flag = 0;
	}
}
//***********************************************
void task_mainloop(void)
{
	// laser
//	if (Laser_Wait_CNT)
//			Laser_Wait_CNT--;
	// PID
//	if (Stop_CNT) 
//	{
//		Stop_CNT++;
//		if (Stop_CNT == 3) 
//		{
//			Stop_Flag = 1;
//			Stop_CNT = 0;
//		}
//	}
	
	if (Mode == 1  ) //&& Laser_Wait_CNT == 0&& Stop_Flag == 0
	{
		f_trace=1;
		
		INC_PID_Callback(&PID_X);
		INC_PID_Callback(&PID_Y);
		POS_2_Pulse(PID_X.output, PID_Y.output, &Pulse_X_Num, &Pulse_Y_Num);
		Pulse_Time = 200;
		if (Pulse_X_Num > 100)
			Pulse_X_Num = 100;
		if (Pulse_X_Num < -100)
			Pulse_X_Num = -100;
		if (Pulse_Y_Num > 100)
			Pulse_Y_Num = 100;
		if (Pulse_Y_Num < -100)
			Pulse_Y_Num = -100;
		if (Step_Free)
			Pulse_2_PWM();
	}
}

/**********************************************************************************/
void IS_DEBUG(void)
{
	if(Uart_RX_Flag)
	{

	}

}
int fputc(int ch, FILE *f)
{
 uint8_t temp[1] = {ch};
 HAL_UART_Transmit(&huart1, temp, 1, 2);
 return ch;
}


	




