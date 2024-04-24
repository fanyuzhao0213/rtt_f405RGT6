#include "ipc.h"


//�߳�ͬ���¼���
struct rt_event ThreadSync;
rt_uint32_t EventFlagValue = 0;


/****************************************/
/*****�����������ź�������*/
/****************************************/

/* ���ڷ����������������� */
rt_uint32_t array[MAXSEM];
/* ָ�������ߡ��������� array �����еĶ�дλ�� */
rt_uint32_t set, get;


struct rt_semaphore sem_lock;				/*���ź���*/
struct rt_semaphore sem_empty, sem_full;	/*��/���ź���*/

/**
 * @name: EventCtrlInit
 * @msg: �¼�����ʼ�������ڸ����̵߳�ͬ��
 * @param ��
 * @return the operation status, RT_EOK on successful
 * @note: �¼�����־����Ϊ���ȼ��������ȼ��ߵ��߳���������
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
 * @msg: ����һ���¼�
 * @param {rt_uint32_t} EventFlag
 * @return the operation status, RT_EOK on successful
 * @note: ����ΪSendEvent
 */
rt_err_t SendEvent(rt_uint32_t EventFlag)
{
    rt_err_t result;

    result = rt_event_send(&ThreadSync,EventFlag);

    return result;
}

/**
 * @name: WaitForThreadSync
 * @msg: �ȴ����߳��漰����Դ��Ӳ��/�������ʼ����ɣ��ﵽ�߳�ͬ����Ŀ��
 * @param ��
 * @return the operation status, RT_EOK on successful
 * @note: ����Ϊrt_event_recv
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
	�ź�����ʼ��
*/
void rt_semaphore_init(void)
{
	rt_sem_init(&sem_lock, 	"lock",     1,      RT_IPC_FLAG_PRIO);
	rt_sem_init(&sem_empty, "empty",   	MAXSEM, RT_IPC_FLAG_PRIO);
	rt_sem_init(&sem_full, 	"full",     0,      RT_IPC_FLAG_PRIO);
}

