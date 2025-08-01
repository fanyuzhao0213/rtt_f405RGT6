#ifndef __MY_PIN_DRIVE_H__
#define __MY_PIN_DRIVE_H__

#include <rtthread.h>
#include <stm32f4xx.h>
#include <rtdevice.h>
#include <board.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 引脚编号，通过查看设备驱动文件drv_gpio.c确定 */
#ifndef BEEP_PIN_NUM
//    #define BEEP_PIN_NUM            35  /* PB0 */
	#define BEEP_PIN_NUM  			GET_PIN(C, 8)  //PC8 led
#endif
#ifndef KEY0_PIN_NUM
//    #define KEY0_PIN_NUM            55  /* PD8 */
	#define KEY0_PIN_NUM            GET_PIN(A, 0)  //PA10 key
#endif
#ifndef KEY1_PIN_NUM
    #define KEY1_PIN_NUM            GET_PIN(B, 0)   /* PD9 */
#endif

extern void MyTestThreadInit(rt_uint8_t ThreadPriority, rt_uint32_t ThreadTick);
#endif






