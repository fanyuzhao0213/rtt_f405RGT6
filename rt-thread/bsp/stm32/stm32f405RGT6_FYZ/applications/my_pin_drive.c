#include "my_pin_drive.h"


static rt_uint32_t MyTestThreadInit_stack = 1024;
/*
 * 程序清单：这是一个 PIN 设备使用例程
 * 例程导出了 pin_beep_sample 命令到控制终端
 * 命令调用格式：pin_beep_sample
 * 程序功能：通过按键控制蜂鸣器对应引脚的电平状态控制蜂鸣器
*/


/* 中断回调函数 ---------------------------------------------------------------------------- */
static void hdr_callback(void *args)
{
	char *str = args;
	rt_kprintf("KEY0 pressed. %s\n", str);
}



/* defined the LED0 pin: PB1 */
#define LED0_PIN    GET_PIN(C, 8)

/* MyTestThreadInitThread线程入口函数 -------------------------------------------------------------------------*/
void MyTestThreadInitThread(void *parameter)
{
//    /* 按键0引脚为输入模式 */
//    rt_pin_mode(KEY0_PIN_NUM, PIN_MODE_INPUT_PULLUP);
//    /* 绑定中断，下降沿模式，回调函数名为beep_on */
//    rt_pin_attach_irq(KEY0_PIN_NUM, PIN_IRQ_MODE_FALLING, hdr_callback, "----fyztest");
//    /* 使能中断 */
//    rt_pin_irq_enable(KEY0_PIN_NUM, PIN_IRQ_ENABLE);
	SendEvent(EVENT_FLAG_MyTestThreadInit);
    WaitForThreadSync();
	
	rt_kprintf("The currrent Thread is %s!\n", "MyTestThreadInitThread");
	rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
	while(1)
	{	
		rt_pin_write(LED0_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(LED0_PIN, PIN_LOW);
        rt_thread_mdelay(500);
	}
}


void MyTestThreadInit(rt_uint8_t ThreadPriority, rt_uint32_t ThreadTick)
{
	    /* 定义线程句柄 */
    rt_thread_t tid;
 
    /* 创建动态pin线程 ：优先级 25 ，时间片 5个系统滴答，线程栈512字节 */
    tid = rt_thread_create("pin_thread",
                  MyTestThreadInitThread,
                  RT_NULL,
                  MyTestThreadInit_stack,
                  ThreadPriority,
                  ThreadTick);
 
    /* 创建成功则启动动态线程 */
    if (tid != RT_NULL)
    {
		rt_kprintf("thread MyTestThreadInit start success!!\n");
    	rt_thread_startup(tid);
	}
}

/* 导出到 msh 命令列表中 */
//MSH_CMD_EXPORT(pin_beep_sample, pin beep sample);