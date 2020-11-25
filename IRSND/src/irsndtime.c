/**************************************************************************
 * @文件  : irsndtime.c
 * @作者  : caochao
 * @日期  : 22-9-2017
 * @摘要  : 这个C文件时实现了定时器的相关函数
 *
 * 修改人 :
 *  日期  :
 *
 * 修改人 :
 *  日期  :
 **************************************************************************/
#include "stm32f10x.h"
#include "irsndtime.h"
#include "irsnd.h"

extern uint32_t SystemCoreClock;
/***************************************************************************
 *   函数名：irsnd_timer_init 
 *   功  能：定时器初始化
 *   输  入：
 *          无
 *   输  出：
 *          无
 ***************************************************************************/
void irsnd_timer_init(void)
{   
    NVIC_InitTypeDef NVIC_InitStructure;

    TIM_ClearITPendingBit(IRSND_TIME_PORT, TIM_IT_Update); 
    TIM_ITConfig(IRSND_TIME_PORT, TIM_IT_Update, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = IRSND_TIME_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;  //抢占优先级最低
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;  //子优先级最低
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  //定时器中断使能
    
    NVIC_Init(&NVIC_InitStructure);
    
    TIM_Cmd(IRSND_TIME_PORT, ENABLE);
}

/*********************************文件结束***************************************/
