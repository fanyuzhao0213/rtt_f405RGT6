#ifndef _IPC_H_
#define _IPC_H_

#include <rtthread.h>
#include <rtdbg.h>
#include <board.h>
#include <rtdevice.h>
#include "stdlib.h"

//�߳�ͬ���¼������
#define EVENT_FLAG_MyTestThreadInit               		(1 << 0)
#define EVENT_FLAG_MyOledThreadInit               		(1 << 1)
#define EVENT_FLAG_MyConsumerThreadInit           		(1 << 2)
#define EVENT_FLAG_MyProducerThreadInit             	(1 << 3)
#define EVENT_FLAG_Main                          		(1 << 4)


/* ������� 5 ��Ԫ���ܹ������� */
#define MAXSEM 5
/* ���ڷ����������������� */
extern rt_uint32_t array[MAXSEM];
/* ָ�������ߡ��������� array �����еĶ�дλ�� */
extern  rt_uint32_t set, get;

extern struct rt_semaphore sem_lock;				/*���ź���*/
extern struct rt_semaphore sem_empty, sem_full;	/*��/���ź���*/

extern void rt_semaphore_init(void);


extern rt_err_t SendEvent(rt_uint32_t EventFlag);
extern rt_err_t WaitForThreadSync(void);
extern rt_err_t EventCtrlInit(void);
#endif