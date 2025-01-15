#include "StepHelper.h"


/* ����ѡ�ʼ */
/*********************  Config Option Begin  ********************/

/* ���δ����RCC_MAX_FREQUENCY������Ĭ��ֵ����Ϊ72MHz */
#ifndef RCC_MAX_FREQUENCY
#define RCC_MAX_FREQUENCY 72000000
#endif


/* �����������ͣ�����ѡ���������� */
#define AcclerateCurve (Curve_S)

/* ����ѡ����� */
/*********************  Config Option End  ********************/

/* ���δ����__weak�����䶨��Ϊ__attribute__((weak)) */
#ifndef __weak
#define __weak __attribute__((weak))
#endif // !__weak

/* ��������������ͺ꣬0x01��ʾ�������ߣ�0x02��ʾS���� */
#define Curve_Trapezoidal (0x01)
#define Curve_S (0x02)
//**************************************
void Step_Init(stepTypedef *hstep, TIM_HandleTypeDef *phtim, uint32_t channel, 
					GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin,
						float Fmin, float Fmax, float Tacc)
{
    // ��ʼ���ṹ���Ա
    hstep->phtim = phtim; // ��ʱ�����
    hstep->channel = channel; // ��ʱ��ͨ��

    hstep->gpioPort = GPIOx; // GPIO�˿�
    hstep->gpioPin = GPIO_Pin; // GPIO����

    // ����TIMͨ�����ö�Ӧ�ıȽϼĴ���ֵΪ��ʱ�����ڵ�һ��
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

    hstep->Fmin = Fmin; // ��СƵ��
    hstep->Fmax = Fmax; // ���Ƶ��
    hstep->Tacc = Tacc; // ����ʱ��

    hstep->t = 0.0f; // ��ǰʱ���ʼ��
    hstep->Fcur = 0.0f; // ��ǰƵ�ʳ�ʼ��

    // ������Ĭ�������ֶ���ʼ�����Ѷ���ʱ�Զ�����
    memset(hstep->buff0, 0, sizeof(hstep->buff0)); 
    memset(hstep->buff1, 0, sizeof(hstep->buff1)); 

    hstep->state = Stop; // ��ʼ��״̬Ϊֹͣ

    hstep->lock = UNLOCK; // ��ʼ��������Ϊ����״̬

    hstep->stepToGo = 0; // ʣ�ಽ����ʼ��Ϊ0
    hstep->buffToUse = 0; // ��ǰʹ�õĻ�����������ʼ��Ϊ0
    hstep->accStep = 0; // ���ٲ�����ʼ��Ϊ0

    // ��־λ��ʼ��
    hstep->useDec = 1; // Ĭ�����ü���
    hstep->buffIndex = 0; // ������������־λ��ʼ��
    hstep->buffRdy = 0; // ������׼����־λ��ʼ��Ϊδ׼��
}

//**************************************
/**
 * @brief ���ɼ��ٹ��̵�Ƶ������
 * 
 * ���������ݵ�ǰ���������״̬���趨�ļ������ߣ����㲢���ɼ��ٹ����е�Ƶ�����顣
 * Ƶ���������ڿ��Ʋ�������ڼ��ٽ׶ε��ٶȱ仯��ȷ��ƽ�����١�
 * 
 * @param hstep ����������ƾ��
 * @return int ����ʵ�����ɵ����鳤��
 */
int Step_FillAccelerate(stepTypedef *hstep) 
{
    // ��鵱ǰ״̬�Ƿ�Ϊ����״̬��������ǣ���ֱ�ӷ���0
    if (hstep->state != Acclerate)
        return 0;

    // ȷ��Ҫʹ�õĻ�������С��ָ��
    int buffToUse = hstep->stepToGo > BufferSize ? BufferSize : hstep->stepToGo;
    uint16_t *buffPtr = hstep->buffIndex ? hstep->buff1 : hstep->buff0;

    // ������ٹ����е�ÿ��Ƶ�ʵ�
    // Acclerate
    int i = 0;
    for (; hstep->t <= hstep->Tacc; i++) 
	{
        // ��ֹ����Խ��
        // overflow check
        if (i >= buffToUse)
            break;

        // ����ʱ��t
        // get t
        hstep->t += 1000.0f / hstep->Fcur;

        // ���ݲ�ͬ�ļ������߼��㵱ǰƵ��
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

        // ���㲢�洢PSCֵ��������
        // get PSC & fill
        buffPtr[i] = (uint16_t) ((RCC_MAX_FREQUENCY / (hstep->phtim->Instance->ARR + 1)) / hstep->Fcur - 1);
    }

    // �л�ʹ���ĸ�������
    hstep->buffIndex = !hstep->buffIndex;

    // ������ɵ����鳤�ȵ���Ԥ�����󳤶ȣ���ʾ���ٹ��������
    // Interrupted
    if (i >= buffToUse) {
        hstep->buffRdy = 1;
        return buffToUse;
    }

    // ����Ϊ�㶨Ƶ��״̬�������ʣ��Ļ������ռ�
    // Constant
    hstep->state = Constant;
    hstep->Fcur = hstep->Fmax;
    uint16_t PSC = (uint16_t) ((RCC_MAX_FREQUENCY / (hstep->phtim->Instance->ARR + 1)) / hstep->Fcur - 1);
    for (; i < buffToUse; i++) {
        buffPtr[i] = PSC;
    }

    // ��ǻ�����׼�����
    hstep->buffRdy = 1;
    return buffToUse;
}

