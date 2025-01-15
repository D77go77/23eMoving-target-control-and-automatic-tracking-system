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
		angle_x=GetAngle(ANGLE_X)/PI*180.0;//��������ȡ
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
 * @brief UART������ɻص�����
 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == huart2.Instance)
    {
        // ���ý�������������յ�������
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
				Reset_New_Pos();//��������󣬸�������ϵ
				HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
				// ͨ����תGPIOA�ĵ�4��LED��ָʾ���ݴ������
			}
		}
        HAL_UART_Receive_IT(&huart2, &UART2_Rx, 1); // ����������һ����
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
    // ����Ƿ�ΪX�Ჽ�������TIM���
    if (htim == &htim3)
    {
        // ֹͣX���PWM�������
        HAL_TIM_PWM_Stop_DMA(stepX.phtim, stepX.channel);
        
        
        
        // ���X��δ����ֹͣ״̬��û���ⲿֹͣ�źţ�������X��������ɱ�־
        if (stepX.state != Stop && Stop_Flag == 0) 
        {
            Step_X_Flag = 1;// δ�˶���Ŀ����������
        }
        else // ���X�ᴦ��ֹͣ״̬�����ⲿֹͣ�ź�
        {// ���˶���Ŀ����������
            // ΪX�������׼����һ���˶�
            Step_Unlock(&stepX);
            // ����X�ᵱǰλ��ΪĿ��λ��
            POS_X_Cur = POS_X_Tar;
            
            // ������ⲿֹͣ�źţ����ø��ٱ�־�ʹ��߲��������������з���������߲���
            if (Stop_Flag == 1) 
            {
                Track_Flag = 0;
                Step_To_Go_X = (int32_t) stepX.stepToGo * (stepX.runDir == 1 ? 1 : -1);
                Step_To_Go_Y = (int32_t) stepY.stepToGo * (stepY.runDir == 1 ? 1 : -1);
            }
            
            // ����д��߲�����־�����ø��ٱ�־�����Ӹ��ټ�������������߲�����־
            if (Step_To_Go_Flag == 1) 
            {
                Track_Flag = 1;
                Track_CNT++;
                Step_To_Go_Flag = 0;
            }
        }

    }
    // ����Ƿ�ΪY�Ჽ�������TIM���
	if (htim == &htim2)
    {
        // ֹͣY���PWM�������
        HAL_TIM_PWM_Stop_DMA(stepY.phtim, stepY.channel);
        
        
        
        // ���Y��δ����ֹͣ״̬��û���ⲿֹͣ�źţ�������Y��������ɱ�־
        if (stepY.state != Stop && Stop_Flag == 0) 
        {
            Step_Y_Flag = 1;
        }
        else // ���Y�ᴦ��ֹͣ״̬�����ⲿֹͣ�ź�
        {
            // ΪY�������׼����һ���˶�
            Step_Unlock(&stepY);
            // ����Y�ᵱǰλ��ΪĿ��λ��
            POS_Y_Cur = POS_Y_Tar;
            
            // ������ⲿֹͣ�źţ����ø��ٱ�־�ʹ��߲��������������з���������߲���
            if (Stop_Flag == 1) 
            {
                Track_Flag = 0;
                Step_To_Go_X = (int32_t) stepX.stepToGo * (stepX.runDir == 1 ? 1 : -1);
                Step_To_Go_Y = (int32_t) stepY.stepToGo * (stepY.runDir == 1 ? 1 : -1);
            }
            
            // ����д��߲�����־�����ø��ٱ�־�����Ӹ��ټ�������������߲�����־
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








