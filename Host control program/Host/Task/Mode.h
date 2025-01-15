#ifndef _MODE_H
#define _MODE_H


#include "common_inc.h"

#define X_REALCENT 305
#define Y_REALCENT 178//����ֱ//��������ԭ��

float myabs(float tmp);

extern volatile u8 Step_X_Flag , Step_Y_Flag ;

// ģʽѡ����������ڿ���X,Y����˶�ģʽ
extern int16 Mode_1_XY ;

// �����ĸ�����λ��
extern int16_t Edge_POS[4][2];

// ���崮�ڽ�������ĺ���
extern int16_t Rex_POS[9][2] ;

//����С������
extern int16_t Box_POS[4][2];

// ����켣�����飬���ڴ洢�˶��켣
extern int16_t Track_POS[41][2] ;

// ��ǰ�켣������
extern int16 Track_CNT ;

// �켣�˶���־λ�����ڱ���Ƿ�����ִ�й켣�˶�
extern int16 Track_Flag ;

// ѡ��Ҫ��ʾ��ͼ�����ڿ�����ʾ����
extern int16 IMG_Chose ;

// ��ǰλ�ú�Ŀ��λ��
extern int16_t POS_X_Cur , POS_Y_Cur ;
extern int16_t POS_X_Tar , POS_Y_Tar ;

extern int16_t POS_X_Lazer_Cent;
extern int16_t POS_Y_Lazer_Cent;//�����е�

// X,Y����������������ڿ��Ʋ�������ľ�ϸ�ƶ�
extern int16 Pulse_X_Num , Pulse_Y_Num ;

// X,Y���������������ڸ��ٲ���������ƶ�����
extern int16 Pulse_X_CNT , Pulse_Y_CNT ;

// ���巢����ʱ���������ڿ��Ʋ���������ٶ�
extern uint16_t Pulse_Time;
extern uint16_t Pulse_Time_Real;

// ��Ļ��ʾ���룬���ڼ�����������Ļ�ϵ�λ��
extern uint16_t Screen_Dis;

// ֹͣ��־λ�����ڱ���Ƿ�Ӧ��ֹͣ�˶�
extern u8 Stop_Flag ;

// Mode4��RX��־λ�������ض�ģʽ�µ�ͨ�Ŵ���
extern u8 Mode4_RX_Flag ;

// X,Y�ỹ���ƶ��Ĳ��������ڼ���ʣ���ƶ�����
extern  int32_t Step_To_Go_X , Step_To_Go_Y ;

// �����ƶ���־λ�����ڱ���Ƿ���ʣ���ƶ�����
extern uint8_t Step_To_Go_Flag;

//�ɼ����ĸ�������󣬸�������ϵ
void Reset_New_Pos(void);


extern u8 Mode;
void mode_mainloop(void);
void Step_Center_Init(void);

#endif
