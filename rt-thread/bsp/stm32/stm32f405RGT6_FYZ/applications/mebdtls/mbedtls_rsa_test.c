/**
 * @brief   RSA Function demo ��ʾ���� MbedTLS �� RSA ���ܽ���
 * @author  mculover666
 * @date    2020/09/27
*/

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_RSA_C)   // ֻ�ж����� MBEDTLS_RSA_C �ű������´���

#include <stdio.h>
#include "string.h"
#include "mbedtls/entropy.h"         // ��Դ���ṩ��������ӣ�
#include "mbedtls/ctr_drbg.h"        // �����������
#include "mbedtls/rsa.h"             // RSA ����ģ��
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

char buf[516];   // ��ʱ�����������ڴ�ӡ RSA ����

/**
 * @brief ��ӡ RSA ��Կ���ݣ��� 16 ������ʾ��
 */
static void dump_rsa_key(mbedtls_rsa_context *ctx)
{
    size_t olen;

    printf("\n  +++++++++++++++++ rsa keypair +++++++++++++++++\n\n");

    mbedtls_mpi_write_string(&ctx->N , 16, buf, sizeof(buf), &olen);
    printf("N: %s\n", buf); // ģ�� N

    mbedtls_mpi_write_string(&ctx->E , 16, buf, sizeof(buf), &olen);
    printf("E: %s\n", buf); // ��Կָ�� E

    mbedtls_mpi_write_string(&ctx->D , 16, buf, sizeof(buf), &olen);
    printf("D: %s\n", buf); // ˽Կ D

    mbedtls_mpi_write_string(&ctx->P , 16, buf, sizeof(buf), &olen);
    printf("P: %s\n", buf); // ���� P

    mbedtls_mpi_write_string(&ctx->Q , 16, buf, sizeof(buf), &olen);
    printf("Q: %s\n", buf); // ���� Q

    mbedtls_mpi_write_string(&ctx->DP, 16, buf, sizeof(buf), &olen);
    printf("DP: %s\n", buf); // DP = D mod (P-1)

    mbedtls_mpi_write_string(&ctx->DQ, 16, buf, sizeof(buf), &olen);
    printf("DQ: %s\n", buf); // DQ = D mod (Q-1)

    mbedtls_mpi_write_string(&ctx->QP, 16, buf, sizeof(buf), &olen);
    printf("QP: %s\n", buf); // QP = Q^(-1) mod P

    printf("\n  +++++++++++++++++ rsa keypair +++++++++++++++++\n\n");
}

/**
 * @brief ��ʮ�����ƴ�ӡ��� buffer ������
 */
static void dump_buf(uint8_t *buf, uint32_t len)
{
    for (int i = 0; i < len; i++) {
        printf("%s%02X%s", i % 16 == 0 ? "\r\n\t" : " ", 
                           buf[i], 
                           i == len - 1 ? "\r\n" : "");
    }
}

uint8_t output_buf[1024/8];   // ������������������ 2048 λ���� 256 �ֽڣ�


/**
 * @brief  ��������ִ�� RSA ��Կ���ɡ����ܡ�����
 */
