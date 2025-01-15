#include "common_inc.h"

//urat_send
extern int UART_printf(uint32_t ui32Base, const char *fmt, ...);

//u8g2
u8g2_t u8g2; // 显示器初始化结构体
u8 Mode=1;//1_找四顶点和中心点 2_大框细化(测试) 3_画大框 4_画小框 3_
u8 Re_Start_Flag=0;
u8 Start_cnt=2;
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
		task_mainloop();
	}
}
//************************
void main_init(void)
{	
	//uart2
    SParser_Init(&parser, "B", "F", ",", fmtStr);
    HAL_UART_Receive_IT(&huart2, &UART2_Rx, 1);
	
	//TIM
	HAL_TIM_Base_Start_IT(&htim1); 
	
	//U8G2
	MD_OLED_RST_Set(); //复位拉高
	u8g2Init(&u8g2);  //显示器调用初始化函数

	//ComKey
    ComKey_Init(&Key_0, 10);
	ComKey_Init(&Key_1, 10);
	ComKey_Init(&Key_2, 10);
	ComKey_Init(&Key_3, 10);
	ComKey_Init(&Key_4, 10);
	

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
	if (key->clickCnt == 1)//单击
	{
        LED_B = 1;
		if(key==&Key_0)
		{
			Start_cnt=2;
			Re_Start_Flag=1;
			LED_R=LED_B=LED_G=1;
		}
        if (key == &Key_1) // 切换模式
		{ 
            Mode++;
            if (Mode > 4)
                Mode = 1;
            if (Mode == 1 || Mode == 2)
                Pulse_Time = 1000;
            else
                Pulse_Time = 400;
        }
		if (Mode == 1) // 校准原点
		{ 
			if (key == &Key_2) // 切换XY方向
			{ 
				if (Mode_1_XY == 0)
					Mode_1_XY = 1;
				else
					Mode_1_XY = 0;
			}
			if (key == &Key_3 && Step_Free)// 递增
			{ 
				if (Mode_1_XY == 0) 
				{
					Pulse_X_Num = 1;
					Pulse_Y_Num = 0;
				} else {
					Pulse_X_Num = 0;
					Pulse_Y_Num = 1;
				}
				Pulse_2_PWM();
			}
			if (key == &Key_4 && Step_Free)// 递减
			{ 
				if (Mode_1_XY == 0) 
				{
					Pulse_X_Num = -1;
					Pulse_Y_Num = 0;
				} else {
					Pulse_X_Num = 0;
					Pulse_Y_Num = -1;
				}
				Pulse_2_PWM();
			}
		}
		if (Mode == 2) // 校准边角
		{ 
			if (key == &Key_2 && Step_Free) // 在角落打点
			{ 
				static uint8_t Cal_CNT = 0;
				POS_X_Tar = Edge_POS[Cal_CNT][0];
				POS_Y_Tar = Edge_POS[Cal_CNT][1];
				POS_2_Pulse(POS_X_Cur, POS_Y_Cur, POS_X_Tar, POS_Y_Tar, &Pulse_X_Num, &Pulse_Y_Num);
				Pulse_2_PWM();
				Cal_CNT++;
				if (Cal_CNT > 3)
					Cal_CNT = 0;
			}
			if (key == &Key_3) { // 屏幕距离递增
				Screen_Dis += 10;
			}
			if (key == &Key_4) { // 屏幕距离递减
				Screen_Dis -= 10;
			}
		}
		if (Mode == 3)  // 画大框框
		{
			if (key == &Key_2 && Step_Free) // 开画
			{ 
				Position_Subdivide(Edge_POS, Track_POS);
				Track_Flag = 1;
				Track_CNT = 0;
			}
			if (key == &Key_3) 
			{ // 切换速度
				if (Pulse_Time == 400)
					Pulse_Time = 600;
				else if (Pulse_Time == 600)
					Pulse_Time = 800;
				else if (Pulse_Time == 800)
					Pulse_Time = 1000;
				else if (Pulse_Time == 1000)
					Pulse_Time = 400;
			}
            if (key == &Key_4) 
			{ // 暂停
                if (Stop_Flag == 0) 
				{
                    Stop_Flag = 1;
                } else if (Stop_Flag == 1) 
				{
                    // 暂停补偿
                    Step_Init(&stepX, &htim3, TIM_CHANNEL_4,
                              DIR_X_GPIO_Port, DIR_X_Pin, 20, 20, 500);
                    Step_Init(&stepY, &htim2, TIM_CHANNEL_1,
                              DIR_Y_GPIO_Port, DIR_Y_Pin, 20, 20, 500);
                    if (Step_To_Go_X != 0 || Step_To_Go_Y != 0) 
					{
                        Pulse_X_Num = (int16_t) Step_To_Go_X;
                        Pulse_Y_Num = (int16_t) Step_To_Go_Y;
//                        UART_printf(&huart1, "Pulse_Num:%d,%d\n", Pulse_X_Num, Pulse_Y_Num);
                        if (Pulse_X_Num > 0)
                            Start_PWM(&stepX, abs(Pulse_X_Num), 0, 0);
                        else if (Pulse_X_Num < 0)
                            Start_PWM(&stepX, abs(Pulse_X_Num), 1, 0);

                        if (Pulse_Y_Num > 0)
                            Start_PWM(&stepY, abs(Pulse_Y_Num), 1, 0);
                        else if (Pulse_Y_Num < 0)
                            Start_PWM(&stepY, abs(Pulse_Y_Num), 0, 0);
                        Step_To_Go_Flag = 1;
                    } else 
					{
                        Track_Flag = 1;
                        Track_CNT++;
                    }
                    Stop_Flag = 0;
                }
            }
		}

	}
	
	if (Mode == 4) // 画小框框&& Mode4_RX_Flag == 1
	{
		if (key == &Key_2 && Step_Free ) // 开画
		{
			Position_Subdivide(Box_POS, Track_POS);
			Track_Flag = 1;
			Track_CNT = 0;
			Pulse_X_CNT = Pulse_Y_CNT = 0;
		}
		if (key == &Key_3)// 切换速度
		{
			if (Pulse_Time == 200)
					Pulse_Time = 400;
			else if (Pulse_Time == 400)
					Pulse_Time = 600;
			else if (Pulse_Time == 600)
					Pulse_Time = 200;
		}
		if (key == &Key_4) // 暂停
		{
			if (Stop_Flag == 0)
			{
				Stop_Flag = 1;
			}
			else if (Stop_Flag == 1) // 暂停补偿
			{
				Step_Init(&stepX, &htim3, TIM_CHANNEL_4,
									DIR_X_GPIO_Port, DIR_X_Pin, 20, 20, 500);
				Step_Init(&stepY, &htim2, TIM_CHANNEL_1,
									DIR_Y_GPIO_Port, DIR_Y_Pin, 20, 20, 500);
			}
			if (Step_To_Go_X != 0 || Step_To_Go_Y != 0)
			{
				Pulse_X_Num = (int16_t) Step_To_Go_X;
				Pulse_Y_Num = (int16_t) Step_To_Go_Y;
				UART_printf((uint32_t)&huart1, "Pulse_Num:%d,%d\n", Pulse_X_Num, Pulse_Y_Num);
				if (Pulse_X_Num > 0)
						Start_PWM(&stepX, abs(Pulse_X_Num), 1, 0);
				else if (Pulse_X_Num < 0)
						Start_PWM(&stepX, abs(Pulse_X_Num), 0, 0);

				if (Pulse_Y_Num > 0)
						Start_PWM(&stepY, abs(Pulse_Y_Num), 1, 0);
				else if (Pulse_Y_Num < 0)
						Start_PWM(&stepY, abs(Pulse_Y_Num), 0, 0);
				Step_To_Go_Flag = 1;
			}
			else
			{
				Track_Flag = 1;
				Track_CNT++;
			}
			Stop_Flag = 0;
		}
	}
	if (key->clickCnt == 2) //双击
	{
		LED_G=1;
        if (key == &Key_1 && Step_Free) 
		{ // 回到原点
			
			
			POS_X_Tar =0;
			POS_Y_Tar = 0;
			POS_2_Pulse(POS_X_Cur, POS_Y_Cur, POS_X_Tar, POS_Y_Tar, &Pulse_X_Num, &Pulse_Y_Num);
			Pulse_2_PWM();
        }
	}
}
//**************************************
//长按函数
void ComKey_HoldTriggerCallback(comkey_t *key)
{
	LED_R=1;
    if (Mode == 1) 
	{ // 校准原点
        if (key == &Key_3) 
		{
            if (Mode_1_XY == 0) 
			{
                Pulse_X_Num = 5;
                Pulse_Y_Num = 0;
                Pulse_2_PWM();
            } else {
                Pulse_X_Num = 0;
                Pulse_Y_Num = 5;
                Pulse_2_PWM();
            }
        }
        if (key == &Key_4) 
		{
            if (Mode_1_XY == 0) 
			{
                Pulse_X_Num = -5;
                Pulse_Y_Num = 0;
                Pulse_2_PWM();
            } else {
                Pulse_X_Num = 0;
                Pulse_Y_Num = -5;
                Pulse_2_PWM();
            }
        }
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
	UART_printf((uint32_t)&huart1, "here as5600:%f,%f\n", angle_x, angle_y);
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
	if (Step_Free) 
	{
		if (Mode == 3) 
		{ // 画大框框
			if (Track_Flag == 1) 
			{
				POS_X_Tar = Track_POS[Track_CNT][0];
				POS_Y_Tar = Track_POS[Track_CNT][1];
				POS_2_Pulse(POS_X_Cur, POS_Y_Cur, POS_X_Tar, POS_Y_Tar, &Pulse_X_Num, &Pulse_Y_Num);
				Pulse_2_PWM();
				Track_CNT++;
				if (Track_CNT >40) 
				{
					Track_Flag = 0;
					Track_CNT = 0;
				}
			}
		}
		else if (Mode == 4) // 画小框框
		{
			if (Track_Flag == 1)
			{
				if (Track_CNT <= 40)
				{
					POS_X_Tar = Track_POS[Track_CNT][0];
					POS_Y_Tar = Track_POS[Track_CNT][1];
					POS_2_Pulse(POS_X_Cur, POS_Y_Cur, POS_X_Tar, POS_Y_Tar, &Pulse_X_Num, &Pulse_Y_Num);
					if (Track_CNT > 0)
					{
						 Pulse_X_CNT += Pulse_X_Num;
						 Pulse_Y_CNT += Pulse_Y_Num;
					}
					Pulse_2_PWM();
					Track_CNT++;
				}
				else
				{
					Track_Flag = 0;
					Track_CNT = 0;
					Mode4_RX_Flag = 0;
					// 误差补偿
					Pulse_X_Num = -Pulse_X_CNT;
					Pulse_Y_Num = -Pulse_Y_CNT;
					if (Pulse_X_Num > 0)
							Start_PWM(&stepX, abs(Pulse_X_Num), 1, 0);
					else if (Pulse_X_Num < 0)
							Start_PWM(&stepX, abs(Pulse_X_Num), 0, 0);

					if (Pulse_Y_Num > 0)
							Start_PWM(&stepY, abs(Pulse_Y_Num), 1, 0);
					else if (Pulse_Y_Num < 0)
							Start_PWM(&stepY, abs(Pulse_Y_Num), 0, 0);
				}
			}
		}
	}		

	
	if(Re_Start_Flag==1)
	{
		Step_Center_Init();
	}


}

/**********************************************************************************/
void IS_DEBUG(void)
{
	if(Uart_RX_Flag)
	{
		//Mode4_RX_Flag=0;
		UART_printf((uint32_t)&huart1, "position:		(%d,%d)(%d,%d)(%d,%d)(%d,%d)\n",Rex_POS[0][0],Rex_POS[0][1],Rex_POS[1][0],Rex_POS[1][1],Rex_POS[2][0],Rex_POS[2][1],Rex_POS[3][0],Rex_POS[3][1]);
		UART_printf((uint32_t)&huart1, "new_position:(%d,%d)(%d,%d)(%d,%d)(%d,%d)\n",Box_POS[0][0],Box_POS[0][1],Box_POS[1][0],Box_POS[1][1],Box_POS[2][0],Box_POS[2][1]);
	}

}


/**********************************************************************************/
void Step_Center_Init(void) 
{
	
	if(Step_Free && Start_cnt)
	{
        // 调整x轴至中心
        float error_x = AS5600_X - angle_x;
        if (fabs(error_x) > 1) // 如果偏差大于1度，则继续调整
        { 
            Pulse_X_Num=error_x*32/1.8f;
            Pulse_2_PWM(); // 调用函数调整x轴电机
        } else {
            Pulse_X_Num = 0; // 达到目标，停止调整
        }

        // 调整y轴至中心
        float error_y = AS5600_Y - angle_y;
        if (fabs(error_y) > 1) 
        {
            Pulse_Y_Num = -error_y*32/1.8f;
            Pulse_2_PWM(); // 调整y轴电机
        } else {
            Pulse_X_Num = 0; // 达到目标，停止调整
        }

        // 当两个轴都接近目标时，结束任务
        if (fabs(error_x) <= 1 && fabs(error_y) <= 1) 
		{
            Start_cnt--;
			
            Pulse_X_Num = (AS5600_X - angle_x) * 32.0f / 1.8f;
            Pulse_Y_Num = (AS5600_Y - angle_y) * 32.0f / 1.8f;
            Pulse_2_PWM(); // 根据最终的脉冲数设置PWM
			POS_X_Cur=0;POS_Y_Cur=0;
        }
	}
	
	if(Start_cnt==0)
		Re_Start_Flag=0;

}

	




