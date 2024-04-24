#include "consumer.h"
#include "ipc.h"

/*������thread*/


static rt_uint32_t MyConsumerThreadInit_stack = 1024;

void MyConsumerThreadInitThread(void *parameter)
{
	rt_uint32_t sum =0;
	SendEvent(EVENT_FLAG_MyConsumerThreadInit);
    WaitForThreadSync();
	rt_kprintf("The currrent Thread is %s!\n", "MyConsumerThreadInitThread");
	while(1)
	{
		/*��ȡһ����λ  �൱��ȡ��Ʒ */
		rt_sem_take(&sem_full,RT_WAITING_FOREVER);
		
		/*�ٽ���������������*/
		rt_sem_take(&sem_lock,RT_WAITING_FOREVER);
		sum += array[get % MAXSEM];
		rt_kprintf("the consumer get number: %d\n", array[get % MAXSEM]);
		get++;
		rt_sem_release(&sem_lock);
		
		/*�ͷ�һ����λ*/
		rt_sem_release(&sem_empty);
		
		/*get == 10,�������߳���Ӧֹͣ*/
		if(get == 10)
			break;
		rt_thread_delay(50);	
	}
	
	rt_kprintf("the consumer sum is: %d\n", sum);
    rt_kprintf("the consumer exit!\n");
}

void MyConsumerThreadInit(rt_uint8_t ThreadPriority, rt_uint32_t ThreadTick)
{
	    /* �����߳̾�� */
    rt_thread_t tid;
 
    /* ������̬pin�߳� �����ȼ� 25 ��ʱ��Ƭ 5��ϵͳ�δ��߳�ջ512�ֽ� */
    tid = rt_thread_create("consumer_thread",
                  MyConsumerThreadInitThread,
                  RT_NULL,
                  MyConsumerThreadInit_stack,
                  ThreadPriority,
                  ThreadTick);
 
    /* �����ɹ���������̬�߳� */
    if (tid != RT_NULL)
    {
		rt_kprintf("thread MyConsumerThreadInit start success!!\n");
    	rt_thread_startup(tid);
	}
}


