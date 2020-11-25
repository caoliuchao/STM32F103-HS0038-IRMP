/**************************************************************************
 * @文件  : irmptime.c
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
#include "irmptime.h"
#include "usart.h"
#include "irmp.h"

extern uint32_t SystemCoreClock;
/***************************************************************************
 *   函数名：irmp_timer_init 
 *   功  能：定时器初始化
 *   输  入：
 *          无
 *   输  出：
 *          无
 ***************************************************************************/
void irmp_timer_init (void)
{   
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB1PeriphClockCmd(IRMP_TIME_PERIPH, ENABLE);

    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = 399;
    TIM_TimeBaseStructure.TIM_Prescaler = ((SystemCoreClock/F_INTERRUPTS)/400) - 1;
    TIM_TimeBaseInit(IRMP_TIME_PORT, &TIM_TimeBaseStructure);

    TIM_SetCounter(IRMP_TIME_PORT,0);	   	//清空定时器值
    TIM_ClearITPendingBit(IRMP_TIME_PORT, TIM_IT_Update); 
    TIM_ITConfig(IRMP_TIME_PORT, TIM_IT_Update, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = IRMP_TIME_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;  //抢占优先级最低
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;  //子优先级最低
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  //定时器中断使能
    
    NVIC_Init(&NVIC_InitStructure);
    
    TIM_Cmd(IRMP_TIME_PORT, ENABLE);
}

#if IRMP_USE_IDLE_CALL == 1
/* 相关宏定义 */
#define  IRMP_EXTI_PORT    CONCAT(GPIO_PortSourceGPIO,IRMP_PORT_LETTER)
#define  IRMP_EXTI_PIN     CONCAT(GPIO_PinSource,IRMP_BIT_NUMBER)
#define  IRMP_EXTI_LINE    CONCAT(EXTI_Line,IRMP_BIT_NUMBER)

#if   ((IRMP_BIT_NUMBER >= 0) && (IRMP_BIT_NUMBER <= 4))
#define  IRMP_IT_CHANNEL   CONCAT(CONCAT(EXTI,IRMP_BIT_NUMBER),_IRQn)
#define  IRMP_IT_Handler   CONCAT(CONCAT(EXTI,IRMP_BIT_NUMBER),_IRQHandler)
#elif ((IRMP_BIT_NUMBER >= 5) && (IRMP_BIT_NUMBER <= 9))
#define  IRMP_IT_CHANNEL   EXTI9_5_IRQn
#define  IRMP_IT_Handler   EXTI9_5_IRQHandler
#elif ((IRMP_BIT_NUMBER >= 10) && (IRMP_BIT_NUMBER <= 15))
#define  IRMP_IT_CHANNEL   EXTI15_10_IRQn
#define  IRMP_IT_Handler   EXTI15_10_IRQHandler
#endif

/***************************************************************************
 *   函数名：irmp_idle(void)
 *   功  能：用户必须提供irmp_idle()函数的实现并链接它
 *   输  入：
 *          无
 *   输  出：
 *          无
 ***************************************************************************/
void irmp_idle(void)   
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    EXTI_InitTypeDef  EXTI_InitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;
    
    TIM_Cmd(IRMP_TIME_PORT,DISABLE);  //关闭定时器
    
    TIM_ITConfig(IRMP_TIME_PORT, TIM_IT_Update, DISABLE);
    TIM_SetCounter(IRMP_TIME_PORT,0);  //清空定时器值
    
    GPIO_InitStructure.GPIO_Pin = IRMP_BIT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;

    GPIO_Init(IRMP_PORT, &GPIO_InitStructure);
    
    /* 把GPIO和中断线连在一起 */
    GPIO_EXTILineConfig(IRMP_EXTI_PORT,IRMP_EXTI_PIN);
    
    /* 外部按键中断配置 */
    EXTI_InitStructure.EXTI_Line = IRMP_EXTI_LINE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    
    EXTI_Init(&EXTI_InitStructure);
    
    /* 按键中断优先级配置 */
    NVIC_InitStructure.NVIC_IRQChannel = IRMP_IT_CHANNEL;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    
    NVIC_Init(&NVIC_InitStructure);
    
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);  //使能时钟
//    PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFI);  //进入低功耗模式
}

