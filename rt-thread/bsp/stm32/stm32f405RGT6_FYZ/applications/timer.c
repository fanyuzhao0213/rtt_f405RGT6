#include "timer.h"
#include "my_key.h"


#define HWTIMER_DEV_NAME   "timer3"     /* 定时器名称 */

rt_device_t TimerDev = RT_NULL;

#define   PRINTF_1STIME				(1000)				/*1s打印函数时间戳---1000*/
#define   PRINTF_3STIME 			(3000)				/*3s打印函数时间戳---1000*/


#define TIMER_TASK_MAX_COUNTER 		(2)					/*执行任务的总个数*/

void printf_1S_func(void)
{
//	rt_kprintf("this is 1s timer func!\r\n");
	/*need user add func*/
}

void printf_3S_func(void)
{
//	rt_kprintf("this is 3s timer func!\r\n");
	/*need user add func*/
}

typedef enum
{
	TASK_STOP = 0,
	TASK_RUN,
}TimerTaskRunStatus;

typedef struct
{
	rt_uint8_t mRunFlag;				/*任务运行标志位*/
	rt_uint32_t	mTimerCountDown;		/*任务运行倒计时*/
	rt_uint32_t mTaskReloadTimer;		/*任务执行间隔重载值*/
	void (*mpFunc)();					/*任务执行函数*/
}TimeTaskInfo;


/*任务执行表*/
static TimeTaskInfo TimerTask[TIMER_TASK_MAX_COUNTER] = 
{
	{TASK_RUN,	PRINTF_1STIME,	PRINTF_1STIME,	printf_1S_func},
	{TASK_RUN,	PRINTF_3STIME,	PRINTF_3STIME,	printf_3S_func},
};

/*定时器1ms中断调用此函数,对应任务的时间到达之后，将运行标志位置1*/
void TimerTaskRemarks(void)
{
	for(int i =0; i<TIMER_TASK_MAX_COUNTER; i++)
	{
		if(TimerTask[i].mTimerCountDown)
		{
			TimerTask[i].mTimerCountDown--;
			if(TimerTask[i].mTimerCountDown == 0)
			{
				TimerTask[i].mRunFlag = TASK_RUN;
			}
		}
	}
}


/**
 * @name: TimerTaskProcess
 * @msg:  执行和定时器相关的函数
 * @param  无
 * @return 无
 * @note:  无
 */
void TimerTaskProcess(void)
{
	for(int i = 0; i<TIMER_TASK_MAX_COUNTER; i++)
	{
		if(TimerTask[i].mRunFlag == TASK_RUN)
		{
			TimerTask[i].mpFunc();					/*执行任务*/
			TimerTask[i].mRunFlag = TASK_STOP;		/*标志位复位*/
			TimerTask[i].mTimerCountDown = TimerTask[i].mTaskReloadTimer;		/*计数值重置,使能下一次函数运行*/
		}
	}
}	


/* 	
	定时器超时回调函数 
	1ms
*/
rt_err_t timeout_cb(rt_device_t dev, rt_size_t size)
{
	Key_Tick(); 			// 按键扫描（状态更新）
	TimerTaskRemarks();
    return 0;
}

int my_timer_init(rt_err_t (*Callback)(rt_device_t dev, rt_size_t size))
{
    rt_err_t ret = RT_EOK;
    rt_hwtimer_mode_t TimerMode = HWTIMER_MODE_PERIOD;		/*定时器模式为循环模式 */
    //定时器超时时间
    rt_hwtimerval_t TimerTimeOut = 
    {
        .sec = 0,
        .usec = 1000,
    };

    /* 查找定时器设备 */
    TimerDev = rt_device_find(HWTIMER_DEV_NAME);
    if (TimerDev == RT_NULL)
    {
        rt_kprintf("hwtimer sample run failed! can't find %s device!\n", HWTIMER_DEV_NAME);
        return RT_ERROR;
    }

    /* 以读写方式打开设备 */
    ret = rt_device_open(TimerDev, RT_DEVICE_OFLAG_RDWR);
    if (ret != RT_EOK)
    {
        rt_kprintf("open %s device failed!\n", HWTIMER_DEV_NAME);
        return ret;
    }

	
	//设置定时模式
    ret = rt_device_control(TimerDev, HWTIMER_CTRL_MODE_SET, &TimerMode);
    if (ret != RT_EOK)
    {
        rt_kprintf("set mode of %s device failed!\n", HWTIMER_DEV_NAME);
        return ret;
    }
	
    //设置超时回调函数
    rt_device_set_rx_indicate(TimerDev, Callback);

    //设置超时时间并启动定时器
    if(rt_device_write(TimerDev, 0, &TimerTimeOut, sizeof(TimerTimeOut)) != sizeof(TimerTimeOut))
    {
        rt_kprintf("Set timeout value failed!\n");
        return RT_ERROR;
    }
	rt_kprintf("Set timer success!\n");

    return ret;
}


