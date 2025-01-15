#include "mode.h"


// Stepper motor
stepTypedef stepX, stepY;
volatile uint8_t Step_X_Flag = 0, Step_Y_Flag = 0;

// mode
int16 Mode_1_XY = 0;
int16_t Edge_POS[4][2] = {{240, -280},
                          {240,  220},
                          {-240,  220},
                          {-260, -280}};

int16_t Rex_POS[9][2] = {0};
int16_t Box_POS[4][2] = {{-175, -80},
                          {-40,  -220},
                          {170,  -10},
                          {21, 130}};
int16_t Track_POS[41][2] = {0};
int16 Track_CNT = 0;
int16 Track_Flag = 0;
int16 IMG_Chose = 0;
// control
int16_t POS_X_Lazer_Cent=318,POS_Y_Lazer_Cent=249;//激光中点//像素中点
int16_t POS_X_Cur = 0;
int16_t POS_Y_Cur = 0;
int16_t POS_X_Tar = 0, POS_Y_Tar = 0;
int16_t Pulse_X_Num = 0, Pulse_Y_Num = 0;
int16_t Pulse_X_CNT = 0, Pulse_Y_CNT = 0;
uint16_t Pulse_Time = 1000;
uint16_t Pulse_Time_Real = 1000;
uint16_t Screen_Dis_R = 1065;//实际距离
uint16_t Screen_Dis = 1065;//像素距离
uint8_t Stop_Flag = 0;
uint8_t Mode4_RX_Flag = 0;
int32_t Step_To_Go_X = 0, Step_To_Go_Y = 0;
uint8_t Step_To_Go_Flag = 0;
//**********************************************
void mode_mainloop(void)
{

	switch(Mode)
	{
		case 0:
			break;
		case 1:
			break;
		case 2:
			break;
	}
}
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
    if (Mode == 4 && Pulse_Time == 600 && Track_CNT < 1) 
	{
        Pulse_Time_Real = 3000;
    } else 
	{
        Pulse_Time_Real = Pulse_Time;
    }
	
    if (Pulse_X_Num ==0)//无脉冲不动
        POS_X_Cur = POS_X_Tar;
    else 
	{
        float F_max = (float) myabs(Pulse_X_Num) / ((float) Pulse_Time_Real / 1000.0f);
        Step_Init(&stepX, &htim3, TIM_CHANNEL_4,
                  DIR_X_GPIO_Port, DIR_X_Pin, F_max, F_max, 100);
        if (Pulse_X_Num > 0)
            Start_PWM(&stepX, myabs(Pulse_X_Num), 0, 0);//使用减速
        else if (Pulse_X_Num < 0)
            Start_PWM(&stepX, myabs(Pulse_X_Num), 1, 0);//使用减速
    }

    if (Pulse_Y_Num == 0)//无脉冲不动
        POS_Y_Cur = POS_Y_Tar;
    else 
	{
        float F2_max = (float) myabs(Pulse_Y_Num) / ((float) Pulse_Time_Real / 1000.0f);
        Step_Init(&stepY, &htim2, TIM_CHANNEL_1,
                  DIR_Y_GPIO_Port, DIR_Y_Pin, F2_max, F2_max, 100);
        if (Pulse_Y_Num > 0)
            Start_PWM(&stepY, myabs(Pulse_Y_Num), 1, 0);//使用减速
        else if (Pulse_Y_Num < 0)
            Start_PWM(&stepY, myabs(Pulse_Y_Num), 0, 0);//使用减速
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
void Reset_New_Pos(void)
{
	POS_X_Lazer_Cent = Rex_POS[4][0]-X_REALCENT;
	POS_Y_Lazer_Cent = Rex_POS[4][1]-Y_REALCENT;
	for(int i = 0; i < 4; i++) 
	{
		// 新坐标 = 原坐标 - 中心坐标
		Rex_POS[i][0] = Rex_POS[i][0] - POS_X_Lazer_Cent;
		Rex_POS[i][1] = Rex_POS[i][1] - POS_Y_Lazer_Cent;
	}
	if(Mode==1)
		Screen_Dis=(Rex_POS[0][0]-Rex_POS[0][1])/500.0*Screen_Dis_R;
}






