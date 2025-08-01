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

#define OS_THREAD_STRUCT OsThreadInitStruct *		/*�����߳̽ṹ���ض���*/
#define OS_THREAD_FUNC_ID_END  0X06					/*����̸߳���������ʵ����Ŀ�޸�*/


typedef void(*pOsThreadFuncInit)(rt_uint8_t,rt_uint32_t);	/*��������һ������ָ��*/

typedef struct
{
	const rt_uint8_t FuncID;				/*ID*/
	const pOsThreadFuncInit ThreadInit;		/*����*/
	const rt_uint16_t Priority;				/*���ȼ�*/
	const rt_uint32_t Tick;					/*tick��*/
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


/*�̳߳�ʼ������*/
static void OsThreadInit(void)
{
	/*OS_THREAD_STRUCT  #define�������߳̽ṹ��*/
	OS_THREAD_STRUCT pOsThread = (OsThreadInitStruct *)&OsThreadInitTable[0]; //ȡ���߳��б�ĵ�һ���̳߳�ʼ��
	while(pOsThread->FuncID < OS_THREAD_FUNC_ID_END)
	{
		if(pOsThread->ThreadInit != NULL)
		{
			pOsThread->ThreadInit(pOsThread->Priority,pOsThread->Tick);	/*�̳߳�ʼ��,���Ϊ���ȼ��Լ�tick*/
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
	EepromHwInit();					/*eeprom��ʼ��*/
//	my_wdt_init();					/*���Ź���ʼ��*/
	my_timer_init(timeout_cb);		/*��ʱ����ʼ��������Ϊ�жϻص�����*/
	rt_semaphore_init(); 			/*�������������߳��õ���semphare��ʼ��*/
	EventCtrlInit();				/*�¼�����ʼ��*/
	MX_RNG_Init();
	/*�����߳��б�*/
	OsThreadInit();
	
	SendEvent(EVENT_FLAG_Main);
    WaitForThreadSync();
	rt_kprintf("The currrent Thread is %s!\n", "main");
	
    while (1)
    {
		TimerTaskProcess();			/*��ʱ����ص�����������*/
		my_key_test_func();
    }
    return RT_EOK;
}
