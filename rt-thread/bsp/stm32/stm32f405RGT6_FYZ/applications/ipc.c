#include "ipc.h"


//线程同步事件集
struct rt_event ThreadSync;
rt_uint32_t EventFlagValue = 0;


/****************************************/
/*****生产消费者信号量举例*/
/****************************************/

/* 用于放置生产的整数数组 */
rt_uint32_t array[MAXSEM];
/* 指向生产者、消费者在 array 数组中的读写位置 */
rt_uint32_t set, get;


struct rt_semaphore sem_lock;				/*锁信号量*/
struct rt_semaphore sem_empty, sem_full;	/*空/满信号量*/

/**
 * @name: EventCtrlInit
 * @msg: 事件集初始化，用于各个线程的同步
 * @param 无
 * @return the operation status, RT_EOK on successful
 * @note: 事件集标志设置为优先级，即优先级高的线程最先运行
 */
rt_err_t EventCtrlInit(void)
{
    rt_err_t result;

    result = rt_event_init(&ThreadSync,"ThreadSync",RT_IPC_FLAG_PRIO);
    if (result != RT_EOK)
    {
        rt_kprintf("init %s event failed.\n", "ThreadSync");
        return RT_ERROR;
    }

    return result;
}

/**
 * @name: SendEvent
 * @msg: 发送一个事件
 * @param {rt_uint32_t} EventFlag
 * @return the operation status, RT_EOK on successful
 * @note: 本质为SendEvent
 */
rt_err_t SendEvent(rt_uint32_t EventFlag)
{
    rt_err_t result;

    result = rt_event_send(&ThreadSync,EventFlag);

    return result;
}

/**
 * @name: WaitForThreadSync
 * @msg: 等待各线程涉及的资源（硬件/软件）初始化完成，达到线程同步的目的
 * @param 无
 * @return the operation status, RT_EOK on successful
 * @note: 本质为rt_event_recv
 */
rt_err_t WaitForThreadSync(void)
{
    rt_err_t result;

    result = rt_event_recv(&ThreadSync,(EVENT_FLAG_MyTestThreadInit\
                                        |EVENT_FLAG_MyOledThreadInit|EVENT_FLAG_MyConsumerThreadInit\
                                        |EVENT_FLAG_MyProducerThreadInit|EVENT_FLAG_Main),
										RT_EVENT_FLAG_AND,RT_WAITING_FOREVER,&EventFlagValue);
    
    return result;
}


/*
	信号量初始化
*/
void rt_semaphore_init(void)
{
	rt_sem_init(&sem_lock, 	"lock",     1,      RT_IPC_FLAG_PRIO);
	rt_sem_init(&sem_empty, "empty",   	MAXSEM, RT_IPC_FLAG_PRIO);
	rt_sem_init(&sem_full, 	"full",     0,      RT_IPC_FLAG_PRIO);
}

