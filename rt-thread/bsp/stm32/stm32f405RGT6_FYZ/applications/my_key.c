#include "my_key.h"
#include "OLED.h"


/// @brief 按键标志数组（每个位代表一个状态，如 KEY_DOWN、KEY_UP 等）
uint8_t Key_Flag[KEY_COUNT];

/// @brief 初始化按键引脚
void Key_Init(void)
{
    rt_pin_mode(KEY0_PIN_NUM, PIN_MODE_INPUT_PULLDOWN);  // KEY_1
    rt_pin_mode(KEY1_PIN_NUM, PIN_MODE_INPUT_PULLDOWN);  // KEY_2
}

/// @brief 获取指定按键当前状态（是否按下）
/// @param n 按键编号
/// @return KEY_PRESSED 或 KEY_UNPRESSED
uint8_t Key_GetState(uint8_t n)
{
    if (n == KEY_1)
    {
        return rt_pin_read(KEY0_PIN_NUM) == 1 ? KEY_PRESSED : KEY_UNPRESSED;
    }
    else if (n == KEY_2)
    {
        return rt_pin_read(KEY1_PIN_NUM) == 1 ? KEY_PRESSED : KEY_UNPRESSED;
    }

    return KEY_UNPRESSED;
}

/// @brief 检查按键是否触发了特定事件（如 KEY_DOWN、KEY_LONG）
/// @param n 按键编号
/// @param Flag 要检测的按键事件类型
/// @return 1: 事件触发，0: 未触发
uint8_t Key_Check(uint8_t n, uint8_t Flag)
{
    if (Key_Flag[n] & Flag)
    {
        if (Flag != KEY_HOLD)  // HOLD 状态不清除
        {
            Key_Flag[n] &= ~Flag;
        }
        return 1;
    }
    return 0;
}

/// @brief 按键状态扫描与事件识别，建议每 5~10ms 调用一次
void Key_Tick(void)
{
    static uint8_t count = 0;
    static uint8_t curr_state[KEY_COUNT] = {0};
    static uint8_t prev_state[KEY_COUNT] = {0};
    static uint8_t step[KEY_COUNT] = {0};       // 状态机状态：0~4
    static uint16_t timer[KEY_COUNT] = {0};     // 多用途定时器

    for (uint8_t i = 0; i < KEY_COUNT; i++)
    {
        if (timer[i] > 0)
        {
            timer[i]--;
        }
    }

    count++;
    if (count >= 20)  // 大约 20ms（20 * 1ms）
    {
        count = 0;

        for (uint8_t i = 0; i < KEY_COUNT; i++)
        {
            prev_state[i] = curr_state[i];		//当前的状态赋值给上次状态这个地方是做消抖
            curr_state[i] = Key_GetState(i);

            // 长按状态持续置位
            if (curr_state[i] == KEY_PRESSED)			//如果按下，则置保持标志位
                Key_Flag[i] |= KEY_HOLD;
            else
                Key_Flag[i] &= ~KEY_HOLD;				//如果松开，则置保持标志位

            // 边沿检测（按下），如果当前是按下，上一次是没按下，则认为现在是按下的状态
            if (curr_state[i] == KEY_PRESSED && prev_state[i] == KEY_UNPRESSED)
                Key_Flag[i] |= KEY_DOWN;				//边沿检测（按下）

            // 边沿检测（松开）
            if (curr_state[i] == KEY_UNPRESSED && prev_state[i] == KEY_PRESSED)
                Key_Flag[i] |= KEY_UP;

            // 状态机处理（支持单击、双击、长按、连发）
            switch (step[i])
            {
                case 0: // 等待按下
                    if (curr_state[i] == KEY_PRESSED)
                    {
                        timer[i] = KEY_TIME_LONG;		//设置长按时间，开始倒计时
                        step[i] = 1;
                    }
                    break;

                case 1: // 检测是否松开或超时（长按）
                    if (curr_state[i] == KEY_UNPRESSED)
                    {
                        timer[i] = KEY_TIME_DOUBLE;		//设置双击超时，开始倒计时
                        step[i] = 2;  // 等待第二次按下
                    }
                    else if (timer[i] == 0)
                    {
                        Key_Flag[i] |= KEY_LONG;
                        timer[i] = KEY_TIME_REPEAT;
                        step[i] = 4;  // 长按连发模式
                    }
                    break;

                case 2: // 等待第二次按下（双击判断）
                    if (curr_state[i] == KEY_PRESSED)
                    {
                        Key_Flag[i] |= KEY_DOUBLE;
                        step[i] = 3;  // 等待释放
                    }
                    else if (timer[i] == 0)
                    {
                        Key_Flag[i] |= KEY_SINGLE;
                        step[i] = 0;
                    }
                    break;

                case 3: // 第二次按下后等待释放
                    if (curr_state[i] == KEY_UNPRESSED)
                    {
                        step[i] = 0;
                    }
                    break;

                case 4: // 长按连发状态
                    if (curr_state[i] == KEY_UNPRESSED)
                    {
                        step[i] = 0;
                    }
                    else if (timer[i] == 0)
                    {
                        Key_Flag[i] |= KEY_REPEAT;
                        timer[i] = KEY_TIME_REPEAT;  // 重装定时器
                    }
                    break;
            }
        }
    }
}


void my_key_test_func(void)
{
	for (int i = 0; i < KEY_COUNT; i++)
	{
		if (Key_Check(i, KEY_DOWN))
		{
			OLED_Printf(0, 16, OLED_6X8, "KEY[%d]: DOWN", i);
			OLED_Update();
		}
		if (Key_Check(i, KEY_UP))
		{
			OLED_Printf(0, 24, OLED_6X8, "KEY[%d]: UP  ", i);
			OLED_Update();
		}
		if (Key_Check(i, KEY_SINGLE))
		{
			OLED_Printf(0, 32, OLED_6X8, "KEY[%d]: CLICK ", i);
			OLED_Update();
		}
		if (Key_Check(i, KEY_DOUBLE))
		{
			OLED_Printf(0, 40, OLED_6X8, "KEY[%d]: DOUBLE", i);
			OLED_Update();
		}
		if (Key_Check(i, KEY_LONG))
		{
			OLED_Printf(0, 48, OLED_6X8, "KEY[%d]: LONG  ", i);
			OLED_Update();
		}

		if (Key_Check(i, KEY_REPEAT))
		{
			OLED_Printf(0, 56, OLED_6X8, "KEY[%d]: REPEAT", i);
			OLED_Update();
		}
	}

	rt_thread_mdelay(20); // 检查间隔，不宜太快
}