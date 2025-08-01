#ifndef __EEPROM_H__
#define __EEPROM_H__

#include <rtdef.h>
#include <rtdevice.h>
#include "rtconfig.h"

extern struct rt_i2c_bus_device *I2C1_BUS;

//由数据手册决定
#define EEPROM_ADDR         (0x50)

#define TEST_WRITE_ADDR			(0x0040)
#define TEST_WRITE_ADDR_LEN		4
extern rt_uint8_t EepromHwInit(void);
#endif



