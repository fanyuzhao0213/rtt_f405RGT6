/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "my_key.h"
#include "rsa.h"

#define OS_THREAD_STRUCT OsThreadInitStruct *		/*定义线程结构体重定义*/
#define OS_THREAD_FUNC_ID_END  0X06					/*最大线程个数，根据实际项目修改*/


typedef void(*pOsThreadFuncInit)(rt_uint8_t,rt_uint32_t);	/*定义声明一个函数指针*/

typedef struct
{
	const rt_uint8_t FuncID;				/*ID*/
	const pOsThreadFuncInit ThreadInit;		/*函数*/
	const rt_uint16_t Priority;				/*优先级*/
	const rt_uint32_t Tick;					/*tick数*/
}OsThreadInitStruct;

static OsThreadInitStruct OsThreadInitTable[]=
{
	//Task ID    					TaskInitPointer     	Thread Priority  	Thread Tick  
	{0,								MyTestThreadInit,		5,					10},
	{0,								MyOledThreadInit,		5,					10},
	{0,								MyConsumerThreadInit,	5,					20},
	{0,								MyProducerThreadInit,	5,					20},
	{OS_THREAD_FUNC_ID_END,			RT_NULL,				1,					5},
};


/*线程初始化函数*/
static void OsThreadInit(void)
{
	/*OS_THREAD_STRUCT  #define定义了线程结构体*/
	OS_THREAD_STRUCT pOsThread = (OsThreadInitStruct *)&OsThreadInitTable[0]; //取出线程列表的第一个线程初始化
	while(pOsThread->FuncID < OS_THREAD_FUNC_ID_END)
	{
		if(pOsThread->ThreadInit != NULL)
		{
			pOsThread->ThreadInit(pOsThread->Priority,pOsThread->Tick);	/*线程初始化,入参为优先级以及tick*/
		}
		pOsThread++;
	}
}
RNG_HandleTypeDef hrng;

void MX_RNG_Init(void)
{
    hrng.Instance = RNG;
    if (HAL_RNG_Init(&hrng) != HAL_OK)
    {
        Error_Handler();
    }
}

extern int mbedtls_rsa_test(void);


int main(void)
{
    /* set LED0 pin mode to output */
	EepromHwInit();					/*eeprom初始化*/
//	my_wdt_init();					/*看门狗初始化*/
	my_timer_init(timeout_cb);		/*定时器初始化，参数为中断回调函数*/
	rt_semaphore_init(); 			/*生产者消费者线程用到的semphare初始化*/
	EventCtrlInit();				/*事件集初始化*/
	MX_RNG_Init();
	/*启动线程列表*/
	OsThreadInit();
	
	SendEvent(EVENT_FLAG_Main);
    WaitForThreadSync();
	rt_kprintf("The currrent Thread is %s!\n", "main");
	
    while (1)
    {
		TimerTaskProcess();			/*定时器相关的任务函数调用*/
		my_key_test_func();
    }
    return RT_EOK;
}
