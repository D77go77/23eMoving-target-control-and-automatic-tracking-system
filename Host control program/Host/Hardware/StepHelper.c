#include "StepHelper.h"


/* 配置选项开始 */
/*********************  Config Option Begin  ********************/

/* 如果未定义RCC_MAX_FREQUENCY，则将其默认值定义为72MHz */
#ifndef RCC_MAX_FREQUENCY
#define RCC_MAX_FREQUENCY 72000000
#endif


/* 加速曲线类型，这里选择梯形曲线 */
#define AcclerateCurve (Curve_S)

/* 配置选项结束 */
/*********************  Config Option End  ********************/

/* 如果未定义__weak，则将其定义为__attribute__((weak)) */
#ifndef __weak
#define __weak __attribute__((weak))
#endif // !__weak

/* 定义加速曲线类型宏，0x01表示梯形曲线，0x02表示S曲线 */
#define Curve_Trapezoidal (0x01)
#define Curve_S (0x02)
//**************************************
void Step_Init(stepTypedef *hstep, TIM_HandleTypeDef *phtim, uint32_t channel, 
					GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin,
						float Fmin, float Fmax, float Tacc)
{
    // 初始化结构体成员
    hstep->phtim = phtim; // 定时器句柄
    hstep->channel = channel; // 定时器通道

    hstep->gpioPort = GPIOx; // GPIO端口
    hstep->gpioPin = GPIO_Pin; // GPIO引脚

    // 根据TIM通道设置对应的比较寄存器值为定时器周期的一半
    switch (hstep->channel) 
    {
        case TIM_CHANNEL_1:
            hstep->phtim->Instance->CCR1 = 0.5f * hstep->phtim->Instance->ARR;
            break;
        case TIM_CHANNEL_2:
            hstep->phtim->Instance->CCR2 = 0.5f * hstep->phtim->Instance->ARR;
            break;
        case TIM_CHANNEL_3:
            hstep->phtim->Instance->CCR3 = 0.5f * hstep->phtim->Instance->ARR;
            break;
        case TIM_CHANNEL_4:
            hstep->phtim->Instance->CCR4 = 0.5f * hstep->phtim->Instance->ARR;
            break;

        default:
            break;
    }

    hstep->Fmin = Fmin; // 最小频率
    hstep->Fmax = Fmax; // 最大频率
    hstep->Tacc = Tacc; // 加速时间

    hstep->t = 0.0f; // 当前时间初始化
    hstep->Fcur = 0.0f; // 当前频率初始化

    // 缓冲区默认无需手动初始化，已定义时自动清零
    memset(hstep->buff0, 0, sizeof(hstep->buff0)); 
    memset(hstep->buff1, 0, sizeof(hstep->buff1)); 

    hstep->state = Stop; // 初始化状态为停止

    hstep->lock = UNLOCK; // 初始化运行锁为解锁状态

    hstep->stepToGo = 0; // 剩余步数初始化为0
    hstep->buffToUse = 0; // 当前使用的缓冲区索引初始化为0
    hstep->accStep = 0; // 加速步数初始化为0

    // 标志位初始化
    hstep->useDec = 1; // 默认启用减速
    hstep->buffIndex = 0; // 缓冲区索引标志位初始化
    hstep->buffRdy = 0; // 缓冲区准备标志位初始化为未准备
}

//**************************************
/**
 * @brief 生成加速过程的频率数组
 * 
 * 本函数根据当前步进电机的状态和设定的加速曲线，计算并生成加速过程中的频率数组。
 * 频率数组用于控制步进电机在加速阶段的速度变化，确保平滑加速。
 * 
 * @param hstep 步进电机控制句柄
 * @return int 返回实际生成的数组长度
 */
