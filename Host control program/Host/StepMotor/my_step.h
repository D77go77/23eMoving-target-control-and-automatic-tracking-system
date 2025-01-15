#ifndef _MY_STEP_H
#define _MY_STEP_H

#include "common_inc.h"

#define PULSE_PER_CIRCLE	800U		//每圈脉冲数
#define S_TIME_MS			100U		//s形加速时间
#define TABLE_LENGTH		2000U		//速度表最大长度

typedef struct
{
	float V0;			//起始速度	Hz
	float V1;			//目标速度
	u16 T;				//加减速时间 ms
	float vTable[TABLE_LENGTH];		//速度表
	u32 s;				//阶段脉冲总数
}S_STEP_t;				//类比为直线运动计算


typedef struct
{
	struct
	{
		u8 run:1;
		u8 sta:2;
		u8 stop_orderly:1;
	}flag;
	
	S_STEP_t up;
	u16 i;			//当前速度表下标
	u32 step_all;	//过程总脉冲数量 
	u32 step_now;	//剩余脉冲数量 为0时关闭pwm输出
	float vnow;		//当前速度
}MOTOR_t;

extern  MOTOR_t Step_X,Step_Y;//

//用于解一元四次方程
typedef struct
{
	float aIn,bIn,cIn,dIn,eIn;		//43210次项的系数
	float xRe[4],xIm[4];			//四个根，re实部 im虚部
}equation4_t;



void motor_init(void);				//初始化io口与定时器
void motor_start(MOTOR_t *motor, float freq);		//以freq频率启动电机
void motor_stop_at_once(MOTOR_t *motor)	;	//立刻停止 直接停止输出脉冲
void motor_stop_orderly(MOTOR_t *motor) ;	//有序停止 进入减速流程
		

u8 get_s_table(S_STEP_t *info);
u8 motor_move_fixed_length(MOTOR_t *motor, float v0,float v1,float d);	//移动固定圈数
void motor_1pulse_callback(MOTOR_t *motor);		//更新中断回调
u8 quad4solve(equation4_t *dataForm);	//解一元四次方程

void set_pwm_freq(MOTOR_t *motor, double freq);






#endif








