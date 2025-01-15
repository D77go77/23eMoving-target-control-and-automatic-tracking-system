#ifndef _PID_H
#define _PID_H


#include "common_inc.h"

////////////////////////////////    Positional PID    ///////////////////////////////////////////
typedef struct PSI_PID {
    float target;
    float real;
    float kp;
    float ki;
    float kd;
    float error;
    float sum_error;
    float sum_error_limt;
    float sum_error_limt_val;
    float output;
} PSI_PID;
//extern PSI_PID PID_X, PID_Y;
void PSI_PID_Init(PSI_PID *psi_pid, float kp, float ki, float kd);

void PSI_PID_Callback(PSI_PID *psi_pid);

void PSI_PID_Reset(PSI_PID *psi_pid);

void PSI_PID_Set_Target(PSI_PID *psi_pid, float target);

////////////////////////////////    Increment PID    ///////////////////////////////////////////
typedef struct INC_PID {
    float target;
    float real;
    float kp;
    float ki;
    float kd;
    float error_k;
    float error_k_1;
    float error_k_2;
    float sum_error;
    float output;
} INC_PID;
extern INC_PID PID_X, PID_Y;

void INC_PID_Init(INC_PID *incPid, float kp, float ki, float kd);

void INC_PID_Callback(INC_PID *incPid);

void INC_PID_Set_Target(INC_PID *incPid, float target);


#endif



