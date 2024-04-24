#ifndef _OLED_H_
#define _OLED_H_

#include <rtthread.h>
#include <rtdbg.h>
#include <board.h>
#include <rtdevice.h>
#include "stdlib.h"

#define OLED_ADDR                  0x3C    /* 从机地址 */
#define Data_ADDR                  0x40    /* 写数据地址 */
#define Command_ADDR               0x00    /* 写命令地址 */
#define Max_Column  128

rt_err_t write_reg(struct rt_i2c_bus_device *bus, rt_uint8_t reg, rt_uint8_t data);
void OLED_ShowChar(struct rt_i2c_bus_device *bus,rt_uint8_t x,rt_uint8_t y,rt_uint8_t chr,rt_uint8_t Char_Size);
void OLED_On(struct rt_i2c_bus_device *bus);
void OLED_Display_Off(struct rt_i2c_bus_device *bus);
void OLED_Display_On(struct rt_i2c_bus_device *bus);
void OLED_Set_Pos(struct rt_i2c_bus_device *bus,rt_uint8_t x,rt_uint8_t y);
void OLED_Clear(struct rt_i2c_bus_device *bus);
void OLED_Init(struct rt_i2c_bus_device *bus);
void OLED_ShowNum(struct rt_i2c_bus_device *bus,rt_uint8_t x,rt_uint8_t y,rt_uint32_t num,rt_uint8_t len,rt_uint8_t size2);
void OLED_ShowString(struct rt_i2c_bus_device *bus,rt_uint8_t x,rt_uint8_t y,rt_uint8_t *chr,rt_uint8_t Char_Size);
void OLED_ShowCHinese(struct rt_i2c_bus_device *bus,rt_uint8_t x,rt_uint8_t y,rt_uint8_t no);
rt_uint32_t OLED_pow(rt_uint8_t m,rt_uint8_t n);

extern void MyOledThreadInit(rt_uint8_t ThreadPriority, rt_uint32_t ThreadTick);
#endif /* WORK_OLED_H_ */

