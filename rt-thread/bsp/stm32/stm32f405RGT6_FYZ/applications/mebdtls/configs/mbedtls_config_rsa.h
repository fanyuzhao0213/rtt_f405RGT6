/**
 * @brief   Minimal configuration for RSA Function
 * @author  mculover666
 * @date    2020/09/27
*/

#ifndef _MBEDTLS_CONFIG_RSA_H_
#define _MBEDTLS_CONFIG_RSA_H_

/* System support */
#define MBEDTLS_HAVE_ASM
//#define MBEDTLS_HAVE_TIME

/* mbed feature support */
#define MBEDTLS_ENTROPY_HARDWARE_ALT
//#define MBEDTLS_NO_DEFAULT_ENTROPY_SOURCES
#define MBEDTLS_NO_PLATFORM_ENTROPY

/* mbed modules */
#define MBEDTLS_AES_C
#define MBEDTLS_AES_ROM_TABLES
#define MBEDTLS_CTR_DRBG_C
#define MBEDTLS_ENTROPY_C
#define MBEDTLS_SHA256_C
#define MBEDTLS_MD_C
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_GENPRIME
#define MBEDTLS_OID_C
#define MBEDTLS_RSA_C
#define MBEDTLS_PKCS1_V21
#define MBEDTLS_PKCS1_V15
#define MBEDTLS_SHA1_C


#define MBEDTLS_PEM_WRITE_C     // 生成 PEM 格式输出
#define MBEDTLS_BASE64_C        // PEM 格式依赖的 base64 编码模块
#define MBEDTLS_PK_WRITE_C      // 支持公钥/私钥写出接口
#define MBEDTLS_ASN1_WRITE_C    // 写 ASN.1 结构
#define MBEDTLS_PK_C


//写pem证书
#define MBEDTLS_PK_WRITE_C
#define MBEDTLS_PEM_WRITE_C
#define MBEDTLS_ASN1_WRITE_C

//读pem证书
#define MBEDTLS_PEM_PARSE_C
#define MBEDTLS_PK_PARSE_C
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_BASE64_C
#define MBEDTLS_FS_IO


#include "mbedtls/check_config.h"

#endif /* _MBEDTLS_CONFIG_RSA_H_ */

