#include "mode.h"
// Stepper motor
stepTypedef stepX, stepY;
volatile uint8_t Step_X_Flag = 0, Step_Y_Flag = 0;

// control
int16_t Pulse_X_Num = 0, Pulse_Y_Num = 0;
uint16_t Pulse_Time = 1000;
uint16_t Screen_Dis = 1070;
int16_t Green_Dis = 1070;
int16_t Green_Dis_Val[14] = {-1000, -900, -800, -700, -600, -500, -400,
                             400, 500, 600, 700, 800, 900, 1000};
uint8_t Stop_CNT = 0;
uint8_t Stop_Flag = 0;
// Laser
uint8_t Laser_Wait_CNT = 0;
/**********************************************************************************/

void Start_PWM(stepTypedef *hstep, uint32_t stepToGo, uint8_t dir, uint8_t useDec) 
{
    Step_Prefill(hstep, stepToGo, dir, useDec);

	if (hstep == &stepX)
	{
        Step_X_Flag = 1;
	}
    if (hstep == &stepY)
	{
        Step_Y_Flag = 1;
	}
}
/**********************************************************************************/
void Pulse_2_PWM(void) 
{	
    if (Pulse_X_Num !=0)//无脉冲不动
	{
        float F_max = (float) myabs(Pulse_X_Num) / ((float) Pulse_Time / 1000.0f);
        Step_Init(&stepX, &htim3, TIM_CHANNEL_4,
                  DIR_X_GPIO_Port, DIR_X_Pin, 10, F_max, 100);
        if (Pulse_X_Num > 0)
            Start_PWM(&stepX, myabs(Pulse_X_Num), 0, 1);//使用减速
        else if (Pulse_X_Num < 0)
            Start_PWM(&stepX, myabs(Pulse_X_Num), 1, 1);//使用减速
    }

    if (Pulse_Y_Num != 0)//无脉冲不动
	{
        float F2_max = (float) myabs(Pulse_Y_Num) / ((float) Pulse_Time / 1000.0f);
        Step_Init(&stepY, &htim2, TIM_CHANNEL_1,
                  DIR_Y_GPIO_Port, DIR_Y_Pin, 10, F2_max, 100);
        if (Pulse_Y_Num > 0)
            Start_PWM(&stepY, myabs(Pulse_Y_Num), 1, 1);//使用减速
        else if (Pulse_Y_Num < 0)
            Start_PWM(&stepY, myabs(Pulse_Y_Num), 0, 1);//使用减速
    }
}
/**********************************************************************************/
float myabs(float tmp)
{
	if(tmp>0)
		return tmp;
	else
		return -tmp;
}
/**********************************************************************************/







