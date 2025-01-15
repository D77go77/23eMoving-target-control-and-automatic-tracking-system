#ifndef _MODE_H
#define _MODE_H


#include "common_inc.h"

#define X_REALCENT 305
#define Y_REALCENT 178//����ֱ//��������ԭ��

float myabs(float tmp);

extern volatile u8 Step_X_Flag , Step_Y_Flag ;

// ģʽѡ����������ڿ���X,Y����˶�ģʽ
extern int16 Mode_1_XY ;

// mode
extern uint8_t Mode ; // 0:����; 1:��Ѱ��
// control
extern int16_t Pulse_X_Num , Pulse_Y_Num ;
extern uint16_t Pulse_Time ;
extern uint16_t Screen_Dis ;
extern int16_t Green_Dis ;
extern int16_t Green_Dis_Val[14];
extern uint8_t Stop_CNT;
extern uint8_t Stop_Flag;
// Laser
extern uint8_t Laser_Wait_CNT;

//�ɼ����ĸ�������󣬸�������ϵ
void Reset_New_Pos(void);


extern u8 Mode;
void mode_mainloop(void);
void Step_Center_Init(void);

#endif
