#include "my_step.h"


// 定义全局电机对象
MOTOR_t Step_X,Step_Y;

//通用接口，主频72MHz，预分频值为72-1，设置PWM的脉冲频率freq(0.16-1kHz)、占空比参数 pulse (0-100)
void set_pwm_param(TIM_HandleTypeDef htim, uint32_t Channel, uint32_t freq, uint16_t duty)
{
    uint16_t prescaler = 72-1;
    uint64_t tim_clk_freq = 72000000;
    //计算PWM频率，所对应的自动重装载值   ---> ARR = 主频 / (预分频+1) / 预期PWM频率(Hz) - 1
    float pwm_freq_arr  = (tim_clk_freq * 1.0) / (prescaler+1) / freq * 1.0 - 1; 
    //计算PWM占空比，所对应比较寄存器的值 ---> CCR = 预期占空比 * (自动重装载值+1)
    //占空比则由捕获/比较寄存器（TIMx_CRx）寄存器决定。占空比:duty = Pluse / (ARR+1)
    float pwm_duty_pulse = duty * 1.0 / 100 * (pwm_freq_arr + 1);
    
    //配置PSC预分频值
    __HAL_TIM_SET_PRESCALER(&htim, prescaler);
    //配置PWM频率 ARR
    __HAL_TIM_SetAutoreload(&htim, (uint16_t)pwm_freq_arr);
    //配置PWM占空比
    __HAL_TIM_SetCompare(&htim, Channel, (uint16_t)pwm_duty_pulse);
}
/**
/**
 * @brief 启动电机，设置初始速度
 * @param freq 电机启动时的频率
 */
// 以xx频率启动电机

void motor_start(MOTOR_t *motor, float freq) 
{
    set_pwm_freq(motor,freq);
    motor->step_now = motor->step_all;
    //my_pwm_start(motor);
    motor->i = 0;
    motor->flag.run = 1;
}

/**
 * @brief 有序停止电机
 */
// 有序停止电机
void motor_stop_orderly(MOTOR_t *motor) 
{
    if (motor->flag.run) {
        motor->step_now = motor->i;
    }
}
/**
 * @brief 立即停止电机
 */
// 立刻停止电机
void motor_stop_at_once(MOTOR_t *motor) 
{
    motor->flag.run = 0;
    //my_pwm_stop(motor);
}
/**
 * @brief 控制电机移动指定圈数
 * @param v0 启动速度
 * @param v1 目标速度
 * @param d 移动的圈数
 * @return 控制结果状态码
 *         0 - 成功
 *         1 - 电机正在运行
 *         2 - 步数不足
 *         3 - 加速距离不足
 */
// 控制电机移动指定圈数
// 输入 启动转速 目标转速(RPM) 转动圈数 （启动转速不能取0，目标转速越大，计算时间越长，需要的内存越多）
// 输出 成功0 否则失败	1电机忙碌  2步数不够
u8 motor_move_fixed_length(MOTOR_t *motor, float v0,float v1,float d)
{
    //DIR_Y=1;
    // 判断电机是否空闲
    if(motor->flag.run)        return 1;

    // 填充运动控制参数
    motor->up.V0=(float)PULSE_PER_CIRCLE*v0/60.0;
    motor->up.V1=(float)PULSE_PER_CIRCLE*v1/60.0;
    motor->up.T=S_TIME_MS;
    motor->step_all=(float)PULSE_PER_CIRCLE*d;

    // 计算加减速表
    if(get_s_table((S_STEP_t *)&motor->up)!=0)
        return 2;
    if(motor->step_all<=2*motor->up.s){
        return 3;
    }
    //DIR_Y=0;

    // 开始运动
    motor_start(motor,motor->up.vTable[0]);
    
    return 0;
}

/**
 * @brief 计算S型加减速曲线所需的参数表
 * @param info 加减速参数结构体
 * @return 计算结果状态码
 *         0 - 成功
 *         1 - 脉冲数量超过表长度
 */