int Step_FillAccelerate(stepTypedef *hstep) 
{
    // 检查当前状态是否为加速状态，如果不是，则直接返回0
    if (hstep->state != Acclerate)
        return 0;

    // 确定要使用的缓冲区大小和指针
    int buffToUse = hstep->stepToGo > BufferSize ? BufferSize : hstep->stepToGo;
    uint16_t *buffPtr = hstep->buffIndex ? hstep->buff1 : hstep->buff0;

    // 计算加速过程中的每个频率点
    // Acclerate
    int i = 0;
    for (; hstep->t <= hstep->Tacc; i++) 
	{
        // 防止数组越界
        // overflow check
        if (i >= buffToUse)
            break;

        // 更新时间t
        // get t
        hstep->t += 1000.0f / hstep->Fcur;

        // 根据不同的加速曲线计算当前频率
        // get freq
        switch (AcclerateCurve) 
		{
            case Curve_Trapezoidal:
                hstep->Fcur = (hstep->Fmax - hstep->Fmin) * 1.0f / (hstep->Tacc) * (hstep->t) + hstep->Fmin;
                break;
            case Curve_S:
                hstep->Fcur = 0.5f * hstep->Fmax * cosf(PI - PI * hstep->t / hstep->Tacc) + hstep->Fmin +
                              hstep->Fmax * 0.5f;
                break;

            default:
                hstep->Fcur = hstep->Fmin;
                break;
        }

        // 计算并存储PSC值到缓冲区
        // get PSC & fill
        buffPtr[i] = (uint16_t) ((RCC_MAX_FREQUENCY / (hstep->phtim->Instance->ARR + 1)) / hstep->Fcur - 1);
    }

    // 切换使用哪个缓冲区
    hstep->buffIndex = !hstep->buffIndex;

    // 如果生成的数组长度等于预设的最大长度，表示加速过程已完成
    // Interrupted
    if (i >= buffToUse) {
        hstep->buffRdy = 1;
        return buffToUse;
    }

    // 设置为恒定频率状态，并填充剩余的缓冲区空间
    // Constant
    hstep->state = Constant;
    hstep->Fcur = hstep->Fmax;
    uint16_t PSC = (uint16_t) ((RCC_MAX_FREQUENCY / (hstep->phtim->Instance->ARR + 1)) / hstep->Fcur - 1);
    for (; i < buffToUse; i++) {
        buffPtr[i] = PSC;
    }

    // 标记缓冲区准备完毕
    hstep->buffRdy = 1;
    return buffToUse;
}

/**
 * @brief 匀速数组生成器
 *
 * @param hstep step句柄
 * @return int 运行步数
 */
/**
 * 步进电机恒定电流设置函数
 * 
 * 该函数用于在步进电机驱动中设置恒定电流。通过计算得到适当的预分频器值（PSC），
 * 并将该值写入指定的缓冲区，以实现电机运行时的恒定电流控制。
 * 
 * @param hstep 步进电机控制结构体指针，包含相关配置和状态信息。
 * @return 返回实际填充到缓冲区的步骤数量。
 */
int Step_FillConstant(stepTypedef *hstep) {
    // 检查步进电机是否处于恒定电流状态，如果不是，则直接返回
    if (hstep->state != Constant) {
        return 0;
    }

    // 根据当前缓冲区索引选择正确的缓冲区指针
    uint16_t *buffPtr = hstep->buffIndex ? hstep->buff1 : hstep->buff0;
    
    // 计算预分频器的值，以实现恒定电流控制
    uint16_t PSC = (uint16_t) ((RCC_MAX_FREQUENCY / (hstep->phtim->Instance->ARR + 1)) / hstep->Fcur - 1);
    
    // 确定实际要使用的缓冲区大小，防止超过缓冲区容量
    int buffToUse = hstep->stepToGo > BufferSize ? BufferSize : hstep->stepToGo;

    // 将计算得到的预分频器值填充到缓冲区
    for (int i = 0; i < buffToUse; i++) {
        buffPtr[i] = PSC;
    }

    // 评论掉的代码原用于在特定条件下设置缓冲区最后一个元素的值，现已被注释

    // 切换缓冲区索引，为下一次填充做准备
    hstep->buffIndex = !hstep->buffIndex;

    // 设置缓冲区准备就绪标志
    hstep->buffRdy = 1;
    
    // 返回实际填充的步骤数量
    return buffToUse;
}

/**
 * 减速数组生成函数
 * 
 * 该函数用于生成减速过程中的频率控制数组，以实现平滑减速效果。
 * 根据当前速度和目标位置，计算需要的时间和频率，进而确定PWM的占空比。
 * 
 * @param hstep 步进电机控制句柄，包含步进电机的相关参数和状态。
 * @return int 返回实际生成的数组长度。
 */
/**
 * @brief 减速数组生成器
 *
 * @param hstep step句柄
 * @return int 运行步数
 */