/***************************************************************************
 *   函数名：RCC_SysClockConfig(void)
 *   功  能：配置STM32的系统时钟
 *   输  入：
 *          无
 *   输  出：
 *          无
 ***************************************************************************/
void RCC_SysClockConfig(void)
{
	ErrorStatus HSEStartUpStatus;
	
	RCC_DeInit();                           //RCC寄存器初始化                  
	RCC_HSEConfig(RCC_HSE_ON);             //打开外部高速晶振     
	HSEStartUpStatus = RCC_WaitForHSEStartUp();   //等待外部高速晶振起振  
    
	if (HSEStartUpStatus == SUCCESS)        //判断外部高速晶振是否起振成功
	{
		RCC_HCLKConfig( RCC_SYSCLK_Div1);      //设置AHB时钟（HCLK）   
		RCC_PCLK1Config(RCC_HCLK_Div2);        //设置低速AHB时钟（PCLK1）   
		RCC_PCLK2Config(RCC_HCLK_Div1);        //设置高速AHB时钟（PCLK2）    
		
		FLASH_SetLatency(FLASH_Latency_2);                         
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);      
		
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9);   //设置PLL时钟源及倍频系数
		RCC_PLLCmd(ENABLE);                                   //使能PLL时钟
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		{
			;                       //检查指定的RCC标志位设置与否,PLL就绪
		} 
		
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);    //设置系统时钟（SYSCLK）
		while (RCC_GetSYSCLKSource() != 0x08)
		{
			;                       //返回用作系统时钟的时钟源,0x08：PLL作为系统时钟 
		}			
	}
}

/***************************************************************************
 *   函数名：irmp_idle(void)
 *   功  能：用户必须提供irmp_idle()函数的实现并链接它
 *   输  入：
 *          无
 *   输  出：
 *          无
 ***************************************************************************/
void IRMP_IT_Handler(void)
{
    ErrorStatus       HSEStartUpStatus;
    EXTI_InitTypeDef  EXTI_InitStructure;
    GPIO_InitTypeDef  GPIO_InitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;
    
#if SYSTEM_SUPPORT_OS	
    OS_CPU_SR cpu_sr;
 	
    OS_ENTER_CRITICAL();
	OSIntEnter(); 
    OS_EXIT_CRITICAL();	      
#endif
    
    if (EXTI_GetITStatus(IRMP_EXTI_LINE) == SET)
    {
        EXTI_ClearITPendingBit(EXTI_Line9);
        
        /* Check if the Wake-Up flag is set */
        if(PWR_GetFlagStatus(PWR_FLAG_WU) != RESET)
        {
            /* Clear Wake Up flag */
            PWR_ClearFlag(PWR_FLAG_WU);
        }
        
        RCC_SysClockConfig();
        USART1_Configuration();         //初始化usart1
        
        RCC_APB2PeriphClockCmd(IRMP_PORT_RCC, ENABLE); //打开GPIO的时钟
    
        GPIO_InitStructure.GPIO_Pin = IRMP_BIT;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;

        GPIO_Init(IRMP_PORT, &GPIO_InitStructure);
       
    
        /* 外部按键中断配置 */
        EXTI_InitStructure.EXTI_Line = IRMP_EXTI_LINE;
        EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
        EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
        EXTI_InitStructure.EXTI_LineCmd = DISABLE;
        
        EXTI_Init(&EXTI_InitStructure);
        
        /* 按键中断优先级配置 */
        NVIC_InitStructure.NVIC_IRQChannel = IRMP_IT_CHANNEL;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    
        NVIC_Init(&NVIC_InitStructure);
        
        USART1_Configuration();         //初始化usart1
        
        TIM_SetCounter(IRMP_TIME_PORT,0);  //清空定时器值
        TIM_ClearITPendingBit(IRMP_TIME_PORT, TIM_IT_Update);  //清除标志位
        TIM_Cmd(IRMP_TIME_PORT, ENABLE);
        TIM_ITConfig(IRMP_TIME_PORT, TIM_IT_Update, ENABLE);
    } 

#if SYSTEM_SUPPORT_OS	 
	OSIntExit();  											 
#endif	    
}
#endif
/*********************************文件结束***************************************/
