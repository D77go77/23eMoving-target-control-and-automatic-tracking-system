#ifndef __CONTROL_H
#define __CONTROL_H

#include "common_inc.h"


void Straight_2_Sphere_Degree(int16_t P0S_X, int16_t P0S_Y, float *DEG_X, float *DEG_Y) ;

void POS_2_Pulse(float DEG_X, float DEG_Y, int16_t *Pulse_X_Num, int16_t *Pulse_Y_Num);

void Position_Subdivide(const int16_t Input_POS[4][2], int16_t Output_POS[41][2]);

extern u8 Step_Next_Flag_X,Step_Next_Flag_Y;
extern u8 Step_Lock_Flag;

void Step_LockAndUnLock(void);
void Step_MyLock(void);

#endif


