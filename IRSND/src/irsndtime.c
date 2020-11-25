/**************************************************************************
 * @�ļ�  : irsndtime.c
 * @����  : caochao
 * @����  : 22-9-2017
 * @ժҪ  : ���C�ļ�ʱʵ���˶�ʱ������غ���
 *
 * �޸��� :
 *  ����  :
 *
 * �޸��� :
 *  ����  :
 **************************************************************************/
#include "stm32f10x.h"
#include "irsndtime.h"
#include "irsnd.h"

extern uint32_t SystemCoreClock;
/***************************************************************************
 *   ��������irsnd_timer_init 
 *   ��  �ܣ���ʱ����ʼ��
 *   ��  �룺
 *          ��
 *   ��  ����
 *          ��
 ***************************************************************************/
void irsnd_timer_init(void)
{   
    NVIC_InitTypeDef NVIC_InitStructure;

    TIM_ClearITPendingBit(IRSND_TIME_PORT, TIM_IT_Update); 
    TIM_ITConfig(IRSND_TIME_PORT, TIM_IT_Update, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = IRSND_TIME_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;  //��ռ���ȼ����
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;  //�����ȼ����
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  //��ʱ���ж�ʹ��
    
    NVIC_Init(&NVIC_InitStructure);
    
    TIM_Cmd(IRSND_TIME_PORT, ENABLE);
}

/*********************************�ļ�����***************************************/
