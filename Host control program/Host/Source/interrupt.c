#include "interrupt.h"

extern int UART_printf(uint32_t ui32Base, const char *fmt, ...);

u8 f_50ms_oled=0;
float angle_x,angle_y=0;
u8 Uart_RX_Flag=0;
//********************=0;***
void tim_proc(void);
//***********************
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)//定时器中断
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
		ComKey_Porc();//按键扫描
		angle_x=GetAngle(ANGLE_X)/PI*180.0;//编码器获取
		angle_y=GetAngle(ANGLE_Y)/PI*180.0;
		
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
}
//***********************
/**
 * @brief UART接收完成回调函数
 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == huart2.Instance)
    {
        // 调用解析函数处理接收到的数据
		if (Mode == 1) 
		{
			if (SParser_Parse(&parser, databuff, UART2_Rx))
			{
				for (uint8_t i = 0; i < 5; i++) 
				{
					Rex_POS[i][0] = (int16_t) databuff[i * 2].flt;
					Rex_POS[i][1] = (int16_t) databuff[i * 2 + 1].flt;
	//				// POS_X_Tar = databuff[8].flt;
	//				// POS_Y_Tar = databuff[9].flt;
	//				// POS_X_Cur = databuff[10].flt;
	//				// POS_Y_Cur = databuff[11].flt;

				}
				Uart_RX_Flag = 1;
				Reset_New_Pos();//接收坐标后，更新坐标系
				HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
				// 通过翻转GPIOA的第4脚LED来指示数据处理完成
			}
		}
        HAL_UART_Receive_IT(&huart2, &UART2_Rx, 1); // 继续接收下一个字
    }
}
//***********************
/**
 * @brief PWM 脉冲结束回调函数
 * 
 * 此函数在 PWM 脉冲播放完成后被调用，用于执行特定的操作。
 * 
 */
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    // 检查是否为X轴步进电机的TIM句柄
    if (htim == &htim3)
    {
        // 停止X轴的PWM脉冲输出
        HAL_TIM_PWM_Stop_DMA(stepX.phtim, stepX.channel);
        
        
        
        // 如果X轴未处于停止状态且没有外部停止信号，则设置X轴脉冲完成标志
        if (stepX.state != Stop && Stop_Flag == 0) 
        {
            Step_X_Flag = 1;// 未运动到目标脉冲数量
        }
        else // 如果X轴处于停止状态或有外部停止信号
        {// 已运动到目标脉冲数量
            // 为X轴解锁以准备下一次运动
            Step_Unlock(&stepX);
            // 更新X轴当前位置为目标位置
            POS_X_Cur = POS_X_Tar;
            
            // 如果有外部停止信号，重置跟踪标志和待走步数，并根据运行方向调整待走步数
            if (Stop_Flag == 1) 
            {
                Track_Flag = 0;
                Step_To_Go_X = (int32_t) stepX.stepToGo * (stepX.runDir == 1 ? 1 : -1);
                Step_To_Go_Y = (int32_t) stepY.stepToGo * (stepY.runDir == 1 ? 1 : -1);
            }
            
            // 如果有待走步数标志，设置跟踪标志，增加跟踪计数，并清除待走步数标志
            if (Step_To_Go_Flag == 1) 
            {
                Track_Flag = 1;
                Track_CNT++;
                Step_To_Go_Flag = 0;
            }
        }

    }
    // 检查是否为Y轴步进电机的TIM句柄
	if (htim == &htim2)
    {
        // 停止Y轴的PWM脉冲输出
        HAL_TIM_PWM_Stop_DMA(stepY.phtim, stepY.channel);
        
        
        
        // 如果Y轴未处于停止状态且没有外部停止信号，则设置Y轴脉冲完成标志
        if (stepY.state != Stop && Stop_Flag == 0) 
        {
            Step_Y_Flag = 1;
        }
        else // 如果Y轴处于停止状态或有外部停止信号
        {
            // 为Y轴解锁以准备下一次运动
            Step_Unlock(&stepY);
            // 更新Y轴当前位置为目标位置
            POS_Y_Cur = POS_Y_Tar;
            
            // 如果有外部停止信号，重置跟踪标志和待走步数，并根据运行方向调整待走步数
            if (Stop_Flag == 1) 
            {
                Track_Flag = 0;
                Step_To_Go_X = (int32_t) stepX.stepToGo * (stepX.runDir == 1 ? 1 : -1);
                Step_To_Go_Y = (int32_t) stepY.stepToGo * (stepY.runDir == 1 ? 1 : -1);
            }
            
            // 如果有待走步数标志，设置跟踪标志，增加跟踪计数，并清除待走步数标志
            if (Step_To_Go_Flag == 1) 
            {
                Track_Flag = 1;
                Track_CNT++;
                Step_To_Go_Flag = 0;
            }
        }
			

    }
}



//***********************