int mbedtls_rsa_test(void)
{
    int ret;
    size_t olen;
    const char* msg = "fyz_sayHelloWorld";    // ��������Ϣ
    uint8_t decrypt_buf[20];           // ���ܽ��������

    const char *pers = "rsa_test";     // ������������������ַ���
    mbedtls_entropy_context entropy;   // ��Դ������
    mbedtls_ctr_drbg_context ctr_drbg; // CTR-DRBG �����������
    mbedtls_rsa_context ctx;           // RSA ������

    /* 1. ��ʼ���ṹ�� */
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_rsa_init(&ctx, MBEDTLS_RSA_PKCS_V21, MBEDTLS_MD_SHA1);  // ��ʼ�� RSA��ʹ�� PKCS#1 v2.1
	
//	mbedtls_rsa_init(&ctx, MBEDTLS_RSA_PKCS_V15, MBEDTLS_MD_SHA256);  // ��ʼ�� RSA��ʹ�� PKCS#1 v2.1

    /* 2. ��ʼ������������� */
    rt_kprintf( "\n  . Seeding the random number generator..." );
    ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                (const unsigned char *) pers, strlen(pers));
    if(ret != 0) {
        rt_kprintf(" failed\n  ! mbedtls_ctr_drbg_seed returned %d(-0x%04x)\n", ret, -ret);
        goto exit;
    }
    rt_kprintf(" ok\n");

    /* 3. ���� RSA ��Կ�� (1024-bit) */
    rt_kprintf("\n  . Generate RSA keypair..." );
    ret = mbedtls_rsa_gen_key(&ctx, mbedtls_ctr_drbg_random, &ctr_drbg, 1024, 65537);
    if(ret != 0) {
        rt_kprintf(" failed\n  ! mbedtls_rsa_gen_key returned %d(-0x%04x)\n", ret, -ret);
        goto exit;
    }
    rt_kprintf(" ok\n");
	
    /* ��ӡ��Կ���� */
    dump_rsa_key(&ctx);

    /* 4. ʹ�ù�Կ�������� */
    rt_kprintf("\n  . RSA pkcs1 encrypt..." );
    ret = mbedtls_rsa_pkcs1_encrypt(&ctx, mbedtls_ctr_drbg_random, &ctr_drbg, 
                                    MBEDTLS_RSA_PUBLIC, strlen(msg), 
                                    (uint8_t *)msg, output_buf);
    if(ret != 0) {
        rt_kprintf(" failed\n  ! mbedtls_rsa_pkcs1_encrypt returned %d(-0x%04x)\n", ret, -ret);
        goto exit;
    }
    rt_kprintf(" ok\n");

    /* ��ӡ���ܽ�� */
    dump_buf(output_buf, sizeof(output_buf));

    /* 5. ʹ��˽Կ���� */
    rt_kprintf("\n RSA pkcs1 decrypt..." );
    ret = mbedtls_rsa_pkcs1_decrypt(&ctx, mbedtls_ctr_drbg_random, &ctr_drbg,
                                    MBEDTLS_RSA_PRIVATE, &olen,
                                    output_buf, decrypt_buf, sizeof(decrypt_buf));
    if(ret != 0) {
        rt_kprintf(" failed\n  ! mbedtls_rsa_pkcs1_decrypt returned %d(-0x%04x)\n", ret, -ret);
        goto exit;
    }
    rt_kprintf(" ok\n");

    /* ��ӡ���ܽ�� */
    decrypt_buf[olen] = '\0';  // ��ӽ���������ֹ����
    rt_kprintf("decrypt result:[%s]\r\n", decrypt_buf);

	//��Կ��֤�ɹ� ���ɱ�׼ pem ��ʽ֤��
	get_key_from_rsa_context(&ctx);
	
	test_rsa_decrypt();
	
exit:
    /* 6. �ͷ���Դ */
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
    mbedtls_rsa_free(&ctx);

    return ret;
}

#endif /* MBEDTLS_RSA_C */


/*
rt_kprintf() ���ַ����е� \r\n�����У��� 0 �ֽڴ��������⣬��ӡ������ʱ���ܻᱻ�ضϻ����쳣��ʾ
��ȷ������ʹ�÷ֶδ�ӡ����һ���Դ�ӡ���� PEM �ַ���
*/
void safe_print_pem(const char *label, const char *pem)
{
    rt_kprintf("===== %s =====\r\n", label);
    // ���д�ӡ�����ⶪʧ
    const char *p = pem;
    while (*p)
    {
        char line[80] = {0};
        int len = 0;

        while (*p && *p != '\n' && len < sizeof(line) - 1)
        {
            line[len++] = *p++;
        }

        // ��ӡ��һ��
        rt_kprintf("%s\r\n", line);

        // ���� '\n'
        if (*p == '\n') p++;
    }
}



