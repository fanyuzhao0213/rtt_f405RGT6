/**
 * @brief   RSA Function demo 演示基于 MbedTLS 的 RSA 加密解密
 * @author  mculover666
 * @date    2020/09/27
*/

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_RSA_C)   // 只有定义了 MBEDTLS_RSA_C 才编译以下代码

#include <stdio.h>
#include "string.h"
#include "mbedtls/entropy.h"         // 熵源（提供随机数种子）
#include "mbedtls/ctr_drbg.h"        // 随机数生成器
#include "mbedtls/rsa.h"             // RSA 加密模块
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "mbedtls/base64.h"



#include "mbedtls/pk.h"
#include "mbedtls/pem.h"


int get_key_from_rsa_context(mbedtls_rsa_context *rsa_context);
void test_rsa_decrypt(void);
int rsa_decrypt_pkcs1_v21_sha256(const char *pem_key,
                                 const uint8_t *input, size_t input_len,
                                 uint8_t *output, size_t output_size,
                                 size_t *output_len);

char buf[516];   // 临时缓冲区，用于打印 RSA 参数

/**
 * @brief 打印 RSA 密钥内容（以 16 进制显示）
 */
static void dump_rsa_key(mbedtls_rsa_context *ctx)
{
    size_t olen;

    printf("\n  +++++++++++++++++ rsa keypair +++++++++++++++++\n\n");

    mbedtls_mpi_write_string(&ctx->N , 16, buf, sizeof(buf), &olen);
    printf("N: %s\n", buf); // 模数 N

    mbedtls_mpi_write_string(&ctx->E , 16, buf, sizeof(buf), &olen);
    printf("E: %s\n", buf); // 公钥指数 E

    mbedtls_mpi_write_string(&ctx->D , 16, buf, sizeof(buf), &olen);
    printf("D: %s\n", buf); // 私钥 D

    mbedtls_mpi_write_string(&ctx->P , 16, buf, sizeof(buf), &olen);
    printf("P: %s\n", buf); // 素数 P

    mbedtls_mpi_write_string(&ctx->Q , 16, buf, sizeof(buf), &olen);
    printf("Q: %s\n", buf); // 素数 Q

    mbedtls_mpi_write_string(&ctx->DP, 16, buf, sizeof(buf), &olen);
    printf("DP: %s\n", buf); // DP = D mod (P-1)

    mbedtls_mpi_write_string(&ctx->DQ, 16, buf, sizeof(buf), &olen);
    printf("DQ: %s\n", buf); // DQ = D mod (Q-1)

    mbedtls_mpi_write_string(&ctx->QP, 16, buf, sizeof(buf), &olen);
    printf("QP: %s\n", buf); // QP = Q^(-1) mod P

    printf("\n  +++++++++++++++++ rsa keypair +++++++++++++++++\n\n");
}

/**
 * @brief 以十六进制打印输出 buffer 的内容
 */
static void dump_buf(uint8_t *buf, uint32_t len)
{
    for (int i = 0; i < len; i++) {
        printf("%s%02X%s", i % 16 == 0 ? "\r\n\t" : " ", 
                           buf[i], 
                           i == len - 1 ? "\r\n" : "");
    }
}

uint8_t output_buf[1024/8];   // 加密输出缓冲区（最大 2048 位，即 256 字节）


/**
 * @brief  主函数：执行 RSA 密钥生成、加密、解密
 */