/**
 * @brief ��������������
 *
 * @param hstep step���
 * @return int ���в���
 */
/**
 * ��������㶨�������ú���
 * 
 * �ú��������ڲ���������������ú㶨������ͨ������õ��ʵ���Ԥ��Ƶ��ֵ��PSC����
 * ������ֵд��ָ���Ļ���������ʵ�ֵ������ʱ�ĺ㶨�������ơ�
 * 
 * @param hstep ����������ƽṹ��ָ�룬����������ú�״̬��Ϣ��
 * @return ����ʵ����䵽�������Ĳ���������
 */
int Step_FillConstant(stepTypedef *hstep) {
    // ��鲽������Ƿ��ں㶨����״̬��������ǣ���ֱ�ӷ���
    if (hstep->state != Constant) {
        return 0;
    }

    // ���ݵ�ǰ����������ѡ����ȷ�Ļ�����ָ��
    uint16_t *buffPtr = hstep->buffIndex ? hstep->buff1 : hstep->buff0;
    
    // ����Ԥ��Ƶ����ֵ����ʵ�ֺ㶨��������
    uint16_t PSC = (uint16_t) ((RCC_MAX_FREQUENCY / (hstep->phtim->Instance->ARR + 1)) / hstep->Fcur - 1);
    
    // ȷ��ʵ��Ҫʹ�õĻ�������С����ֹ��������������
    int buffToUse = hstep->stepToGo > BufferSize ? BufferSize : hstep->stepToGo;

    // ������õ���Ԥ��Ƶ��ֵ��䵽������
    for (int i = 0; i < buffToUse; i++) {
        buffPtr[i] = PSC;
    }

    // ���۵��Ĵ���ԭ�������ض����������û��������һ��Ԫ�ص�ֵ�����ѱ�ע��

    // �л�������������Ϊ��һ�������׼��
    hstep->buffIndex = !hstep->buffIndex;

    // ���û�����׼��������־
    hstep->buffRdy = 1;
    
    // ����ʵ�����Ĳ�������
    return buffToUse;
}

/**
 * �����������ɺ���
 * 
 * �ú����������ɼ��ٹ����е�Ƶ�ʿ������飬��ʵ��ƽ������Ч����
 * ���ݵ�ǰ�ٶȺ�Ŀ��λ�ã�������Ҫ��ʱ���Ƶ�ʣ�����ȷ��PWM��ռ�ձȡ�
 * 
 * @param hstep ����������ƾ�������������������ز�����״̬��
 * @return int ����ʵ�����ɵ����鳤�ȡ�
 */
/**
 * @brief ��������������
 *
 * @param hstep step���
 * @return int ���в���
 */
