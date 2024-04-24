#include "producer.h"
#include "ipc.h"

/*生产者thread*/
static rt_uint32_t MyProducerThreadInit_stack = 1024;

void MyProducerThreadInitThread(void *parameter)
{
	int cnt = 0;
	/*运行10次*/
	SendEvent(EVENT_FLAG_MyProducerThreadInit);
    WaitForThreadSync();
	rt_kprintf("The currrent Thread is %s!\n", "MyProducerThreadInitThread");
	
	
	while(cnt < 10)
	{
		/*获取1个空位*/
		rt_sem_take(&sem_empty,RT_WAITING_FOREVER);
		
		/*修改ARRAY内容，上锁*/
		rt_sem_take(&sem_lock,RT_WAITING_FOREVER);
		array[set % MAXSEM] = cnt + 1;
        rt_kprintf("the producer set a number: %d\n", array[set % MAXSEM]);
        set++;
        rt_sem_release(&sem_lock);

        /* 发布一个满位 */
        rt_sem_release(&sem_full);
        cnt++;

        /* 暂停一段时间 */
        rt_thread_mdelay(20);
	}
	rt_kprintf("the producer exit!\n");
}

void MyProducerThreadInit(rt_uint8_t ThreadPriority, rt_uint32_t ThreadTick)
{
	/* 定义线程句柄 */
    rt_thread_t tid;
 
    /* 创建动态pin线程 ：优先级 25 ，时间片 5个系统滴答，线程栈512字节 */
    tid = rt_thread_create("producer_thread",
                  MyProducerThreadInitThread,
                  RT_NULL,
                  MyProducerThreadInit_stack,
                  ThreadPriority,
                  ThreadTick);
 
    /* 创建成功则启动动态线程 */
    if (tid != RT_NULL)
    {
		rt_kprintf("thread MyProducerThreadInit start success!!\n");
    	rt_thread_startup(tid);
	}
}