int Step_FillDecelerate(stepTypedef *hstep) {
    // 检查当前状态是否为减速状态，如果不是，则直接返回
    if (hstep->state != Decelerate)
        return 0;

    // 确定实际需要生成的数组长度，不超过缓冲区大小
    int buffToUse = hstep->stepToGo > BufferSize ? BufferSize : hstep->stepToGo;
    // 根据当前缓冲区指针的位置，选择对应的缓冲区
    uint16_t *buffPtr = hstep->buffIndex ? hstep->buff1 : hstep->buff0;

    // 减速过程循环
    // Decelerate
    int i = 0;
    for (; hstep->Fcur > hstep->Fmin; i++) {
        // 检查是否超过缓冲区大小，防止溢出
        // overflow check
        if (i >= buffToUse)
            break;

        // 计算时间t，单位为毫秒
        // get t
        hstep->t -= 1000.0f / hstep->Fcur;

        // 根据当前时间和加速度计算当前频率
        // get freq
        hstep->Fcur = (hstep->Fmax - hstep->Fmin) * 1.0f / (hstep->Tacc) * (hstep->t) + hstep->Fmin;

        // 计算PWM的预分频系数，并写入缓冲区
        // get PSC & fill
        buffPtr[i] = (uint16_t) ((RCC_MAX_FREQUENCY / (hstep->phtim->Instance->ARR + 1)) / hstep->Fcur - 1);
    }

    // 切换缓冲区指针
    hstep->buffIndex = !hstep->buffIndex;

    // 如果生成的数组长度等于缓冲区大小，表示减速过程已完成
    // Interrupted
    if (i >= buffToUse) {
        hstep->buffRdy = 1;
        return buffToUse;
    }

    // 设置当前频率为最小频率
    // Constant
    // hstep->state = Constant;
    hstep->Fcur = hstep->Fmin;
    // 计算对应的预分频系数
    uint16_t PSC = (uint16_t) ((RCC_MAX_FREQUENCY / (hstep->phtim->Instance->ARR + 1)) / hstep->Fcur - 1);
    // 填充剩余的缓冲区空间为恒定频率的预分频系数
    for (; i < buffToUse; i++) {
        buffPtr[i] = PSC;
    }

    // 设置缓冲区准备完成标志
    hstep->buffRdy = 1;

    // 返回实际生成的数组长度
    return buffToUse;
}

/**
 * @brief 当DMA使用完步进电机的缓冲区后调用此函数
 * @note 此函数为弱函数，旨在被用户实现。在RTOS环境下，建议使用二值信号量来实现Fill操作与DMA传输的同步。
 * 
 * @param hstep 指向步进电机实例的句柄
 */
__weak void Step_BufferUsed(stepTypedef *hstep) {
    /* 重置缓冲区准备状态 */
    hstep->buffRdy = 0;
    
    /* 更新剩余步数，减去刚刚使用的步数 */
    hstep->stepToGo -= hstep->buffToUse;

    /* 根据剩余步数决定电机状态 */
    if (hstep->stepToGo <= 0) 
	{
        /* 如果剩余步数小于等于0，设置电机状态为停止 */
        hstep->state = Stop;
    } else if (hstep->stepToGo <= hstep->accStep) 
	{
        /* 如果剩余步数小于等于加速步数，根据是否使用减速判断电机状态 */
        if (hstep->useDec) {
            /* 如果使用减速，设置电机状态为减速 */
            hstep->state = Decelerate;
        } else {
            /* 如果不使用减速，设置电机状态为恒速 */
            hstep->state = Constant;
        }
    }
}

/**
 * @brief 检查生成器数据缓冲区是否准备就绪
 * 
 * 该函数用于查询生成器（可能是某种数据处理或生成模块）的数据缓冲区是否已经准备就绪，
 * 可以被进一步处理或使用。这在需要等待数据生成或处理完成的场景中非常有用，比如在
 * 实时数据处理系统中。
 * 
 * @param hstep 指向stepTypedef结构体的指针，该结构体通常包含了生成器的状态信息。
 * @return int 返回生成器数据缓冲区的状态。一般情况下，如果缓冲区准备就绪，则返回1或true；
 *             如果缓冲区尚未准备就绪，则返回0或false。
 * @note 在RTOS（实时操作系统）环境下，为了确保数据生成和DMA传输的同步，建议使用二值信号量
 *       来配合使用这个函数。这可以帮助精确控制数据处理的时机，避免数据竞争或丢失。
 */
__inline int Step_IsBuffRdy(stepTypedef *hstep) {
    return hstep->buffRdy;
}


