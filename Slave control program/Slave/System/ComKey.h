#ifndef _COMKEY_H
#define _COMKEY_H

#include "common_inc.h"

#define COMKEY_ClickThreshold           20  //����ʱ����ֵ�����ڴ�ʱ����Ϊ����
#define COMKEY_HoldThreshold            800 //����ʱ����ֵ�����ڴ�ʱ����Ϊ����
#define COMKEY_IntervalVal              200 //���ʱ����ֵ�����ڴ�ʱ����Ϊ����
#define COMKEY_HoldTriggerThreshold     100 //����������ֵ������ʱÿ�����¼�����һ�ΰ���

typedef struct comKeyTypedef 
{
    u8 preVal: 1;
    u8 val: 1;

    u32 preTimer;// ���¼�ʱ
    u32 intervalTimer;// �ſ���ʱ
    u16 triggerTimer;// ����������ʱ

    struct comKeyTypedef *next;

    enum 
	{
        Release = 0,
        PrePress,
        Prelong,
        LongHold,
        MultiClick
    } state;// ״̬ö��

    uint32_t holdTime;// ������ʱ
    uint8_t clickCnt;// ���¼���
} comkey_t, *pcomkey_t;


// KEY
extern comkey_t Key_0,Key_1,Key_2,Key_3,Key_4;;

//Fuction
void ComKey_GetValue(comkey_t *key);

void ComKey_Init(comkey_t *key, int pollingPeriod);

void ComKey_Porc(void);


//CallBack_Fuction
void ComKey_LongHoldCallback(comkey_t *key);

void ComKey_HoldTriggerCallback(comkey_t *key);

void ComKey_FirstLongTriggerCallback(comkey_t *key);

void ComKey_MultipleClickCallback(comkey_t *key);

void ComKey_KeyReleaseCallback(comkey_t *key);

void ComKey_KeyPressCallback(comkey_t *key);










#endif 
