#ifndef __STEPHELP_H__
#define __STEPHELP_H__

#include "common_inc.h"

#define BufferSize (200)

#define M_PI (3.1415926535f)

#define Step_Free stepX.lock == UNLOCK && stepY.lock == UNLOCK//���������ܶ�

//���岽��������ƽṹ��
typedef struct stepTypedef {
    //��ʱ����������ڲ�������
    TIM_HandleTypeDef *phtim;
    //��ʱ��ͨ��������ѡ��ʱ�����ĸ�ͨ����������
    uint32_t channel;

    //GPIO�˿ڣ������������
    GPIO_TypeDef *gpioPort;
    //GPIO���ţ�����ѡ������GPIO�����������
    uint16_t gpioPin;

    //��СƵ�ʣ���λΪHz
    float Fmin; 
    //���Ƶ�ʣ���λΪHz
    float Fmax; 
    //����ʱ�䣬��λΪms
    float Tacc; 

    //��ǰʱ�䣬��λΪms���ڲ�ʹ��
    float t; 
    //��ǰƵ�ʣ���λΪHz���ڲ�ʹ��
    float Fcur; 

    //������0�����ڴ洢������
    uint16_t buff0[BufferSize]; 
    //������1�����ڴ洢������
    uint16_t buff1[BufferSize]; 

    //״̬ö�٣��������١����١����ٺ�ֹͣ
    enum fillState {
        Acclerate,
        Constant,
        Decelerate,
        Stop
    } state; 

    //������ö�٣����ڿ�������״̬
    enum runningLock {
        UNLOCK,
        LOCK
    } lock;

    //ʣ�ಽ�������ڿ��Ʋ���������ƶ�����
    uint32_t stepToGo;
    //��ǰʹ�õĻ���������
    uint32_t buffToUse;
    //���ٲ��������ڼ�����ٹ����еĲ���
    uint32_t accStep;

    //���з����־λ
    uint8_t runDir: 1;
    //�Ƿ�ʹ�ü��ٱ�־λ
    uint8_t useDec: 1;
    //�Ƿ���Ϊ������б�־λ
    uint8_t useAsMotor: 1;
    //��ǰ������������־λ
    uint8_t buffIndex: 1; 
    //�������Ƿ�׼���õı�־λ
    uint8_t buffRdy: 1; 
} stepTypedef;

extern stepTypedef stepX, stepY;

/**
 * ��ʼ�������������
 * @param hstep ����������ƽṹ��ָ��
 * @param phtim TIM������ָ��
 * @param channel TIMͨ����
 * @param GPIOx GPIO������ָ��
 * @param GPIO_Pin GPIO���ź�
 * @param Fmin ��СƵ��
 * @param Fmax ���Ƶ��
 * @param Tacc ����ʱ��
 */
void Step_Init(stepTypedef *hstep, TIM_HandleTypeDef *phtim, 
                uint32_t channel, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin,
               float Fmin, float Fmax, float Tacc);

/**
 * ��ǲ����������������ʹ��
 * @param hstep ����������ƽṹ��ָ��
 */
void Step_BufferUsed(stepTypedef *hstep);

/**
 * ��鲽������������Ƿ�׼������
 * @param hstep ����������ƽṹ��ָ��
 * @return ������׼��״̬
 */
int Step_IsBuffRdy(stepTypedef *hstep);

/**
 * ��ȡ��������ĵ�ǰ����
 * @param hstep ����������ƽṹ��ָ��
 * @return �����������
 */
int Step_GetDir(stepTypedef *hstep);

/**
 * ��ȡ���������ǰʹ�õĻ�����ָ��
 * @param hstep ����������ƽṹ��ָ��
 * @return ������ָ��
 */
uint16_t *Step_GetCurBuffer(stepTypedef *hstep);

/**
 * ��ȡ���������������ʹ�ó���
 * @param hstep ����������ƽṹ��ָ��
 * @return ��������ʹ�ó���
 */
uint32_t Step_BuffUsedLength(stepTypedef *hstep);

/**
 * ������������������ƽṹ��
 * @param hstep ����������ƽṹ��ָ��
 * @return ����״̬
 */
int Step_Lock(stepTypedef *hstep);

/**
 * ���Խ�������������ƽṹ��
 * @param hstep ����������ƽṹ��ָ��
 * @return ����״̬
 */
int Step_Unlock(stepTypedef *hstep);

/**
 * ��ֹ���������ǰ����
 * @param hstep ����������ƽṹ��ָ��
 */
void Step_Abort(stepTypedef *hstep);

/**
 * Ԥ��䲽�����������
 * @param hstep ����������ƽṹ��ָ��
 * @param stepToGo ��Ҫ�ƶ��Ĳ���
 * @param dir �ƶ�����
 * @param useDec �Ƿ�ʹ�ü���
 * @return Ԥ���״̬
 */
int Step_Prefill(stepTypedef *hstep, uint32_t stepToGo, uint8_t dir, uint8_t useDec);

/**
 * ��䲽�����������
 * @param hstep ����������ƽṹ��ָ��
 * @return ���������״̬
 */
int Step_BuffFill(stepTypedef *hstep);

/**
 * ��䲽�����������
 * @param hstep ����������ƽṹ��ָ��
 * @return ���������״̬
 */
int Step_FillDecelerate(stepTypedef *hstep);

/**
 * ��䲽�����������
 * @param hstep ����������ƽṹ��ָ��
 * @return ���������״̬
 */
int Step_FillConstant(stepTypedef *hstep);

/**
 * ��䲽�����������
 * @param hstep ����������ƽṹ��ָ��
 * @return ���������״̬
 */
int Step_FillAccelerate(stepTypedef *hstep);

//
void Start_PWM(stepTypedef *hstep, uint32_t stepToGo, uint8_t dir, uint8_t useDec);
void Pulse_2_PWM(void);

#endif