/**
 * @brief 获取步进电机的运行方向
 * 
 * 该函数通过访问步进电机句柄中的runDir成员，来获取步进电机当前的运行方向。
 * 这对于需要根据电机方向进行控制逻辑判断的应用场景非常有用。
 * 
 * @param hstep 步进电机的句柄，指向含有运行方向信息的结构体
 * @return int 返回步进电机的运行方向，通常为0代表正向，1代表反向
 */
int Step_GetDir(stepTypedef *hstep) {
    return hstep->runDir;
}

/**
 * 获取当前应使用的缓冲区
 * 
 * 此函数根据步进器对象内的缓冲区索引，决定返回哪个缓冲区。这是在双缓冲区策略中常见的做法，
 * 用于分离数据的处理和显示，减少因数据处理导致的界面刷新延迟。
 * 
 * @param hstep 步进器对象的指针，包含了缓冲区信息和当前使用的缓冲区索引。
 * @return 返回一个指向当前应使用的缓冲区的指针。如果索引为0，则返回buff0；如果索引为1，则返回buff1。
 */
uint16_t *Step_GetCurBuffer(stepTypedef *hstep) {
    // 根据缓冲区索引决定返回哪个缓冲区
    return hstep->buffIndex ? hstep->buff0 : hstep->buff1;
}

/**
 * @brief 获取当前步骤使用的缓冲区长度
 * 
 * 该函数用于返回指定步骤句柄所使用的缓冲区的长度。这可以帮助用户了解当前步骤处理的数据量，
 * 或者在需要时调整缓冲区的大小。
 * 
 * @param hstep 指向步骤句柄的指针。步骤句柄是一个结构体指针，包含了步骤相关的各种信息和状态。
 * @return uint32_t 返回当前步骤使用的缓冲区长度。长度以字节为单位。
 */
__inline uint32_t Step_BuffUsedLength(stepTypedef *hstep) {
    return hstep->buffToUse;
}

/**
 * @brief 尝试对步骤对象进行运行时锁定
 * 
 * 本函数用于在运行时锁定一个步骤对象，以防止多个线程或过程同时访问该对象。
 * 如果对象当前未被锁定（即处于解锁状态），则函数将锁定对象并返回成功。
 * 如果对象已被锁定，则函数直接返回失败，避免了死锁或竞争条件的发生。
 * 
 * @param hstep 指向步骤对象的指针
 * @return int 锁定成功返回0，锁定失败返回-1
 */
/**
 * @brief 句柄运行时锁定
 *
 * @param hstep step句柄
 * @return int 成功返回0
 *             失败返回-1
 */
int Step_Lock(stepTypedef *hstep) {
    /* 检查步骤对象的锁定状态，如果当前为解锁状态，则将其锁定 */
    if (hstep->lock == UNLOCK)
        hstep->lock = LOCK;
    else
        /* 如果对象已被锁定，则返回失败 */
        return -1;
    /* 如果成功锁定对象，则返回成功 */
    return 0;
}

/**
 * @brief 解锁步骤句柄
 * 
 * 本函数用于在运行时解锁一个步骤对象。只有当步骤处于锁定状态时，才能成功解锁。
 * 如果步骤当前并未被锁定，则函数将返回错误。
 * 
 * @param hstep 指向步骤对象的句柄
 * @return int 成功解锁返回0，解锁失败（步骤未锁定）返回-1
 */
/**
 * @brief 句柄运行时解锁
 *
 * @param hstep step句柄
 * @return int 成功返回0
 *             失败返回-1
 */
int Step_Unlock(stepTypedef *hstep) {
    /* 检查步骤是否处于锁定状态 */
    if (hstep->lock == LOCK)
        /* 如果是锁定状态，则将其改为未锁定状态 */
        hstep->lock = UNLOCK;
    else
        /* 如果步骤未被锁定，则返回错误 */
        return -1;
    /* 解锁成功，返回0 */
    return 0;
}

/**
 * @brief 中止步进电机的运行
 * 
 * 此函数用于立即停止步进电机的运行，它通过停止PWM信号的输出并释放步进电机的锁来实现。
 * 这是停止步进电机运动的关键函数，适用于需要快速停止电机的场景。
 * 
 * @param hstep 指向步进电机句柄的指针，包含了步进电机的相关配置和状态信息。
 */
