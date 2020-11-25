/********************************************************************
* @�ļ�  : task.c
* @����  : caochao
* @����  : 14-2-2017
* @ժҪ  : ���c�ļ�������������
* �޸��� :
*  ����  :
*
* �޸��� :
*  ����  :
*********************************************************************/
#include "stm32f10x.h"
#include "includes.h"

/*-----------------��ʼ�����ջ-------------------------------------*/
__align(8) OS_STK START_TASK_STK[START_STK_SIZE];   //���������ջ�ռ�

/*-----------------���ڴ�ӡ��ջ-------------------------------------*/
__align(8) OS_STK USART1_TASK_STK[USART1_STK_SIZE]; //���������ջ�ռ�	

/*-----------------IRMP�����ջ-------------------------------------*/
__align(8) OS_STK IRMP_TASK_STK[IRMP_STK_SIZE];     //���������ջ�ռ�

/*******************************************************************
*   ��������start_task(void *pdata)
*   ��  �ܣ���ʼ���񣬽�����������
*   ��  �룺
*          pdata ָ��
*   ��  ����
*          ��
*******************************************************************/
void start_task(void *pdata)
{
    uint8_t status;
    OS_CPU_SR cpu_sr = 0;
	
    USART1_Configuration();         //��ʼ��usart1
		
	Systick_Init();                 //����UCOSIIϵͳ����   
    
	OSStatInit();					//��ʼ��ͳ������.�������ʱ1��������	
 	OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)   
	
    OSTaskCreate(usart1_task,(void *)0,(OS_STK*)&USART1_TASK_STK[USART1_STK_SIZE-1],USART1_TASK_PRIO);  
    OSTaskCreate(irmp_task,(void *)0,(OS_STK*)&IRMP_TASK_STK[IRMP_STK_SIZE-1],IRMP_TASK_PRIO);

    status = OSTaskDelReq(OS_PRIO_SELF);	//����ɾ����ʼ����.
    if (status == OS_ERR_NONE)
    {
		status = OSTaskDel(OS_PRIO_SELF);
        if (status == OS_ERR_NONE)
        {
            printf("ɾ����ʼ����ɹ� \r\n");
        }
        else
        {
            printf("��ʼ����ɾ��ʧ�� \r\n");
        }
    }
    else
    {
        printf("����ʼ����ɾ��ʧ�� \r\n");
    }
    
    OS_EXIT_CRITICAL();	   //�˳��ٽ���(���Ա��жϴ��)    	
}


/*********************************************************************
 *   ��������irmp_task(void *pdata)
 *   ��  �ܣ�IRMP������
 *   ��  �룺
 *          pdata ָ��
 *   ��  ����
 *          ��
 *********************************************************************/
void irmp_task(void *pdata)
{
    uint32_t  count = 0; 
    IRMP_DATA irmp_data;

    irmp_init();    // initialize irmp
    irmp_timer_init(); // initialize timer

    while (1)
    {  
        if (irmp_get_data (&irmp_data))
        {
            printf("���յ��ĵ�ַΪ��%d \r\n",irmp_data.address);
            printf("���յ�������Ϊ��%d \r\n",irmp_data.command);
            if (irmp_data.flags && IRMP_FLAG_REPETITION)
            {
                count++;
                printf("���յ������ݴ�����%d \r\n",count);
            }
            else
            {
                count = 1;
                printf("���յ������ݴ�����%d \r\n",count);
            }
        }
        
        Delay1ms(110);
    }
}

/*********************************************************************
*   ��������usart1_task(void *pdata)
*   ��  �ܣ�usart1���ڴ�ӡ����
*   ��  �룺
*          pdata ָ��
*   ��  ����
*          ��
**********************************************************************/
void usart1_task(void *pdata)
{  
    while (1)
    {  	
        printf("CPU��ʹ����Ϊ��%d%%  \r\n",OSCPUUsage);  //��ӡ��CPU��ʹ����
		Delay1ms(500);	
    }
}
				
/*****************************�ļ�����********************************/
                


