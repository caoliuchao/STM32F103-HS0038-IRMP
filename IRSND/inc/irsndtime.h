/******************************************************************************
 * @文件  : irsndtime.h
 * @作者  : caochao
 * @日期  : 22-9-2017
 * @摘要  : 这个h文件时对定时器相关宏定义的设置
 *
 * 修改人 :
 *  日期  :
 *
 * 修改人 :
 *  日期  :
 *****************************************************************************/
#ifndef _IRSNDTIME_H
#define _IRSNDTIME_H

/*---------------------------定时器相关宏定义---------------------------------*/
#define  IRSND_TIME_PORT     TIM4
#define  IRSND_TIME_IRQn     TIM4_IRQn
#define  IRSND_TIME_PERIPH   RCC_APB1Periph_TIM4

/*---------------------------定时器函数--------------------------------------*/
void irsnd_timer_init(void);

#endif/*irsndtime.h*/
/*********************************文件结束**************************************/
