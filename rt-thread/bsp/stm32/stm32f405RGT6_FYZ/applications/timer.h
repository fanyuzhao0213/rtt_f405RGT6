#ifndef _TIMER_H_
#define _TIMER_H_
#include <rtthread.h>
#include <rtdevice.h>

extern rt_err_t timeout_cb(rt_device_t dev, rt_size_t size);
extern int my_timer_init(rt_err_t (*Callback)(rt_device_t dev, rt_size_t size));
extern void TimerTaskProcess(void);
#endif