int mbedtls_rsa_test(void)
{
    int ret;
    size_t olen;
    const char* msg = "fyz_sayHelloWorld";    // 待加密消息
    uint8_t decrypt_buf[20];           // 解密结果缓冲区

    const char *pers = "rsa_test";     // 随机数生成器的种子字符串
    mbedtls_entropy_context entropy;   // 熵源上下文
    mbedtls_ctr_drbg_context ctr_drbg; // CTR-DRBG 随机数上下文
    mbedtls_rsa_context ctx;           // RSA 上下文

    /* 1. 初始化结构体 */
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_rsa_init(&ctx, MBEDTLS_RSA_PKCS_V21, MBEDTLS_MD_SHA1);  // 初始化 RSA，使用 PKCS#1 v2.1
	
//	mbedtls_rsa_init(&ctx, MBEDTLS_RSA_PKCS_V15, MBEDTLS_MD_SHA256);  // 初始化 RSA，使用 PKCS#1 v2.1

    /* 2. 初始化随机数生成器 */
    rt_kprintf( "\n  . Seeding the random number generator..." );
    ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                (const unsigned char *) pers, strlen(pers));
    if(ret != 0) {
        rt_kprintf(" failed\n  ! mbedtls_ctr_drbg_seed returned %d(-0x%04x)\n", ret, -ret);
        goto exit;
    }
    rt_kprintf(" ok\n");

    /* 3. 生成 RSA 密钥对 (1024-bit) */
    rt_kprintf("\n  . Generate RSA keypair..." );
    ret = mbedtls_rsa_gen_key(&ctx, mbedtls_ctr_drbg_random, &ctr_drbg, 1024, 65537);
    if(ret != 0) {
        rt_kprintf(" failed\n  ! mbedtls_rsa_gen_key returned %d(-0x%04x)\n", ret, -ret);
        goto exit;
    }
    rt_kprintf(" ok\n");
	
    /* 打印密钥参数 */
    dump_rsa_key(&ctx);

    /* 4. 使用公钥加密数据 */
    rt_kprintf("\n  . RSA pkcs1 encrypt..." );
    ret = mbedtls_rsa_pkcs1_encrypt(&ctx, mbedtls_ctr_drbg_random, &ctr_drbg, 
                                    MBEDTLS_RSA_PUBLIC, strlen(msg), 
                                    (uint8_t *)msg, output_buf);
    if(ret != 0) {
        rt_kprintf(" failed\n  ! mbedtls_rsa_pkcs1_encrypt returned %d(-0x%04x)\n", ret, -ret);
        goto exit;
    }
    rt_kprintf(" ok\n");

    /* 打印加密结果 */
    dump_buf(output_buf, sizeof(output_buf));

    /* 5. 使用私钥解密 */
    rt_kprintf("\n RSA pkcs1 decrypt..." );
    ret = mbedtls_rsa_pkcs1_decrypt(&ctx, mbedtls_ctr_drbg_random, &ctr_drbg,
                                    MBEDTLS_RSA_PRIVATE, &olen,
                                    output_buf, decrypt_buf, sizeof(decrypt_buf));
    if(ret != 0) {
        rt_kprintf(" failed\n  ! mbedtls_rsa_pkcs1_decrypt returned %d(-0x%04x)\n", ret, -ret);
        goto exit;
    }
    rt_kprintf(" ok\n");

    /* 打印解密结果 */
    decrypt_buf[olen] = '\0';  // 添加结束符，防止乱码
    rt_kprintf("decrypt result:[%s]\r\n", decrypt_buf);

	//密钥验证成功 生成标准 pem 格式证书
	get_key_from_rsa_context(&ctx);
	
	test_rsa_decrypt();
	
exit:
    /* 6. 释放资源 */
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
    mbedtls_rsa_free(&ctx);

    return ret;
}

#endif /* MBEDTLS_RSA_C */


/*
rt_kprintf() 对字符串中的 \r\n（换行）或 0 字节处理有问题，打印到串口时可能会被截断或者异常显示
正确做法：使用分段打印代替一次性打印整个 PEM 字符串
*/
void safe_print_pem(const char *label, const char *pem)
{
    rt_kprintf("===== %s =====\r\n", label);
    // 分行打印，避免丢失
    const char *p = pem;
    while (*p)
    {
        char line[80] = {0};
        int len = 0;

        while (*p && *p != '\n' && len < sizeof(line) - 1)
        {
            line[len++] = *p++;
        }

        // 打印这一行
        rt_kprintf("%s\r\n", line);

        // 跳过 '\n'
        if (*p == '\n') p++;
    }
}



#include "mbedtls/pk.h"
unsigned char public_key_pem[275];
unsigned char private_key_pem[890];


/**
 * @brief 打印二进制为 HEX 格式
 */
void print_hex(const char *label, const uint8_t *buf, size_t len)
{
    rt_kprintf("%s (%d bytes):\n", label, (int)len);
    for (size_t i = 0; i < len; i++)
    {
        rt_kprintf("%02X ", buf[i]);
        if ((i + 1) % 16 == 0)
            rt_kprintf("\n");
    }
    rt_kprintf("\n");
}

