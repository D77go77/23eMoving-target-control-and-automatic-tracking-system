#ifndef __STEPHELP_H__
#define __STEPHELP_H__

#include "common_inc.h"

#define BufferSize (200)

#define M_PI (3.1415926535f)

#define Step_Free stepX.lock == UNLOCK && stepY.lock == UNLOCK//都解锁才能动

//定义步进电机控制结构体
typedef struct stepTypedef {
    //定时器句柄，用于产生脉冲
    TIM_HandleTypeDef *phtim;
    //定时器通道，用于选择定时器的哪个通道产生脉冲
    uint32_t channel;

    //GPIO端口，用于输出脉冲
    GPIO_TypeDef *gpioPort;
    //GPIO引脚，用于选择具体的GPIO引脚输出脉冲
    uint16_t gpioPin;

    //最小频率，单位为Hz
    float Fmin; 
    //最大频率，单位为Hz
    float Fmax; 
    //加速时间，单位为ms
    float Tacc; 

    //当前时间，单位为ms，内部使用
    float t; 
    //当前频率，单位为Hz，内部使用
    float Fcur; 

    //缓冲区0，用于存储脉冲宽度
    uint16_t buff0[BufferSize]; 
    //缓冲区1，用于存储脉冲宽度
    uint16_t buff1[BufferSize]; 

    //状态枚举，包括加速、恒速、减速和停止
    enum fillState {
        Acclerate,
        Constant,
        Decelerate,
        Stop
    } state; 

    //运行锁枚举，用于控制运行状态
    enum runningLock {
        UNLOCK,
        LOCK
    } lock;

    //剩余步数，用于控制步进电机的移动距离
    uint32_t stepToGo;
    //当前使用的缓冲区索引
    uint32_t buffToUse;
    //加速步数，用于计算加速过程中的步数
    uint32_t accStep;

    //运行方向标志位
    uint8_t runDir: 1;
    //是否使用减速标志位
    uint8_t useDec: 1;
    //是否作为电机运行标志位
    uint8_t useAsMotor: 1;
    //当前缓冲区索引标志位
    uint8_t buffIndex: 1; 
    //缓冲区是否准备好的标志位
    uint8_t buffRdy: 1; 
} stepTypedef;

extern stepTypedef stepX, stepY;

/**
 * 初始化步进电机驱动
 * @param hstep 步进电机控制结构体指针
 * @param phtim TIM控制器指针
 * @param channel TIM通道号
 * @param GPIOx GPIO控制器指针
 * @param GPIO_Pin GPIO引脚号
 * @param Fmin 最小频率
 * @param Fmax 最大频率
 * @param Tacc 加速时间
 */
void Step_Init(stepTypedef *hstep, TIM_HandleTypeDef *phtim, 
                uint32_t channel, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin,
               float Fmin, float Fmax, float Tacc);

/**
 * 标记步进电机缓冲区正在使用
 * @param hstep 步进电机控制结构体指针
 */
void Step_BufferUsed(stepTypedef *hstep);

/**
 * 检查步进电机缓冲区是否准备就绪
 * @param hstep 步进电机控制结构体指针
 * @return 缓冲区准备状态
 */
int Step_IsBuffRdy(stepTypedef *hstep);

/**
 * 获取步进电机的当前方向
 * @param hstep 步进电机控制结构体指针
 * @return 步进电机方向
 */
int Step_GetDir(stepTypedef *hstep);

/**
 * 获取步进电机当前使用的缓冲区指针
 * @param hstep 步进电机控制结构体指针
 * @return 缓冲区指针
 */
uint16_t *Step_GetCurBuffer(stepTypedef *hstep);

/**
 * 获取步进电机缓冲区已使用长度
 * @param hstep 步进电机控制结构体指针
 * @return 缓冲区已使用长度
 */
uint32_t Step_BuffUsedLength(stepTypedef *hstep);

/**
 * 尝试锁定步进电机控制结构体
 * @param hstep 步进电机控制结构体指针
 * @return 锁定状态
 */
int Step_Lock(stepTypedef *hstep);

/**
 * 尝试解锁步进电机控制结构体
 * @param hstep 步进电机控制结构体指针
 * @return 解锁状态
 */
int Step_Unlock(stepTypedef *hstep);

/**
 * 中止步进电机当前动作
 * @param hstep 步进电机控制结构体指针
 */
void Step_Abort(stepTypedef *hstep);

/**
 * 预填充步进电机缓冲区
 * @param hstep 步进电机控制结构体指针
 * @param stepToGo 需要移动的步数
 * @param dir 移动方向
 * @param useDec 是否使用减速
 * @return 预填充状态
 */
int Step_Prefill(stepTypedef *hstep, uint32_t stepToGo, uint8_t dir, uint8_t useDec);

/**
 * 填充步进电机缓冲区
 * @param hstep 步进电机控制结构体指针
 * @return 缓冲区填充状态
 */
int Step_BuffFill(stepTypedef *hstep);

/**
 * 填充步进电机减速区
 * @param hstep 步进电机控制结构体指针
 * @return 减速区填充状态
 */
int Step_FillDecelerate(stepTypedef *hstep);

/**
 * 填充步进电机恒速区
 * @param hstep 步进电机控制结构体指针
 * @return 恒速区填充状态
 */
int Step_FillConstant(stepTypedef *hstep);

/**
 * 填充步进电机加速区
 * @param hstep 步进电机控制结构体指针
 * @return 加速区填充状态
 */
int Step_FillAccelerate(stepTypedef *hstep);

//
void Start_PWM(stepTypedef *hstep, uint32_t stepToGo, uint8_t dir, uint8_t useDec);
void Pulse_2_PWM(void);

#endif