#include "mbedtls/pk.h"
unsigned char public_key_pem[275];
unsigned char private_key_pem[890];


/**
 * @brief ��ӡ������Ϊ HEX ��ʽ
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
    // ���� Base64 �����ĳ��ȣ���ԼΪ 4/3 ԭ���� + '\0'
    size_t estimated_output_len = ((len + 2) / 3) * 4;
    unsigned char *output = rt_malloc(estimated_output_len + 1);  // ��һ���ֽ����ڽ�β '\0'

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

        // �ֶδ�ӡÿ 64 ���ַ�
        for (size_t i = 0; i < olen; i += 64)
        {
            char line[65] = {0}; // ÿ�� 64 �ַ� + null
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
 * @brief �� mbedtls_rsa_context �е��� PEM ��ʽ��Կ��ִ�мӽ��ܲ���
 */
int get_key_from_rsa_context(mbedtls_rsa_context *rsa_context)
{
    int result = 0;
	mbedtls_rsa_context *rsa_pub;
	mbedtls_rsa_context *rsa_priv;
	const char *personalization = "Fr789jj-ikrkjfjs@";  // ����������������ĸ��Ի��ַ�������ǿ�أ�
	const char *plaintext = "hello 123456";             // Ҫ���ܵ���������
	
	size_t encrypted_len = 0;
    // ��ʼ���ṹ��
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

    /********** ���� PK context ���ڵ��� PEM **********/
    if ((result = mbedtls_pk_setup(&pk_context, mbedtls_pk_info_from_type(MBEDTLS_PK_RSA))) != 0)
        goto exit;

    if ((result = mbedtls_rsa_copy(mbedtls_pk_rsa(pk_context), rsa_context)) != 0)
        goto exit;

    // ������Կ
    if ((result = mbedtls_pk_write_pubkey_pem(&pk_context, public_key_pem, sizeof(public_key_pem))) != 0)
        goto exit;
    safe_print_pem("PUBLIC KEY", (const char *)public_key_pem);

    // ����˽Կ
    if ((result = mbedtls_pk_write_key_pem(&pk_context, private_key_pem, sizeof(private_key_pem))) != 0)
        goto exit;
    safe_print_pem("PRIVATE KEY", (const char *)private_key_pem);

    /********** �ӽ��ܲ��� **********/

	// ʹ����Դ�͸��Ի��ַ�����ʼ�� CTR-DRBG �����������
	if ((result = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
										(const unsigned char *)personalization, strlen(personalization))) != 0)
		goto exit;  // ��ʧ�ܣ���ת����Դ�ͷ��߼�

	// ���ڴ��е� PEM �ַ�����������Կ������
	if ((result = mbedtls_pk_parse_public_key(&pub_ctx, public_key_pem, strlen((char *)public_key_pem) + 1)) != 0)
		goto exit;

	// ���ڴ��е� PEM �ַ���������˽Կ������
	if ((result = mbedtls_pk_parse_key(&priv_ctx, private_key_pem, strlen((char *)private_key_pem) + 1, NULL, 0)) != 0)
		goto exit;

	// �� PK ��������ȡ RSA �����ģ�ʵ�ʵ� RSA �㷨�ṹ�壩
	rsa_pub = mbedtls_pk_rsa(pub_ctx);   // ��Կ������
	rsa_priv = mbedtls_pk_rsa(priv_ctx); // ˽Կ������

	// ������䷽ʽΪ PKCS#1 v2.1��Ҳ��Ϊ OAEP������ʹ�� SHA256 ��Ϊ hash �㷨
	mbedtls_rsa_set_padding(rsa_pub, MBEDTLS_RSA_PKCS_V21, MBEDTLS_MD_SHA1);
	mbedtls_rsa_set_padding(rsa_priv, MBEDTLS_RSA_PKCS_V21, MBEDTLS_MD_SHA1);