void Step_Abort(stepTypedef *hstep) {
    /* 停止PWM信号的DMA传输，这是为了立即停止电机的旋转 */
    HAL_TIM_PWM_Stop_DMA(hstep->phtim, hstep->channel);
    
    /* 停止PWM信号的输出，进一步确保电机停止 */
    HAL_TIM_PWM_Stop(hstep->phtim, hstep->channel);
    
    /* 释放步进电机的锁，以便下次能够正常启动 */
    Step_Unlock(hstep);
}

/**
 * 预填充步进电机加速缓冲区
 * 
 * 本函数用于在开始运动前，根据设定的加速曲线和参数，计算并预填充步进电机的加速缓冲区。
 * 这样做可以提高运动的平滑度和响应速度。
 * 
 * @param hstep 步进电机步骤处理句柄，包含步进电机的相关参数和状态。
 * @param stepToGo 待执行的步数。
 * @param dir 运动方向，用于设定步进电机的旋转方向。
 * @param useDec 是否使用减速，用于控制运动结束时是否采用减速策略。
 * @return 返回预填充的缓冲区使用量，用于指示缓冲区的填充情况。
 */
int Step_Prefill(stepTypedef *hstep, uint32_t stepToGo, uint8_t dir, uint8_t useDec) 
{
    // 锁定步进电机处理句柄，确保并发安全
    Step_Lock(hstep);

    // 设置待执行的步数和是否使用减速
    hstep->stepToGo = stepToGo;
    hstep->useDec = useDec;

    // 根据加速曲线类型，计算加速步数
    switch (AcclerateCurve) 
	{
        case Curve_Trapezoidal:
            // 矩形波加速，计算加速步数
            hstep->accStep = (hstep->Fmin + hstep->Fmax) * hstep->Tacc / 2000;
            break;
        case Curve_S:
            // S型曲线加速，计算加速步数
            hstep->accStep = 0.5f * hstep->Fmax * hstep->Tacc * 0.001f + hstep->Fmin * hstep->Tacc * 0.001f;
            break;
    }

    // 初始化缓冲区使用量
    hstep->buffToUse = 0;

    // 设置运动方向，并通过GPIO输出方向信号
    hstep->runDir = dir;
    HAL_GPIO_WritePin(hstep->gpioPort, hstep->gpioPin, dir);

    // 根据加速时间和步数，决定是直接以最大频率运行，还是进行加速
    if (hstep->Tacc == 0) { // 加速时间为0，直接以最大频率运行
        hstep->Fcur = hstep->Fmax;
        hstep->state = Constant;
        hstep->buffToUse = Step_FillConstant(hstep);
    } else if (hstep->accStep > stepToGo) 
	{ // 加速步数大于待执行步数，直接以最小频率运行
        hstep->Fcur = hstep->Fmin;
        hstep->state = Constant;
        hstep->buffToUse = Step_FillConstant(hstep);
    } else 
	{ // 正常加速运行
        hstep->state = Acclerate;
        hstep->buffToUse = Step_FillAccelerate(hstep);
    }

    // 返回缓冲区使用量
    return hstep->buffToUse;
}

/**
 * @brief 根据当前状态填充步进电机的缓冲区
 * 
 * 本函数根据步进电机运行的状态，选择合适的缓冲区填充算法，
 * 以实现加速、恒速或减速运行。通过调用相应的填充函数，为
 * 步进电机的运动提供精确的脉冲信号。
 * 
 * @param hstep 步进电机状态句柄，包含当前状态和将要使用的缓冲区
 * @return int 返回填充的缓冲区大小，若状态无效则返回-1
 */
/**
 * @brief 填充缓冲区，循环调用
 *
 * @param hstep step句柄
 */
int Step_BuffFill(stepTypedef *hstep) {
    /* 初始化将要使用的缓冲区索引 */
    hstep->buffToUse = 0;

    /* 根据当前状态选择缓冲区填充算法 */
    switch (hstep->state) {
        case Acclerate:
            /* 加速运行时，调用加速缓冲区填充函数 */
            hstep->buffToUse = Step_FillAccelerate(hstep);
            break;
        case Constant:
            /* 恒速运行时，调用恒速缓冲区填充函数 */
            hstep->buffToUse = Step_FillConstant(hstep);
            break;
        case Decelerate:
            /* 减速运行时，调用减速缓冲区填充函数 */
            hstep->buffToUse = Step_FillDecelerate(hstep);
            break;
        default:
            /* 若状态无效，返回错误码-1 */
            return -1;
    }

    /* 返回填充的缓冲区大小 */
    return hstep->buffToUse;
}



