void print_base64(const char *title, const uint8_t *input, size_t len)
{
    // 计算 Base64 编码后的长度，大约为 4/3 原长度 + '\0'
    size_t estimated_output_len = ((len + 2) / 3) * 4;
    unsigned char *output = rt_malloc(estimated_output_len + 1);  // 多一个字节用于结尾 '\0'

    if (!output)
    {
        rt_kprintf("%s (Base64): [Memory Allocation Failed]\n", title);
        return;
    }

    size_t olen = 0;
    int ret = mbedtls_base64_encode(output, estimated_output_len + 1, &olen, input, len);
    if (ret == 0)
    {
        output[olen] = '\0'; // null-terminate
        rt_kprintf("\n%s (Base64):\n", title);

        // 分段打印每 64 个字符
        for (size_t i = 0; i < olen; i += 64)
        {
            char line[65] = {0}; // 每行 64 字符 + null
            size_t chunk_len = (olen - i > 64) ? 64 : (olen - i);
            memcpy(line, output + i, chunk_len);
            rt_kprintf("%s\n", line);
        }
        rt_kprintf("\n");
    }
    else
    {
        rt_kprintf("%s (Base64): [Encoding Failed]\n", title);
    }

    rt_free(output);
}


/**
 * @brief 从 mbedtls_rsa_context 中导出 PEM 格式密钥并执行加解密测试
 */
int get_key_from_rsa_context(mbedtls_rsa_context *rsa_context)
{
    int result = 0;
	mbedtls_rsa_context *rsa_pub;
	mbedtls_rsa_context *rsa_priv;
	const char *personalization = "Fr789jj-ikrkjfjs@";  // 用于随机数生成器的个性化字符串（增强熵）
	const char *plaintext = "hello 123456";             // 要加密的明文数据
	
	size_t encrypted_len = 0;
    // 初始化结构体
    mbedtls_pk_context pk_context;
    mbedtls_pk_context pub_ctx, priv_ctx;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;

    uint8_t encrypted_buf[256];
    uint8_t decrypted_buf[256] = {0};
    size_t decrypted_len = 0;

    mbedtls_pk_init(&pk_context);
    mbedtls_pk_init(&pub_ctx);
    mbedtls_pk_init(&priv_ctx);
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);

    /********** 设置 PK context 用于导出 PEM **********/
    if ((result = mbedtls_pk_setup(&pk_context, mbedtls_pk_info_from_type(MBEDTLS_PK_RSA))) != 0)
        goto exit;

    if ((result = mbedtls_rsa_copy(mbedtls_pk_rsa(pk_context), rsa_context)) != 0)
        goto exit;

    // 导出公钥
    if ((result = mbedtls_pk_write_pubkey_pem(&pk_context, public_key_pem, sizeof(public_key_pem))) != 0)
        goto exit;
    safe_print_pem("PUBLIC KEY", (const char *)public_key_pem);

    // 导出私钥
    if ((result = mbedtls_pk_write_key_pem(&pk_context, private_key_pem, sizeof(private_key_pem))) != 0)
        goto exit;
    safe_print_pem("PRIVATE KEY", (const char *)private_key_pem);

    /********** 加解密测试 **********/

	// 使用熵源和个性化字符串初始化 CTR-DRBG 随机数生成器
	if ((result = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
										(const unsigned char *)personalization, strlen(personalization))) != 0)
		goto exit;  // 若失败，跳转至资源释放逻辑

	// 从内存中的 PEM 字符串解析出公钥上下文
	if ((result = mbedtls_pk_parse_public_key(&pub_ctx, public_key_pem, strlen((char *)public_key_pem) + 1)) != 0)
		goto exit;

	// 从内存中的 PEM 字符串解析出私钥上下文
	if ((result = mbedtls_pk_parse_key(&priv_ctx, private_key_pem, strlen((char *)private_key_pem) + 1, NULL, 0)) != 0)
		goto exit;

	// 从 PK 容器中提取 RSA 上下文（实际的 RSA 算法结构体）
	rsa_pub = mbedtls_pk_rsa(pub_ctx);   // 公钥上下文
	rsa_priv = mbedtls_pk_rsa(priv_ctx); // 私钥上下文

	// 设置填充方式为 PKCS#1 v2.1（也称为 OAEP），并使用 SHA256 作为 hash 算法
	mbedtls_rsa_set_padding(rsa_pub, MBEDTLS_RSA_PKCS_V21, MBEDTLS_MD_SHA1);
	mbedtls_rsa_set_padding(rsa_priv, MBEDTLS_RSA_PKCS_V21, MBEDTLS_MD_SHA1);

