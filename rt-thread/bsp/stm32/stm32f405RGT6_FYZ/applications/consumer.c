#include "consumer.h"
#include "ipc.h"

/*消费者thread*/


static rt_uint32_t MyConsumerThreadInit_stack = 1024;

void MyConsumerThreadInitThread(void *parameter)
{
	rt_uint32_t sum =0;
	SendEvent(EVENT_FLAG_MyConsumerThreadInit);
    WaitForThreadSync();
	rt_kprintf("The currrent Thread is %s!\n", "MyConsumerThreadInitThread");
	while(1)
	{
		/*获取一个满位  相当于取产品 */
		rt_sem_take(&sem_full,RT_WAITING_FOREVER);
		
		/*临界区进行上锁操作*/
		rt_sem_take(&sem_lock,RT_WAITING_FOREVER);
		sum += array[get % MAXSEM];
		rt_kprintf("the consumer get number: %d\n", array[get % MAXSEM]);
		get++;
		rt_sem_release(&sem_lock);
		
		/*释放一个空位*/
		rt_sem_release(&sem_empty);
		
		/*get == 10,消费者线程相应停止*/
		if(get == 10)
			break;
		rt_thread_delay(50);	
	}
	
	rt_kprintf("the consumer sum is: %d\n", sum);
    rt_kprintf("the consumer exit!\n");
}

void MyConsumerThreadInit(rt_uint8_t ThreadPriority, rt_uint32_t ThreadTick)
{
	    /* 定义线程句柄 */
    rt_thread_t tid;
 
    /* 创建动态pin线程 ：优先级 25 ，时间片 5个系统滴答，线程栈512字节 */
    tid = rt_thread_create("consumer_thread",
                  MyConsumerThreadInitThread,
                  RT_NULL,
                  MyConsumerThreadInit_stack,
                  ThreadPriority,
                  ThreadTick);
 
    /* 创建成功则启动动态线程 */
    if (tid != RT_NULL)
    {
		rt_kprintf("thread MyConsumerThreadInit start success!!\n");
    	rt_thread_startup(tid);
	}
}


