#ifndef _MODE_H
#define _MODE_H


#include "common_inc.h"

#define X_REALCENT 305
#define Y_REALCENT 178//纯垂直//计算点相对原点

float myabs(float tmp);

extern volatile u8 Step_X_Flag , Step_Y_Flag ;

// 模式选择变量，用于控制X,Y轴的运动模式
extern int16 Mode_1_XY ;

// 定义四个顶点位置
extern int16_t Edge_POS[4][2];

// 定义串口接受坐标的函数
extern int16_t Rex_POS[9][2] ;

//定义小框坐标
extern int16_t Box_POS[4][2];

// 定义轨迹点数组，用于存储运动轨迹
extern int16_t Track_POS[41][2] ;

// 当前轨迹点索引
extern int16 Track_CNT ;

// 轨迹运动标志位，用于标记是否正在执行轨迹运动
extern int16 Track_Flag ;

// 选择要显示的图像，用于控制显示内容
extern int16 IMG_Chose ;

// 当前位置和目标位置
extern int16_t POS_X_Cur , POS_Y_Cur ;
extern int16_t POS_X_Tar , POS_Y_Tar ;

extern int16_t POS_X_Lazer_Cent;
extern int16_t POS_Y_Lazer_Cent;//激光中点

// X,Y轴的脉冲数量，用于控制步进电机的精细移动
extern int16 Pulse_X_Num , Pulse_Y_Num ;

// X,Y轴的脉冲计数，用于跟踪步进电机的移动进度
extern int16 Pulse_X_CNT , Pulse_Y_CNT ;

// 脉冲发生的时间间隔，用于控制步进电机的速度
extern uint16_t Pulse_Time;
extern uint16_t Pulse_Time_Real;

// 屏幕显示距离，用于计算物体在屏幕上的位置
extern uint16_t Screen_Dis;

// 停止标志位，用于标记是否应该停止运动
extern u8 Stop_Flag ;

// Mode4的RX标志位，用于特定模式下的通信处理
extern u8 Mode4_RX_Flag ;

// X,Y轴还需移动的步数，用于计算剩余移动距离
extern  int32_t Step_To_Go_X , Step_To_Go_Y ;

// 还需移动标志位，用于标记是否还有剩余移动距离
extern uint8_t Step_To_Go_Flag;

//采集到四个点坐标后，更新坐标系
void Reset_New_Pos(void);


extern u8 Mode;
void mode_mainloop(void);
void Step_Center_Init(void);

#endif