// 最低频率 最高频率 时间
u8 get_s_table(S_STEP_t *info)
{
    u32 i,j;
    float k;
    float T=info->T;
    float v,s;                // 速度，总脉冲
    float m,n;                // 速度的三次函数高项系数
//    float x[4];                // 用于保存四次函数根
//    int num;                // 四次函数根的数量
    equation4_t dataForm;
    
    // 计算三次函数的系数
    k=6.0*(float)((float)info->V0-(float)info->V1)/(float)pow(info->T,3.0);
    
    // 初始化四次方程参数
    m=k/(float)3;
    n=-k*T/(float)2;
    dataForm.aIn=k/(float)12000;
    dataForm.bIn=-k*T/(float)6000;
    dataForm.cIn=0;
    dataForm.dIn=(float)info->V0/(float)1000;
    dataForm.eIn=0;
    
    // 计算总脉冲数量
    s=dataForm.aIn*T*T*T*T+dataForm.bIn*T*T*T+dataForm.cIn*T;
    info->s=(int)s+1;
    if(info->s>TABLE_LENGTH){
        return 1;
    }

    // 计算每个脉冲对应的速度
    for(i=1;i<info->s;i++){
        dataForm.eIn=-(float)i;
        if(quad4solve(&dataForm)==0){
            for(j=0;j<4;j++){
                if(dataForm.xIm[j]==0&&dataForm.xRe[j]>=0&&dataForm.xRe[j]<=T){
                    info->vTable[i]=m*dataForm.xRe[j]*dataForm.xRe[j]*dataForm.xRe[j]+n*dataForm.xRe[j]*dataForm.xRe[j]+info->V0;
                    break;
                }
            }
        }
        else{
            info->vTable[i]=info->vTable[i-1];    // 如果无法计算，使用前一个速度
        }
    }
    info->vTable[0]=info->V0;                // 设置初始速度
    info->vTable[info->s-1]=info->V1;    
    
    return 0;
}

/**
 * @brief 电机每步回调函数，用于记录脉冲数量和控制速度
 */
// 中断中记录脉冲次数
void motor_1pulse_callback(MOTOR_t *motor)// 移动任务完成，关闭PWM输出
{
    if(motor->step_now==0)
	{                
        motor_stop_at_once(motor);
    }
    else{
        motor->step_now--;                // 未完成，步数减1
        if(motor->step_now<motor->up.s-1){    // 减速阶段
            //set_pwm_freq(motor->up.vTable[motor->step_now]);
        }
        else if(motor->i<motor->up.s-1){    // 加速阶段
            motor->i++;
            //set_pwm_freq(motor->up.vTable[motor->i]);
        }
    }
} 

// 百度来的高效Sqrt，实测比math库的快一点点，结果也有细微区别
float mSqrt(float x)
{
  float xhalf = 0.5f*x;
  int i = *(int*)&x; // get bits for floating VALUE
  i = 0x5f375a86- (i>>1); // gives initial guess y0
  x = *(float*)&i; // convert bits BACK to float
  x = x*(1.5f-xhalf*x*x); // Newton step, repeating increases accuracy
  return 1/x;
} 

