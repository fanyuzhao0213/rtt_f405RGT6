#include "eeprom.h"


#define EEPROM_I2C_BUS_NAME "i2c1"
struct rt_i2c_bus_device *EepromI2CBus;

/**
 * @name: EepromHwInit
 * @msg:  EEPROM初始化
 * @param [in]
 *         无
 * @param [out]
 *         无 
 * @return 1:初始化失败；0：初始化成功
 * @note:  无
 *         
 */
rt_uint8_t EepromHwInit(void)
{
    EepromI2CBus = (struct rt_i2c_bus_device *)rt_device_find(EEPROM_I2C_BUS_NAME);

    if (RT_NULL == EepromI2CBus)
    {
        rt_kprintf("can't find EEPROM %s device!\n", EEPROM_I2C_BUS_NAME);
        return RT_ERROR;
    }
    else
    {
		rt_kprintf("find EEPROM %s device!\n", EEPROM_I2C_BUS_NAME);
        return RT_EOK;
    } 
}
// INIT_DEVICE_EXPORT(EepromHwInit);

/**
 * @name: ReadEeprom
 * @msg:  读EEPROM
 * @param [in]
 *         ReadAddr：EEPROM读的开始地址
 *         len：读取的数据长度
 * @param [out]
 *         buf：读数据缓冲区指针
 * @return -1:读失败；0：读取成功
 * @note:  1)读数据前需要首先发送读的地址
 *         2)EEPROM的的寄存器地址为双字节,首先发送寄存器地址的高8位字节地址,然后发送寄存器地址的低8位字节地址
 *         
 */
rt_err_t ReadEeprom(rt_uint16_t ReadAddr, rt_uint8_t *buf, rt_uint16_t len)
{
    struct rt_i2c_msg msgs[2] = {0};
    rt_uint8_t TempBuf[2] = {0};

    TempBuf[0] = (ReadAddr >> 8);
    TempBuf[1] = ReadAddr;

    //写读取的地址
    msgs[0].addr  = EEPROM_ADDR;
    msgs[0].flags = RT_I2C_WR;
    msgs[0].buf   = TempBuf;
    msgs[0].len   = 2;

    msgs[1].addr  = EEPROM_ADDR;
    msgs[1].flags = RT_I2C_RD;
    msgs[1].buf   = buf;
    msgs[1].len   = len;

    if (2 == rt_i2c_transfer(EepromI2CBus, msgs, 2))
    {
        return RT_EOK;
    }
    else
    {
        rt_kprintf("Failed to read EEPROM!\n");
        return -RT_ERROR;
    }
}

/**
 * @name: WriteEeprom
 * @msg:  写EEPROM
 * @param [in]
 *         WriteAddr：EEPROM开始写的地址
 *         data：写数据缓冲区指针
 *         len： 写数据长度
 * @param [out]
 *         无
 * @return -1:读失败；0：读取成功
 * @note:  1)写数据前需要首先发送写的地址
 *         2)EEPROM的的寄存器地址为双字节,首先发送寄存器地址的高8位字节地址,然后发送寄存器地址的低8位字节地址
 *         
 */
rt_err_t WriteEeprom(rt_uint16_t WriteAddr, rt_uint8_t *data, rt_uint16_t len)
{
    struct rt_i2c_msg msgs = {0};
    rt_uint8_t buf[len+2];

    //写地址
    buf[0] = (WriteAddr >> 8);
    buf[1] = WriteAddr;
    rt_memcpy(buf+2, data, len);

    msgs.addr  = EEPROM_ADDR;
    msgs.flags = RT_I2C_WR;
    msgs.buf   = buf;
    msgs.len   = (len + 2);

    if (1 == rt_i2c_transfer(EepromI2CBus, &msgs, 1))
    {
        return RT_EOK;
    }
    else
    {
        rt_kprintf("Failed to write EEPROM!\n");
        return -RT_ERROR;
    }
}

void eeprom_write_test(void)
{
	rt_uint8_t testbuff[4] = {0x12,0x34,0x56,0x78};
	if(WriteEeprom(TEST_WRITE_ADDR,testbuff,4) == RT_EOK)
	{
		 rt_kprintf("success to write EEPROM!\n");
	}
}

void eeprom_read_test(void)
{
	rt_uint8_t testbuff[4] = {0};
	rt_uint8_t i =0;
    if (RT_EOK == ReadEeprom(TEST_WRITE_ADDR, testbuff, 4))
    {
        //判断指定的EEPROM是否为空白区
        for (i = 0; i < 4; i++)
        {
            if (0xFF != testbuff[i])
            {
                break;
            }
        }
		if(i != 4)
		{
				rt_kprintf("From EEPROM data is %02x,%02x,%02x,%02x\n",testbuff[0],testbuff[1],testbuff[2],testbuff[3]);
		}
    }
}

MSH_CMD_EXPORT(eeprom_write_test, eeprom write test);
MSH_CMD_EXPORT(eeprom_read_test, eeprom read test);