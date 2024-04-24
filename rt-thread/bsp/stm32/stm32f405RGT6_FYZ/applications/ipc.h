#ifndef _IPC_H_
#define _IPC_H_

#include <rtthread.h>
#include <rtdbg.h>
#include <board.h>
#include <rtdevice.h>
#include "stdlib.h"

//线程同步事件集相关
#define EVENT_FLAG_MyTestThreadInit               		(1 << 0)
#define EVENT_FLAG_MyOledThreadInit               		(1 << 1)
#define EVENT_FLAG_MyConsumerThreadInit           		(1 << 2)
#define EVENT_FLAG_MyProducerThreadInit             	(1 << 3)
#define EVENT_FLAG_Main                          		(1 << 4)


/* 定义最大 5 个元素能够被产生 */
#define MAXSEM 5
/* 用于放置生产的整数数组 */
extern rt_uint32_t array[MAXSEM];
/* 指向生产者、消费者在 array 数组中的读写位置 */
extern  rt_uint32_t set, get;

extern struct rt_semaphore sem_lock;				/*锁信号量*/
extern struct rt_semaphore sem_empty, sem_full;	/*空/满信号量*/

extern void rt_semaphore_init(void);


extern rt_err_t SendEvent(rt_uint32_t EventFlag);
extern rt_err_t WaitForThreadSync(void);
extern rt_err_t EventCtrlInit(void);
#endif