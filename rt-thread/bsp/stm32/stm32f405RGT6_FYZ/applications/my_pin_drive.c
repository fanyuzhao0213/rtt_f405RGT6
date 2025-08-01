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
#define LED0_PIN    GET_PIN(C, 0)
#define LED1_PIN    GET_PIN(C, 1)
#define LED2_PIN    GET_PIN(C, 2)
#define LED3_PIN    GET_PIN(C, 3)

#define LED_NUM     3
static rt_base_t led_pins[LED_NUM] = { LED0_PIN, LED1_PIN, LED2_PIN};

// 初始化所有LED引脚
static void led_init(void)
{
    for (int i = 0; i < LED_NUM; i++)
    {
        rt_pin_mode(led_pins[i], PIN_MODE_OUTPUT);
        rt_pin_write(led_pins[i], PIN_HIGH); // 熄灭
    }
}

// 设置某个LED亮或灭
static void led_set(int index, rt_bool_t on)
{
    if (index >= 0 && index < LED_NUM)
        rt_pin_write(led_pins[index], on ? PIN_LOW : PIN_HIGH); // 低电平点亮
}

// 全灭
static void leds_off_all(void)
{
    for (int i = 0; i < LED_NUM; i++)
        led_set(i, RT_FALSE);
}

// 模式1：单灯轮流闪烁
static void led_mode_rotate(int delay_ms)
{
    for (int i = 0; i < LED_NUM; i++)
    {
        leds_off_all();
        led_set(i, RT_TRUE);
        rt_thread_mdelay(delay_ms);
    }
}

// 模式2：跑马灯（顺序亮灭）
static void led_mode_marquee(int delay_ms)
{
    for (int i = 0; i < LED_NUM; i++)
    {
        led_set(i, RT_TRUE);
        rt_thread_mdelay(delay_ms);
        led_set(i, RT_FALSE);
    }
}

// 模式3：全部闪烁
static void led_mode_all_blink(int delay_ms)
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < LED_NUM; j++)
            led_set(j, RT_TRUE);
        rt_thread_mdelay(delay_ms);
        for (int j = 0; j < LED_NUM; j++)
            led_set(j, RT_FALSE);
        rt_thread_mdelay(delay_ms);
    }
}

// 模式4：往返流水灯
static void led_mode_bounce(int delay_ms)
{
    for (int i = 0; i < LED_NUM; i++)
    {
        leds_off_all();
        led_set(i, RT_TRUE);
        rt_thread_mdelay(delay_ms);
    }
    for (int i = LED_NUM - 2; i > 0; i--)
    {
        leds_off_all();
        led_set(i, RT_TRUE);
        rt_thread_mdelay(delay_ms);
    }
}


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
	
	led_init();
	
	rt_kprintf("The currrent Thread is %s!\n", "MyTestThreadInitThread");

	while(1)
	{	
		led_mode_rotate(200);
        led_mode_marquee(100);
        led_mode_all_blink(300);
        led_mode_bounce(150);
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

