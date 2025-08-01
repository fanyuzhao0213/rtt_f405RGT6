#include "my_oled.h"
#include "OLED.h"

static rt_uint32_t MyOledThreadInit_stack = 1024;

	
/*oled 线程入口函数*/
void MyOledThreadInitThread(void *parameter)
{
	OLED_Init();
	OLED_ShowString(20,0,"FYZ_SYSTERM",OLED_8X16);
	OLED_Update();				//更新显示
	
	SendEvent(EVENT_FLAG_MyOledThreadInit);
    WaitForThreadSync();
	
	rt_kprintf("The currrent Thread is %s!\n", "MyOledThreadInitThread");
	while(1)
	{
		rt_thread_mdelay(100);
	}
}

void MyOledThreadInit(rt_uint8_t ThreadPriority, rt_uint32_t ThreadTick)
{
	    /* 定义线程句柄 */
    rt_thread_t tid;
 
    /* 创建动态pin线程 ：优先级 25 ，时间片 5个系统滴答，线程栈512字节 */
    tid = rt_thread_create("oled_thread",
                  MyOledThreadInitThread,
                  RT_NULL,
                  MyOledThreadInit_stack,
                  ThreadPriority,
                  ThreadTick);
 
    /* 创建成功则启动动态线程 */
    if (tid != RT_NULL)
    {
		rt_kprintf("thread  MyOledThreadInit  start success!!\n");
    	rt_thread_startup(tid);
	}

}

