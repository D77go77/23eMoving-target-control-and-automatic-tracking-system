#ifndef _MY_STEP_H
#define _MY_STEP_H

#include "common_inc.h"

#define PULSE_PER_CIRCLE	800U		//ÿȦ������
#define S_TIME_MS			100U		//s�μ���ʱ��
#define TABLE_LENGTH		2000U		//�ٶȱ���󳤶�

typedef struct
{
	float V0;			//��ʼ�ٶ�	Hz
	float V1;			//Ŀ���ٶ�
	u16 T;				//�Ӽ���ʱ�� ms
	float vTable[TABLE_LENGTH];		//�ٶȱ�
	u32 s;				//�׶���������
}S_STEP_t;				//���Ϊֱ���˶�����


typedef struct
{
	struct
	{
		u8 run:1;
		u8 sta:2;
		u8 stop_orderly:1;
	}flag;
	
	S_STEP_t up;
	u16 i;			//��ǰ�ٶȱ��±�
	u32 step_all;	//�������������� 
	u32 step_now;	//ʣ���������� Ϊ0ʱ�ر�pwm���
	float vnow;		//��ǰ�ٶ�
}MOTOR_t;

extern  MOTOR_t Step_X,Step_Y;//

//���ڽ�һԪ�Ĵη���
typedef struct
{
	float aIn,bIn,cIn,dIn,eIn;		//43210�����ϵ��
	float xRe[4],xIm[4];			//�ĸ�����reʵ�� im�鲿
}equation4_t;



void motor_init(void);				//��ʼ��io���붨ʱ��
void motor_start(MOTOR_t *motor, float freq);		//��freqƵ���������
void motor_stop_at_once(MOTOR_t *motor)	;	//����ֹͣ ֱ��ֹͣ�������
void motor_stop_orderly(MOTOR_t *motor) ;	//����ֹͣ �����������
		

u8 get_s_table(S_STEP_t *info);
u8 motor_move_fixed_length(MOTOR_t *motor, float v0,float v1,float d);	//�ƶ��̶�Ȧ��
void motor_1pulse_callback(MOTOR_t *motor);		//�����жϻص�
u8 quad4solve(equation4_t *dataForm);	//��һԪ�Ĵη���

void set_pwm_freq(MOTOR_t *motor, double freq);






#endif