//	mbedtls_rsa_set_padding(rsa_pub, MBEDTLS_RSA_PKCS_V15, MBEDTLS_MD_SHA256);
//	mbedtls_rsa_set_padding(rsa_priv, MBEDTLS_RSA_PKCS_V15, MBEDTLS_MD_SHA256);

	// ========= RSA 加密 =========
	// 使用公钥加密 plaintext 内容，结果保存在 encrypted_buf 中
	if ((result = mbedtls_rsa_pkcs1_encrypt(rsa_pub, mbedtls_ctr_drbg_random, &ctr_drbg,
											MBEDTLS_RSA_PUBLIC, strlen(plaintext),
											(const uint8_t *)plaintext, encrypted_buf)) != 0)
		goto exit;

	// 打印加密后的密文数据（十六进制）
	print_hex("Encrypted Ciphertext", encrypted_buf, mbedtls_pk_get_len(&pub_ctx));
											
	encrypted_len = mbedtls_pk_get_len(&pub_ctx);
	print_base64("Encrypted Ciphertext", encrypted_buf, encrypted_len);
	// ========= RSA 解密 =========
	// 使用私钥解密密文，输出保存到 decrypted_buf，解密后长度存于 decrypted_len
	if ((result = mbedtls_rsa_pkcs1_decrypt(rsa_priv, mbedtls_ctr_drbg_random, &ctr_drbg,
											MBEDTLS_RSA_PRIVATE, &decrypted_len,
											encrypted_buf, decrypted_buf,
											sizeof(decrypted_buf))) != 0)
		goto exit;

	// 添加字符串结束符，避免乱码
	decrypted_buf[decrypted_len] = '\0';

	// 打印解密后的明文内容
	rt_kprintf("Decrypted Result: [%s]\n", decrypted_buf);


exit:
    mbedtls_pk_free(&pk_context);
    mbedtls_pk_free(&pub_ctx);
    mbedtls_pk_free(&priv_ctx);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);

    return result;
}

/**
 * @brief 使用 PEM 格式 RSA 私钥解密数据（PKCS#1 v2.1 / OAEP + SHA256）
 * @param private_key_pem PEM 格式的 RSA 私钥字符串（包含头尾）
 * @param encrypted       输入加密数据（明文经公钥加密后的结果）
 * @param encrypted_len   加密数据长度（字节）
 * @param output          解密结果输出缓冲区
 * @param output_size     解密输出缓冲区大小
 * @param output_len      解密后的实际长度（输出参数）
 * @return int            0 成功，非 0 为 MbedTLS 错误码
 */
int rsa_decrypt_with_private_pem(const char *private_key_pem,
                                  const uint8_t *encrypted, size_t encrypted_len,
                                  uint8_t *output, size_t output_size,
                                  size_t *output_len)
{
    int ret;
    const char *personalization = "rsa-decrypt";
    mbedtls_pk_context priv_ctx;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;

    mbedtls_pk_init(&priv_ctx);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_entropy_init(&entropy);

	mbedtls_rsa_context *rsa_priv;
	
    *output_len = 0;
    memset(output, 0, output_size);

    // 初始化随机数生成器
    if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                     (const unsigned char *)personalization, strlen(personalization))) != 0)
    {
        rt_kprintf("mbedtls_ctr_drbg_seed failed: -0x%04X\n", -ret);
        goto cleanup;
    }

    // 解析 PEM 格式的私钥
    if ((ret = mbedtls_pk_parse_key(&priv_ctx, (const unsigned char *)private_key_pem,
                                    strlen(private_key_pem) + 1, NULL, 0)) != 0)
    {
        rt_kprintf("mbedtls_pk_parse_key failed: -0x%04X\n", -ret);
        goto cleanup;
    }

    // 提取 RSA context
    if (!mbedtls_pk_can_do(&priv_ctx, MBEDTLS_PK_RSA)) {
        rt_kprintf("Provided key is not RSA\n");
        ret = -1;
        goto cleanup;
    }

    rsa_priv = mbedtls_pk_rsa(priv_ctx);
