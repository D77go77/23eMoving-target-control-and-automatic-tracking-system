#include "Pid.h"
#include "math.h"

INC_PID PID_X, PID_Y;
////////////////////////////////    Positional PID    ///////////////////////////////////////////
void PSI_PID_Init(PSI_PID *psi_pid, float kp, float ki, float kd) 
{
    psi_pid->kp = kp;
    psi_pid->ki = ki;
    psi_pid->kd = kd;
}

void PSI_PID_Callback(PSI_PID *psi_pid) 
{
    float error = psi_pid->target - psi_pid->real;
    float change_error = error - psi_pid->error;
    psi_pid->error = error;
    psi_pid->sum_error += error;
    if (psi_pid->sum_error_limt) 
	{
        psi_pid->sum_error = psi_pid->sum_error > psi_pid->sum_error_limt_val ? psi_pid->sum_error_limt_val : psi_pid->sum_error;
        psi_pid->sum_error = psi_pid->sum_error < -psi_pid->sum_error_limt_val ? -psi_pid->sum_error_limt_val : psi_pid->sum_error;
    }
    psi_pid->output = psi_pid->kp * psi_pid->error + psi_pid->ki * psi_pid->sum_error + psi_pid->kd * change_error;
}

void PSI_PID_Reset(PSI_PID *psi_pid) 
{
    psi_pid->error = 0;
    psi_pid->sum_error = 0;
}

void PSI_PID_Set_Target(PSI_PID *psi_pid, float target) 
{
    psi_pid->target = target;
}

////////////////////////////////    Increment PID    ///////////////////////////////////////////
void INC_PID_Init(INC_PID *incPid, float kp, float ki, float kd) 
{
    incPid->kp = kp;
    incPid->ki = ki;
    incPid->kd = kd;
}

void INC_PID_Callback(INC_PID *incPid) 
{


    incPid->error_k = incPid->target - incPid->real;
    incPid->sum_error += incPid->error_k;
    incPid->output = incPid->kp * incPid->error_k + incPid->ki * incPid->sum_error + incPid->kd * (incPid->error_k - incPid->error_k_1);
	if(incPid->output>6)incPid->output=6;
	if(incPid->output<-6)incPid->output=-6;
    incPid->error_k_1 = incPid->error_k;

    if (incPid->sum_error > 100)
        incPid->sum_error = 100;
    if (incPid->sum_error < -100)
        incPid->sum_error = -100;

}

void INC_PID_Set_Target(INC_PID *incPid, float target) 
{
    incPid->target = target;
}