//	mbedtls_rsa_set_padding(rsa_pub, MBEDTLS_RSA_PKCS_V15, MBEDTLS_MD_SHA256);
//	mbedtls_rsa_set_padding(rsa_priv, MBEDTLS_RSA_PKCS_V15, MBEDTLS_MD_SHA256);

	// ========= RSA ���� =========
	// ʹ�ù�Կ���� plaintext ���ݣ���������� encrypted_buf ��
	if ((result = mbedtls_rsa_pkcs1_encrypt(rsa_pub, mbedtls_ctr_drbg_random, &ctr_drbg,
											MBEDTLS_RSA_PUBLIC, strlen(plaintext),
											(const uint8_t *)plaintext, encrypted_buf)) != 0)
		goto exit;

	// ��ӡ���ܺ���������ݣ�ʮ�����ƣ�
	print_hex("Encrypted Ciphertext", encrypted_buf, mbedtls_pk_get_len(&pub_ctx));
											
	encrypted_len = mbedtls_pk_get_len(&pub_ctx);
	print_base64("Encrypted Ciphertext", encrypted_buf, encrypted_len);
	// ========= RSA ���� =========
	// ʹ��˽Կ�������ģ�������浽 decrypted_buf�����ܺ󳤶ȴ��� decrypted_len
	if ((result = mbedtls_rsa_pkcs1_decrypt(rsa_priv, mbedtls_ctr_drbg_random, &ctr_drbg,
											MBEDTLS_RSA_PRIVATE, &decrypted_len,
											encrypted_buf, decrypted_buf,
											sizeof(decrypted_buf))) != 0)
		goto exit;

	// ����ַ�������������������
	decrypted_buf[decrypted_len] = '\0';

	// ��ӡ���ܺ����������
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
 * @brief ʹ�� PEM ��ʽ RSA ˽Կ�������ݣ�PKCS#1 v2.1 / OAEP + SHA256��
 * @param private_key_pem PEM ��ʽ�� RSA ˽Կ�ַ���������ͷβ��
 * @param encrypted       ����������ݣ����ľ���Կ���ܺ�Ľ����
 * @param encrypted_len   �������ݳ��ȣ��ֽڣ�
 * @param output          ���ܽ�����������
 * @param output_size     ���������������С
 * @param output_len      ���ܺ��ʵ�ʳ��ȣ����������
 * @return int            0 �ɹ����� 0 Ϊ MbedTLS ������
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

    // ��ʼ�������������
    if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                     (const unsigned char *)personalization, strlen(personalization))) != 0)
    {
        rt_kprintf("mbedtls_ctr_drbg_seed failed: -0x%04X\n", -ret);
        goto cleanup;
    }

    // ���� PEM ��ʽ��˽Կ
    if ((ret = mbedtls_pk_parse_key(&priv_ctx, (const unsigned char *)private_key_pem,
                                    strlen(private_key_pem) + 1, NULL, 0)) != 0)
    {
        rt_kprintf("mbedtls_pk_parse_key failed: -0x%04X\n", -ret);
        goto cleanup;
    }

    // ��ȡ RSA context
    if (!mbedtls_pk_can_do(&priv_ctx, MBEDTLS_PK_RSA)) {
        rt_kprintf("Provided key is not RSA\n");
        ret = -1;
        goto cleanup;
    }

    rsa_priv = mbedtls_pk_rsa(priv_ctx);
//	mbedtls_rsa_set_padding(rsa_priv, MBEDTLS_RSA_PKCS_V15, MBEDTLS_MD_SHA256);
    mbedtls_rsa_set_padding(rsa_priv, MBEDTLS_RSA_PKCS_V21, MBEDTLS_MD_SHA1);

    // ִ�н���
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


	// �������� base64 �����
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
		decrypted[decrypted_len] = '\0';  // ȷ���ַ�����ȫ
		rt_kprintf("decrypt success: %s\n", decrypted);
	} else {
		rt_kprintf("decrypt failed! failed type: -0x%04X\n", -ret);
	}

}