//	mbedtls_rsa_set_padding(rsa_priv, MBEDTLS_RSA_PKCS_V15, MBEDTLS_MD_SHA256);
    mbedtls_rsa_set_padding(rsa_priv, MBEDTLS_RSA_PKCS_V21, MBEDTLS_MD_SHA1);

    // 执行解密
    ret = mbedtls_rsa_pkcs1_decrypt(rsa_priv,
                                    mbedtls_ctr_drbg_random, &ctr_drbg,
                                    MBEDTLS_RSA_PRIVATE,
                                    output_len,
                                    encrypted,
                                    output,
                                    output_size);
    if (ret != 0) {
        rt_kprintf("mbedtls_rsa_pkcs1_decrypt failed: -0x%04X\n", -ret);
    }

cleanup:
    mbedtls_pk_free(&priv_ctx);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
    return ret;
}


void test_rsa_decrypt(void)
{
	const char *pem_key =
		
	"-----BEGIN RSA PRIVATE KEY-----\n"
	"MIICXAIBAAKBgQCbWQUhc8p8Fe9JdCT4ROC+c95tk7iFiSkASlZExI4m3F8GgolP\n"
	"paiuUozWkjRQzkUk4+8IBk5AxZjKs6yxeP4QpF9kf6I1H4+NgQVpfQx0/Pm9k4Bc\n"
	"K7sH4cj4r6asa2ocJ5JiZYw97a7cTWgTclgQBrvr/9VcaoQoE80SQ4VHYwIDAQAB\n"
	"AoGAGWs2AzTCn44pQsAd4n/GtHMaJJzktvjr/A+t0P/GcMZjSBno5N7q3GdGgz+W\n"
	"DtjJiJFBRfNeL4wFhbljmwB8tbQA+gY1Q9v861OCu1PfUqGsBtePMSZvtggZeYF8\n"
	"maTE5OLEb7g5pK+Xe69vnCPwGuN9OkbVI9Oma8gpSkhQp8kCQQDU4cCrrJdaUh6q\n"
	"wYoAvQmKeLQe+CXyf34q+ACJxLF+t+rzvhIMmVc29ao2jQ/5A+OvUzm7JN3RDtvF\n"
	"ioubidHZAkEAutAJSHCbfA0ebFD1iqRrMl2xYunDVSe2XFyxv31gh8XcFIMxwzW/\n"
	"SD98SbKSO27Fivu/GTgTiPG5iKObKRBhmwJAEaXZbCH1tiYiprM+tor1VIZnID4g\n"
	"HzdUhUlj+gfVUxT6JFS4eFTp9wNspfMRxOn+oRU8LviCbwdVTs4Kfs1xGQJAOYDB\n"
	"6r9mzOeI3tSI0FVN/eHjMUw9aR/hPUr3F7dzGDfqR5IepjUMHSQVkOXbp1mwith0\n"
	"+UFOmg+2CX46y4gUUwJBAK5NIk5W9tk7cDNVJ8LLo5FSUXmQMXC4k7EvOPHlRAng\n"
	"166+g4Z3rDUa++8+1sMbOKtoLXcuzHBL5Jrh8wgSft0=\n"
	"-----END RSA PRIVATE KEY-----\n";

	const char *base64_cipher =
	"BcQYdRjJklhAqBK+fo4eslA2gDAK5Dge2mMBJ1UVLPjUppM0IkofpTuRXtxIFNDUuQffj7AlyD8uxV+ZZd+onGtKRULNZF8fypaXwaMgXPyPMCT71RSiqH1B2MzBeOMy+RlYw1Ne+Hahe7eILSafNQ3UnYr1jZsyNc8cc0Zjzxo=";


	// 加密数据 base64 解码后
	uint8_t encrypted_bin[256];
	size_t encrypted_len = 0;
	mbedtls_base64_decode(encrypted_bin, sizeof(encrypted_bin), &encrypted_len,
						  (const uint8_t *)base64_cipher, strlen(base64_cipher));

	uint8_t decrypted[256];
	size_t decrypted_len = 0;
	int ret = rsa_decrypt_with_private_pem(pem_key,
										   encrypted_bin, encrypted_len,
										   decrypted, sizeof(decrypted), &decrypted_len);
	if (ret == 0) {
		decrypted[decrypted_len] = '\0';  // 确保字符串安全
		rt_kprintf("decrypt success: %s\n", decrypted);
	} else {
		rt_kprintf("decrypt failed! failed type: -0x%04X\n", -ret);
	}

}
