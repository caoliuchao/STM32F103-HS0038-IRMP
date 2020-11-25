/**************************************************************************
 * @�ļ�  : irmptime.c
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
#include "irmptime.h"
#include "usart.h"
#include "irmp.h"

extern uint32_t SystemCoreClock;
/***************************************************************************
 *   ��������irmp_timer_init 
 *   ��  �ܣ���ʱ����ʼ��
 *   ��  �룺
 *          ��
 *   ��  ����
 *          ��
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

    TIM_SetCounter(IRMP_TIME_PORT,0);	   	//��ն�ʱ��ֵ
    TIM_ClearITPendingBit(IRMP_TIME_PORT, TIM_IT_Update); 
    TIM_ITConfig(IRMP_TIME_PORT, TIM_IT_Update, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = IRMP_TIME_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;  //��ռ���ȼ����
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;  //�����ȼ����
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  //��ʱ���ж�ʹ��
    
    NVIC_Init(&NVIC_InitStructure);
    
    TIM_Cmd(IRMP_TIME_PORT, ENABLE);
}

#if IRMP_USE_IDLE_CALL == 1
/* ��غ궨�� */
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
 *   ��������irmp_idle(void)
 *   ��  �ܣ��û������ṩirmp_idle()������ʵ�ֲ�������
 *   ��  �룺
 *          ��
 *   ��  ����
 *          ��
 ***************************************************************************/
void irmp_idle(void)   
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    EXTI_InitTypeDef  EXTI_InitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;
    
    TIM_Cmd(IRMP_TIME_PORT,DISABLE);  //�رն�ʱ��
    
    TIM_ITConfig(IRMP_TIME_PORT, TIM_IT_Update, DISABLE);
    TIM_SetCounter(IRMP_TIME_PORT,0);  //��ն�ʱ��ֵ
    
    GPIO_InitStructure.GPIO_Pin = IRMP_BIT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;

    GPIO_Init(IRMP_PORT, &GPIO_InitStructure);
    
    /* ��GPIO���ж�������һ�� */
    GPIO_EXTILineConfig(IRMP_EXTI_PORT,IRMP_EXTI_PIN);
    
    /* �ⲿ�����ж����� */
    EXTI_InitStructure.EXTI_Line = IRMP_EXTI_LINE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    
    EXTI_Init(&EXTI_InitStructure);
    
    /* �����ж����ȼ����� */
    NVIC_InitStructure.NVIC_IRQChannel = IRMP_IT_CHANNEL;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    
    NVIC_Init(&NVIC_InitStructure);
    
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);  //ʹ��ʱ��
//    PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFI);  //����͹���ģʽ
}

/***************************************************************************
 *   ��������RCC_SysClockConfig(void)
 *   ��  �ܣ�����STM32��ϵͳʱ��
 *   ��  �룺
 *          ��
 *   ��  ����
 *          ��
 ***************************************************************************/
void RCC_SysClockConfig(void)
{
	ErrorStatus HSEStartUpStatus;
	
	RCC_DeInit();                           //RCC�Ĵ�����ʼ��                  
	RCC_HSEConfig(RCC_HSE_ON);             //���ⲿ���پ���     
	HSEStartUpStatus = RCC_WaitForHSEStartUp();   //�ȴ��ⲿ���پ�������  
    
	if (HSEStartUpStatus == SUCCESS)        //�ж��ⲿ���پ����Ƿ�����ɹ�
	{
		RCC_HCLKConfig( RCC_SYSCLK_Div1);      //����AHBʱ�ӣ�HCLK��   
		RCC_PCLK1Config(RCC_HCLK_Div2);        //���õ���AHBʱ�ӣ�PCLK1��   
		RCC_PCLK2Config(RCC_HCLK_Div1);        //���ø���AHBʱ�ӣ�PCLK2��    
		
		FLASH_SetLatency(FLASH_Latency_2);                         
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);      
		
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9);   //����PLLʱ��Դ����Ƶϵ��
		RCC_PLLCmd(ENABLE);                                   //ʹ��PLLʱ��
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		{
			;                       //���ָ����RCC��־λ�������,PLL����
		} 
		
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);    //����ϵͳʱ�ӣ�SYSCLK��
		while (RCC_GetSYSCLKSource() != 0x08)
		{
			;                       //��������ϵͳʱ�ӵ�ʱ��Դ,0x08��PLL��Ϊϵͳʱ�� 
		}			
	}
}

/***************************************************************************
 *   ��������irmp_idle(void)
 *   ��  �ܣ��û������ṩirmp_idle()������ʵ�ֲ�������
 *   ��  �룺
 *          ��
 *   ��  ����
 *          ��
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
        USART1_Configuration();         //��ʼ��usart1
        
        RCC_APB2PeriphClockCmd(IRMP_PORT_RCC, ENABLE); //��GPIO��ʱ��
    
        GPIO_InitStructure.GPIO_Pin = IRMP_BIT;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;

        GPIO_Init(IRMP_PORT, &GPIO_InitStructure);
       
    
        /* �ⲿ�����ж����� */
        EXTI_InitStructure.EXTI_Line = IRMP_EXTI_LINE;
        EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
        EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
        EXTI_InitStructure.EXTI_LineCmd = DISABLE;
        
        EXTI_Init(&EXTI_InitStructure);
        
        /* �����ж����ȼ����� */
        NVIC_InitStructure.NVIC_IRQChannel = IRMP_IT_CHANNEL;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    
        NVIC_Init(&NVIC_InitStructure);
        
        USART1_Configuration();         //��ʼ��usart1
        
        TIM_SetCounter(IRMP_TIME_PORT,0);  //��ն�ʱ��ֵ
        TIM_ClearITPendingBit(IRMP_TIME_PORT, TIM_IT_Update);  //�����־λ
        TIM_Cmd(IRMP_TIME_PORT, ENABLE);
        TIM_ITConfig(IRMP_TIME_PORT, TIM_IT_Update, ENABLE);
    } 

#if SYSTEM_SUPPORT_OS	 
	OSIntExit();  											 
#endif	    
}
#endif
/*********************************�ļ�����***************************************/
