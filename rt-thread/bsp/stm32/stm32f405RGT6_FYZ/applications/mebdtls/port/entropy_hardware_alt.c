#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#ifdef MBEDTLS_ENTROPY_HARDWARE_ALT

#include "mbedtls/entropy_poll.h"
#include <string.h>
#include <stdint.h>
#include <rtthread.h>
#include "stm32f4xx_hal.h"

extern RNG_HandleTypeDef hrng;

int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen)
{
    size_t generated = 0;
    uint32_t random_value;

    *olen = 0;  // 初始化输出长度

    while (generated + 4 <= len)
    {
        if (HAL_RNG_GenerateRandomNumber(&hrng, &random_value) == HAL_OK)
        {
            memcpy(output + generated, &random_value, 4);
            generated += 4;
        }
        else
        {
            return -1; // 失败
        }
    }

    // 如果还有剩余字节
    size_t remain = len - generated;
    if (remain > 0)
    {
        if (HAL_RNG_GenerateRandomNumber(&hrng, &random_value) == HAL_OK)
        {
            memcpy(output + generated, &random_value, remain);
            generated += remain;
        }
        else
        {
            return -1;
        }
    }

    *olen = generated;
    return 0;
}

#endif /*MBEDTLS_ENTROPY_HARDWARE_ALT*/
                           



