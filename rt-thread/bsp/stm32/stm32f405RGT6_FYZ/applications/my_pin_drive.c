#include "my_pin_drive.h"


static rt_uint32_t MyTestThreadInit_stack = 1024;
/*
 * �����嵥������һ�� PIN �豸ʹ������
 * ���̵����� pin_beep_sample ��������ն�
 * ������ø�ʽ��pin_beep_sample
 * �����ܣ�ͨ���������Ʒ�������Ӧ���ŵĵ�ƽ״̬���Ʒ�����
*/


/* �жϻص����� ---------------------------------------------------------------------------- */
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

// ��ʼ������LED����
static void led_init(void)
{
    for (int i = 0; i < LED_NUM; i++)
    {
        rt_pin_mode(led_pins[i], PIN_MODE_OUTPUT);
        rt_pin_write(led_pins[i], PIN_HIGH); // Ϩ��
    }
}

// ����ĳ��LED������
static void led_set(int index, rt_bool_t on)
{
    if (index >= 0 && index < LED_NUM)
        rt_pin_write(led_pins[index], on ? PIN_LOW : PIN_HIGH); // �͵�ƽ����
}

// ȫ��
static void leds_off_all(void)
{
    for (int i = 0; i < LED_NUM; i++)
        led_set(i, RT_FALSE);
}

// ģʽ1������������˸
static void led_mode_rotate(int delay_ms)
{
    for (int i = 0; i < LED_NUM; i++)
    {
        leds_off_all();
        led_set(i, RT_TRUE);
        rt_thread_mdelay(delay_ms);
    }
}

// ģʽ2������ƣ�˳������
static void led_mode_marquee(int delay_ms)
{
    for (int i = 0; i < LED_NUM; i++)
    {
        led_set(i, RT_TRUE);
        rt_thread_mdelay(delay_ms);
        led_set(i, RT_FALSE);
    }
}

// ģʽ3��ȫ����˸
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

// ģʽ4��������ˮ��
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


/* MyTestThreadInitThread�߳���ں��� -------------------------------------------------------------------------*/
void MyTestThreadInitThread(void *parameter)
{
//    /* ����0����Ϊ����ģʽ */
//    rt_pin_mode(KEY0_PIN_NUM, PIN_MODE_INPUT_PULLUP);
//    /* ���жϣ��½���ģʽ���ص�������Ϊbeep_on */
//    rt_pin_attach_irq(KEY0_PIN_NUM, PIN_IRQ_MODE_FALLING, hdr_callback, "----fyztest");
//    /* ʹ���ж� */
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
	    /* �����߳̾�� */
    rt_thread_t tid;
 
    /* ������̬pin�߳� �����ȼ� 25 ��ʱ��Ƭ 5��ϵͳ�δ��߳�ջ512�ֽ� */
    tid = rt_thread_create("pin_thread",
                  MyTestThreadInitThread,
                  RT_NULL,
                  MyTestThreadInit_stack,
                  ThreadPriority,
                  ThreadTick);
 
    /* �����ɹ���������̬�߳� */
    if (tid != RT_NULL)
    {
		rt_kprintf("thread MyTestThreadInit start success!!\n");
    	rt_thread_startup(tid);
	}
}

/* ������ msh �����б��� */
//MSH_CMD_EXPORT(pin_beep_sample, pin beep sample);

