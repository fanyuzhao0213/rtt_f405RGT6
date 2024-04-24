#ifndef _PRODUCER_H_
#define _PRODUCER_H_

#include <rtthread.h>
#include <rtdbg.h>
#include <board.h>
#include <rtdevice.h>


extern void MyProducerThreadInit(rt_uint8_t ThreadPriority, rt_uint32_t ThreadTick);

#endif