int Step_FillDecelerate(stepTypedef *hstep) {
    // ��鵱ǰ״̬�Ƿ�Ϊ����״̬��������ǣ���ֱ�ӷ���
    if (hstep->state != Decelerate)
        return 0;

    // ȷ��ʵ����Ҫ���ɵ����鳤�ȣ���������������С
    int buffToUse = hstep->stepToGo > BufferSize ? BufferSize : hstep->stepToGo;
    // ���ݵ�ǰ������ָ���λ�ã�ѡ���Ӧ�Ļ�����
    uint16_t *buffPtr = hstep->buffIndex ? hstep->buff1 : hstep->buff0;

    // ���ٹ���ѭ��
    // Decelerate
    int i = 0;
    for (; hstep->Fcur > hstep->Fmin; i++) {
        // ����Ƿ񳬹���������С����ֹ���
        // overflow check
        if (i >= buffToUse)
            break;

        // ����ʱ��t����λΪ����
        // get t
        hstep->t -= 1000.0f / hstep->Fcur;

        // ���ݵ�ǰʱ��ͼ��ٶȼ��㵱ǰƵ��
        // get freq
        hstep->Fcur = (hstep->Fmax - hstep->Fmin) * 1.0f / (hstep->Tacc) * (hstep->t) + hstep->Fmin;

        // ����PWM��Ԥ��Ƶϵ������д�뻺����
        // get PSC & fill
        buffPtr[i] = (uint16_t) ((RCC_MAX_FREQUENCY / (hstep->phtim->Instance->ARR + 1)) / hstep->Fcur - 1);
    }

    // �л�������ָ��
    hstep->buffIndex = !hstep->buffIndex;

    // ������ɵ����鳤�ȵ��ڻ�������С����ʾ���ٹ��������
    // Interrupted
    if (i >= buffToUse) {
        hstep->buffRdy = 1;
        return buffToUse;
    }

    // ���õ�ǰƵ��Ϊ��СƵ��
    // Constant
    // hstep->state = Constant;
    hstep->Fcur = hstep->Fmin;
    // �����Ӧ��Ԥ��Ƶϵ��
    uint16_t PSC = (uint16_t) ((RCC_MAX_FREQUENCY / (hstep->phtim->Instance->ARR + 1)) / hstep->Fcur - 1);
    // ���ʣ��Ļ������ռ�Ϊ�㶨Ƶ�ʵ�Ԥ��Ƶϵ��
    for (; i < buffToUse; i++) {
        buffPtr[i] = PSC;
    }

    // ���û�����׼����ɱ�־
    hstep->buffRdy = 1;

    // ����ʵ�����ɵ����鳤��
    return buffToUse;
}

/**
 * @brief ��DMAʹ���경������Ļ���������ô˺���
 * @note �˺���Ϊ��������ּ�ڱ��û�ʵ�֡���RTOS�����£�����ʹ�ö�ֵ�ź�����ʵ��Fill������DMA�����ͬ����
 * 
 * @param hstep ָ�򲽽����ʵ���ľ��
 */
__weak void Step_BufferUsed(stepTypedef *hstep) {
    /* ���û�����׼��״̬ */
    hstep->buffRdy = 0;
    
    /* ����ʣ�ಽ������ȥ�ո�ʹ�õĲ��� */
    hstep->stepToGo -= hstep->buffToUse;

    /* ����ʣ�ಽ���������״̬ */
    if (hstep->stepToGo <= 0) 
	{
        /* ���ʣ�ಽ��С�ڵ���0�����õ��״̬Ϊֹͣ */
        hstep->state = Stop;
    } else if (hstep->stepToGo <= hstep->accStep) 
	{
        /* ���ʣ�ಽ��С�ڵ��ڼ��ٲ����������Ƿ�ʹ�ü����жϵ��״̬ */
        if (hstep->useDec) {
            /* ���ʹ�ü��٣����õ��״̬Ϊ���� */
            hstep->state = Decelerate;
        } else {
            /* �����ʹ�ü��٣����õ��״̬Ϊ���� */
            hstep->state = Constant;
        }
    }
}

/**
 * @brief ������������ݻ������Ƿ�׼������
 * 
 * �ú������ڲ�ѯ��������������ĳ�����ݴ��������ģ�飩�����ݻ������Ƿ��Ѿ�׼��������
 * ���Ա���һ�������ʹ�á�������Ҫ�ȴ��������ɻ�����ɵĳ����зǳ����ã�������
 * ʵʱ���ݴ���ϵͳ�С�
 * 
 * @param hstep ָ��stepTypedef�ṹ���ָ�룬�ýṹ��ͨ����������������״̬��Ϣ��
 * @return int �������������ݻ�������״̬��һ������£����������׼���������򷵻�1��true��
 *             �����������δ׼���������򷵻�0��false��
 * @note ��RTOS��ʵʱ����ϵͳ�������£�Ϊ��ȷ���������ɺ�DMA�����ͬ��������ʹ�ö�ֵ�ź���
 *       �����ʹ���������������԰�����ȷ�������ݴ����ʱ�����������ݾ�����ʧ��
 */
__inline int Step_IsBuffRdy(stepTypedef *hstep) {
    return hstep->buffRdy;
}


