#ifndef MY_KEY_H_
#define MY_KEY_H_

#include <rtthread.h>
#include <stm32f4xx.h>
#include <rtdevice.h>
#include <board.h>

/// @brief 按键状态定义
#define KEY_PRESSED             1   // 按键按下（有效）
#define KEY_UNPRESSED           0   // 按键未按下

/// @brief 按键时序阈值（单位：毫秒）
#define KEY_TIME_DOUBLE         200     // 双击最大间隔时间
#define KEY_TIME_LONG           2000    // 长按判定时间
#define KEY_TIME_REPEAT         100     // 连发触发间隔时间

/// @brief 按键数量
#define KEY_COUNT               1       // 当前支持的按键数量

/// @brief 按键编号定义
#define KEY_1                   0
#define KEY_2                   1
#define KEY_3                   2
#define KEY_4                   3

/// @brief 按键事件标志位（可组合）
#define KEY_HOLD                0x01    // 按键持续按住状态（保持）
#define KEY_DOWN                0x02    // 刚刚被按下
#define KEY_UP                  0x04    // 刚刚被释放
#define KEY_SINGLE              0x08    // 单击事件
#define KEY_DOUBLE              0x10    // 双击事件
#define KEY_LONG                0x20    // 长按事件
#define KEY_REPEAT              0x40    // 连续触发事件（保持不放时重复触发）


/**
 * @brief 按键初始化函数，配置按键引脚和中断
 */
void Key_Init(void);
/**
 * @brief 检查某个按键的状态（带事件标志位）
 * 
 * @param n     按键编号（0 ~ KEY_COUNT-1）
 * @param Flag  要检测的事件类型（如 KEY_SINGLE、KEY_LONG 等）
 * @return uint8_t 返回1表示事件发生，0表示未发生
 */
uint8_t Key_Check(uint8_t n, uint8_t Flag);

void Key_Tick(void);
void my_key_test_func(void);
#endif  // MY_KEY_H_
