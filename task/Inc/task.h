/*********************************************************************
 * @文件  : task.h
 * @作者  : caochao
 * @日期  : 22-9-2017
 * @摘要  : 这个h文件时对UCOSII的任务堆栈大小和优先级的宏定义
 *
 * 修改人 :
 *  日期  :
 *
 * 修改人 :
 *  日期  :
 *********************************************************************/
/* 防止重复编译 */
#ifndef TASK_H
#define TASK_H

#ifdef __cplusplus
 extern "C" {
#endif
 
/*-------------------------START 任务--------------------------------*/
#define START_TASK_PRIO      			30 ////设置任务优先级 开始任务的优先级设置为最低
#define START_STK_SIZE  				128 //设置任务堆栈大小
    
/*-------------------------IRMP任务---------------------------------*/
#define IRMP_TASK_PRIO                  15//设置任务优先级
#define IRMP_STK_SIZE                   256 //设置任务堆栈大
 
/*-------------------------IRSND任务--------------------------------*/
#define IRSND_TASK_PRIO                 16//设置任务优先级
#define IRSND_STK_SIZE                  256 //设置任务堆栈大

/*-----------------------USART1-------------------------------------*/
#define USART1_TASK_PRIO                17 //设置任务优先级
#define USART1_STK_SIZE                 128 //设置任务堆栈大小  

/*-----------------开始任务堆栈--------------------------------------*/
extern OS_STK START_TASK_STK[START_STK_SIZE];   //创建任务堆栈空间

void start_task(void *pdata);  //任务函数接口
void usart1_task(void *pdata); //任务函数接口
void irmp_task(void *pdata);   //任务函数接口
void irsnd_task(void *pdata);   //任务函数接口

#ifdef __cplusplus
 }
#endif

#endif/*task.h*/
/*******************************文件结束*****************************/
 
 
 