/**
 * @brief ��ȡ������������з���
 * 
 * �ú���ͨ�����ʲ����������е�runDir��Ա������ȡ���������ǰ�����з���
 * �������Ҫ���ݵ��������п����߼��жϵ�Ӧ�ó����ǳ����á�
 * 
 * @param hstep ��������ľ����ָ�������з�����Ϣ�Ľṹ��
 * @return int ���ز�����������з���ͨ��Ϊ0��������1������
 */
int Step_GetDir(stepTypedef *hstep) {
    return hstep->runDir;
}

/**
 * ��ȡ��ǰӦʹ�õĻ�����
 * 
 * �˺������ݲ����������ڵĻ��������������������ĸ���������������˫�����������г�����������
 * ���ڷ������ݵĴ������ʾ�����������ݴ����µĽ���ˢ���ӳ١�
 * 
 * @param hstep �����������ָ�룬�����˻�������Ϣ�͵�ǰʹ�õĻ�����������
 * @return ����һ��ָ��ǰӦʹ�õĻ�������ָ�롣�������Ϊ0���򷵻�buff0���������Ϊ1���򷵻�buff1��
 */
uint16_t *Step_GetCurBuffer(stepTypedef *hstep) {
    // ���ݻ������������������ĸ�������
    return hstep->buffIndex ? hstep->buff0 : hstep->buff1;
}

/**
 * @brief ��ȡ��ǰ����ʹ�õĻ���������
 * 
 * �ú������ڷ���ָ����������ʹ�õĻ������ĳ��ȡ�����԰����û��˽⵱ǰ���账�����������
 * ��������Ҫʱ�����������Ĵ�С��
 * 
 * @param hstep ָ��������ָ�롣��������һ���ṹ��ָ�룬�����˲�����صĸ�����Ϣ��״̬��
 * @return uint32_t ���ص�ǰ����ʹ�õĻ��������ȡ��������ֽ�Ϊ��λ��
 */
__inline uint32_t Step_BuffUsedLength(stepTypedef *hstep) {
    return hstep->buffToUse;
}

/**
 * @brief ���ԶԲ�������������ʱ����
 * 
 * ����������������ʱ����һ����������Է�ֹ����̻߳����ͬʱ���ʸö���
 * �������ǰδ�������������ڽ���״̬�����������������󲢷��سɹ���
 * ��������ѱ�����������ֱ�ӷ���ʧ�ܣ��������������������ķ�����
 * 
 * @param hstep ָ��������ָ��
 * @return int �����ɹ�����0������ʧ�ܷ���-1
 */
/**
 * @brief �������ʱ����
 *
 * @param hstep step���
 * @return int �ɹ�����0
 *             ʧ�ܷ���-1
 */
int Step_Lock(stepTypedef *hstep) {
    /* ��鲽����������״̬�������ǰΪ����״̬���������� */
    if (hstep->lock == UNLOCK)
        hstep->lock = LOCK;
    else
        /* ��������ѱ��������򷵻�ʧ�� */
        return -1;
    /* ����ɹ����������򷵻سɹ� */
    return 0;
}

/**
 * @brief ����������
 * 
 * ����������������ʱ����һ���������ֻ�е����账������״̬ʱ�����ܳɹ�������
 * ������赱ǰ��δ�����������������ش���
 * 
 * @param hstep ָ�������ľ��
 * @return int �ɹ���������0������ʧ�ܣ�����δ����������-1
 */
/**
 * @brief �������ʱ����
 *
 * @param hstep step���
 * @return int �ɹ�����0
 *             ʧ�ܷ���-1
 */
int Step_Unlock(stepTypedef *hstep) {
    /* ��鲽���Ƿ�������״̬ */
    if (hstep->lock == LOCK)
        /* ���������״̬�������Ϊδ����״̬ */
        hstep->lock = UNLOCK;
    else
        /* �������δ���������򷵻ش��� */
        return -1;
    /* �����ɹ�������0 */
    return 0;
}

/**
 * @brief ��ֹ�������������
 * 
 * �˺�����������ֹͣ������������У���ͨ��ֹͣPWM�źŵ�������ͷŲ������������ʵ�֡�
 * ����ֹͣ��������˶��Ĺؼ���������������Ҫ����ֹͣ����ĳ�����
 * 
 * @param hstep ָ�򲽽���������ָ�룬�����˲��������������ú�״̬��Ϣ��
 */
void Step_Abort(stepTypedef *hstep) {
    /* ֹͣPWM�źŵ�DMA���䣬����Ϊ������ֹͣ�������ת */
    HAL_TIM_PWM_Stop_DMA(hstep->phtim, hstep->channel);
    
    /* ֹͣPWM�źŵ��������һ��ȷ�����ֹͣ */
    HAL_TIM_PWM_Stop(hstep->phtim, hstep->channel);
    
    /* �ͷŲ�������������Ա��´��ܹ��������� */
    Step_Unlock(hstep);
}

