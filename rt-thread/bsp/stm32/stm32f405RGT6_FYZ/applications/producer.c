#include "producer.h"
#include "ipc.h"

/*������thread*/
static rt_uint32_t MyProducerThreadInit_stack = 1024;

void MyProducerThreadInitThread(void *parameter)
{
	int cnt = 0;
	/*����10��*/
	SendEvent(EVENT_FLAG_MyProducerThreadInit);
    WaitForThreadSync();
	rt_kprintf("The currrent Thread is %s!\n", "MyProducerThreadInitThread");
	
	
	while(cnt < 10)
	{
		/*��ȡ1����λ*/
		rt_sem_take(&sem_empty,RT_WAITING_FOREVER);
		
		/*�޸�ARRAY���ݣ�����*/
		rt_sem_take(&sem_lock,RT_WAITING_FOREVER);
		array[set % MAXSEM] = cnt + 1;
        rt_kprintf("the producer set a number: %d\n", array[set % MAXSEM]);
        set++;
        rt_sem_release(&sem_lock);

        /* ����һ����λ */
        rt_sem_release(&sem_full);
        cnt++;

        /* ��ͣһ��ʱ�� */
        rt_thread_mdelay(20);
	}
	rt_kprintf("the producer exit!\n");
}

void MyProducerThreadInit(rt_uint8_t ThreadPriority, rt_uint32_t ThreadTick)
{
	/* �����߳̾�� */
    rt_thread_t tid;
 
    /* ������̬pin�߳� �����ȼ� 25 ��ʱ��Ƭ 5��ϵͳ�δ��߳�ջ512�ֽ� */
    tid = rt_thread_create("producer_thread",
                  MyProducerThreadInitThread,
                  RT_NULL,
                  MyProducerThreadInit_stack,
                  ThreadPriority,
                  ThreadTick);
 
    /* �����ɹ���������̬�߳� */
    if (tid != RT_NULL)
    {
		rt_kprintf("thread MyProducerThreadInit start success!!\n");
    	rt_thread_startup(tid);
	}
}