// 解一元四次函数
u8 quad4solve(equation4_t *dataForm)
{
	float a = dataForm->aIn;
	float b = dataForm->bIn;
	float c = dataForm->cIn;
	float d = dataForm->dIn;
	float e = dataForm->eIn;
	if (a == 0)
	{
		//printf("The coefficient of the power four of x is 0. Please use the utility for a third degree quadratic.");
		return 1;
	} 
	if (e == 0)
	{
		//printf("One root is 0. Now divide through by x and use the utility for a third degree quadratic to solve the resulting equation for the other three roots.");
		return 2;
	} 
	if (a != 1) 
	{
		b /= a;
		c /= a;
		d /= a;
		e /= a;
	}

// Coefficients for cubic solver
	float cb, cc, cd;  
	float discrim, q, r, RRe, RIm, DRe, DIm, dum1, ERe, EIm, s, t, term1, r13, sqR, y1, z1Re, z1Im, z2Re;
	cb = -c;
	cc = -4.0*e + d*b;
	cd = -(b*b*e + d*d) + 4.0*c*e;
	if (cd == 0)  
	{
		//printf("cd = 0.");
	}
	q = (3.0*cc - (cb*cb))/9.0;
	r = -(27.0*cd) + cb*(9.0*cc - 2.0*(cb*cb));
	r /= 54.0;
	discrim = q*q*q + r*r;
	term1 = (cb/3.0);
	if (discrim > 0) 
	{ 
// 1 root real, 2 are complex
		s = r + mSqrt(discrim);
		s = ((s < 0) ? -pow(-s, (1.0/3.0)) : pow(s, (1.0/3.0)));
		t = r - mSqrt(discrim);
		t = ((t < 0) ? -pow(-t, (1.0/3.0)) : pow(t, (1.0/3.0)));
		y1 = -term1 + s + t;
		} 
		else 
		{
			if (discrim == 0) 
			{
				r13 = ((r < 0) ? -pow(-r,(1.0/3.0)) : pow(r,(1.0/3.0)));
				y1 = -term1 + 2.0*r13;
			} 
			else 
			{                             	
				q = -q;
				dum1 = q*q*q;
				dum1 = acos(r/mSqrt(dum1));
				r13 = 2.0*mSqrt(q);
				y1 = -term1 + r13*cos(dum1/3.0);
			}
		} 
// Determined y1, a real root of the resolvent cubic.
		term1 = b/4.0;
		sqR = -c + term1*b + y1;  
		RRe = RIm = DRe = DIm = ERe = EIm = z1Re = z1Im = z2Re = 0;
		if (sqR >= 0) 
		{
			if (sqR == 0) 
			{
				dum1 = -(4.0*e) + y1*y1;
				if (dum1 < 0) //D and E will be complex
	   			z1Im = 2.0*mSqrt(-dum1);
				else 
				{                      //else (dum1 >= 0)
	  				z1Re = 2.0*mSqrt(dum1);
					z2Re = -z1Re;
				}
			} 
			else 
			{                       
			RRe = mSqrt(sqR);
			z1Re = -(8.0*d + b*b*b)/4.0 + b*c;
			z1Re /= RRe;
			z2Re = -z1Re;
		 } 
	} 
	else 
	{                           
		RIm = mSqrt(-sqR);
		z1Im = -(8.0*d + b*b*b)/4.0 + b*c;
		z1Im /= RIm;
		z1Im = -z1Im;
	} 
	z1Re += -(2.0*c + sqR) + 3.0*b*term1;
	z2Re += -(2.0*c + sqR) + 3.0*b*term1;

//At this point, z1 and z2 should be the terms under the square root for D and E
	if (z1Im == 0)
	{               // Both z1 and z2 real
		if (z1Re >= 0)
		{
			DRe = mSqrt(z1Re);
		}
		else
		{
			DIm = mSqrt(-z1Re);
		}
		if (z2Re >= 0)
		{
			ERe = mSqrt(z2Re);
		}
		else
		{
			EIm = mSqrt(-z2Re);
		}
	}
	else 
	{                      
		r = mSqrt(z1Re*z1Re + z1Im*z1Im); 
		r = mSqrt(r);
		dum1 = atan2(z1Im, z1Re); 
		dum1 /= 2; //Divide this angle by 2
		ERe = DRe = r*cos(dum1); 
		DIm = r*sin(dum1);
		EIm = -DIm;
	} 
	dataForm->xRe[0] = -term1 + (RRe + DRe)/2;
	dataForm->xIm[0] = (RIm + DIm)/2;
	dataForm->xRe[1] = -(term1 + DRe/2) + RRe/2;
	dataForm->xIm[1] = (-DIm + RIm)/2;
	dataForm->xRe[2] = -(term1 + RRe/2) + ERe/2;
	dataForm->xIm[2] = (-RIm + EIm)/2;
	dataForm->xRe[3] = -(term1 + (RRe + ERe)/2);
	dataForm->xIm[3] = -(RIm + EIm)/2;
    return 0;
}

/* 设置PWM频率 */
void set_pwm_freq(MOTOR_t *motor, double freq)
{
    u32 ccr;  /* 用于设置占空比 */
    u32 arr;  /* 用于设置频率 */

    /* 更新给定电机的速度值 */
    motor->vnow = freq;

    /* 计算自动重载寄存器值 */
    arr = 12000000.0 / freq;
    ccr = arr / 2.0 - 1;
    arr -= 1;

    /* 设置自动重载寄存器和比较寄存器值，这里仍直接操作TIM2寄存器，假设这是全局控制的TIM2 */
    TIM2->ARR = arr;
    TIM2->CCR3 = ccr;
}