/**
 * Ԥ��䲽��������ٻ�����
 * 
 * �����������ڿ�ʼ�˶�ǰ�������趨�ļ������ߺͲ��������㲢Ԥ��䲽������ļ��ٻ�������
 * ��������������˶���ƽ���Ⱥ���Ӧ�ٶȡ�
 * 
 * @param hstep ����������账���������������������ز�����״̬��
 * @param stepToGo ��ִ�еĲ�����
 * @param dir �˶����������趨�����������ת����
 * @param useDec �Ƿ�ʹ�ü��٣����ڿ����˶�����ʱ�Ƿ���ü��ٲ��ԡ�
 * @return ����Ԥ���Ļ�����ʹ����������ָʾ����������������
 */
int Step_Prefill(stepTypedef *hstep, uint32_t stepToGo, uint8_t dir, uint8_t useDec) 
{
    // �������������������ȷ��������ȫ
    Step_Lock(hstep);

    // ���ô�ִ�еĲ������Ƿ�ʹ�ü���
    hstep->stepToGo = stepToGo;
    hstep->useDec = useDec;

    // ���ݼ����������ͣ�������ٲ���
    switch (AcclerateCurve) 
	{
        case Curve_Trapezoidal:
            // ���β����٣�������ٲ���
            hstep->accStep = (hstep->Fmin + hstep->Fmax) * hstep->Tacc / 2000;
            break;
        case Curve_S:
            // S�����߼��٣�������ٲ���
            hstep->accStep = 0.5f * hstep->Fmax * hstep->Tacc * 0.001f + hstep->Fmin * hstep->Tacc * 0.001f;
            break;
    }

    // ��ʼ��������ʹ����
    hstep->buffToUse = 0;

    // �����˶����򣬲�ͨ��GPIO��������ź�
    hstep->runDir = dir;
    HAL_GPIO_WritePin(hstep->gpioPort, hstep->gpioPin, dir);

    // ���ݼ���ʱ��Ͳ�����������ֱ�������Ƶ�����У����ǽ��м���
    if (hstep->Tacc == 0) { // ����ʱ��Ϊ0��ֱ�������Ƶ������
        hstep->Fcur = hstep->Fmax;
        hstep->state = Constant;
        hstep->buffToUse = Step_FillConstant(hstep);
    } else if (hstep->accStep > stepToGo) 
	{ // ���ٲ������ڴ�ִ�в�����ֱ������СƵ������
        hstep->Fcur = hstep->Fmin;
        hstep->state = Constant;
        hstep->buffToUse = Step_FillConstant(hstep);
    } else 
	{ // ������������
        hstep->state = Acclerate;
        hstep->buffToUse = Step_FillAccelerate(hstep);
    }

    // ���ػ�����ʹ����
    return hstep->buffToUse;
}

/**
 * @brief ���ݵ�ǰ״̬��䲽������Ļ�����
 * 
 * ���������ݲ���������е�״̬��ѡ����ʵĻ���������㷨��
 * ��ʵ�ּ��١����ٻ�������С�ͨ��������Ӧ����亯����Ϊ
 * ����������˶��ṩ��ȷ�������źš�
 * 
 * @param hstep �������״̬�����������ǰ״̬�ͽ�Ҫʹ�õĻ�����
 * @return int �������Ļ�������С����״̬��Ч�򷵻�-1
 */
/**
 * @brief ��仺������ѭ������
 *
 * @param hstep step���
 */
int Step_BuffFill(stepTypedef *hstep) {
    /* ��ʼ����Ҫʹ�õĻ��������� */
    hstep->buffToUse = 0;

    /* ���ݵ�ǰ״̬ѡ�񻺳�������㷨 */
    switch (hstep->state) {
        case Acclerate:
            /* ��������ʱ�����ü��ٻ�������亯�� */
            hstep->buffToUse = Step_FillAccelerate(hstep);
            break;
        case Constant:
            /* ��������ʱ�����ú��ٻ�������亯�� */
            hstep->buffToUse = Step_FillConstant(hstep);
            break;
        case Decelerate:
            /* ��������ʱ�����ü��ٻ�������亯�� */
            hstep->buffToUse = Step_FillDecelerate(hstep);
            break;
        default:
            /* ��״̬��Ч�����ش�����-1 */
            return -1;
    }

    /* �������Ļ�������С */
    return hstep->buffToUse;
}



































