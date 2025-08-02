#include "rsa.h"
#include <stdlib.h>

/* 快速幂模运算：计算 a^b mod c，避免溢出 */
int modpow(long long a, long long b, int c) {
    int res = 1;
    while (b > 0) {
        if (b & 1)
            res = (res * a) % c;
        b >>= 1;
        a = (a * a) % c;
    }
    return res;
}

/* 计算 Jacobi 符号 (a/n)，用于 Solovay-Strassen 素性测试 */
int jacobi(int a, int n) {
    int twos, temp;
    int mult = 1;

    while (a > 1 && a != n) {
        a = a % n;
        if (a <= 1 || a == n) break;

        // 提取 2 的因子
        twos = 0;
        while (a % 2 == 0 && ++twos) a /= 2;

        // 如果提取出奇数个 2，应用 Jacobi 性质调整符号
        if (twos > 0 && twos % 2 == 1)
            mult *= (n % 8 == 1 || n % 8 == 7) ? 1 : -1;

        if (a <= 1 || a == n) break;

        // 利用 Jacobi 对称性
        if (n % 4 != 1 && a % 4 != 1)
            mult *= -1;

        // 交换 a 和 n
        temp = a; a = n; n = temp;
    }

    if (a == 0) return 0;
    else if (a == 1) return mult;
    else return 0;
}

/* 判断 a 是否为 n 的欧拉见证 */
int solovayPrime(int a, int n) {
    int x = jacobi(a, n);
    if (x == -1) x = n - 1;
    return x != 0 && modpow(a, (n - 1) / 2, n) == x;
}

/* Solovay-Strassen 概率素性测试：重复 k 次测试 */
int probablePrime(int n, int k) {
    if (n == 2) return 1;
    else if (n % 2 == 0 || n == 1) return 0;

    while (k-- > 0) {
        if (!solovayPrime(rand() % (n - 2) + 2, n)) return 0;
    }
    return 1;
}

/* 在 [3, n-1] 之间随机生成素数 */
int randPrime(int n) {
    int prime = rand() % n;
    n += n % 2; // 保证 n 是偶数，便于后续步长为 2

    prime += 1 - prime % 2; // 保证 prime 是奇数
    while (1) {
        if (probablePrime(prime, ACCURACY))
            return prime;
        prime = (prime + 2) % n;
    }
}

/* 计算最大公约数 */
int gcd(int a, int b) {
    int temp;
    while (b != 0) {
        temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

/* 在 [3, n-1] 中找到与 phi 互素的随机指数 */
int randExponent(int phi, int n) {
    int e = rand() % n;
    while (1) {
        if (gcd(e, phi) == 1) return e;
        e = (e + 1) % n;
        if (e <= 2) e = 3;
    }
}

/* 扩展欧几里得算法求逆元 n^-1 mod modulus */
int inverse(int n, int modulus) {
    int a = n, b = modulus;
    int x = 0, y = 1, x0 = 1, y0 = 0, q, temp;
    while (b != 0) {
        q = a / b;
        temp = a % b;
        a = b;
        b = temp;

        temp = x; x = x0 - q * x; x0 = temp;
        temp = y; y = y0 - q * y; y0 = temp;
    }
    if (x0 < 0) x0 += modulus;
    return x0;
}

/* 加密单个数字 m：c = m^e mod n */
int encode(int m, int e, int n) {
    return modpow(m, e, n);
}

/* 解密单个数字 c：m = c^d mod n */
int decode(int c, int d, int n) {
    return modpow(c, d, n);
}

/* 加密字符串消息（按 bytes 打包） */
int *encodeMessage(int len, int bytes, char *message, int exponent, int modulus) {
    int *encoded = (int *)rt_malloc((len / bytes) * sizeof(int));
    int x, i, j;

    for (i = 0; i < len; i += bytes) {
        x = 0;
        for (j = 0; j < bytes; j++)
            x += message[i + j] * (1 << (7 * j)); // 7位移位方式（ASCII兼容）
        encoded[i / bytes] = encode(x, exponent, modulus);
    }
    return encoded;
}

/* 解密密文（转换为原始字符） */
int *decodeMessage(int len, int bytes, int *cryptogram, int exponent, int modulus) {
    int *decoded = (int *)rt_malloc(len * bytes * sizeof(int));
    int x, i, j;

    for (i = 0; i < len; i++) {
        x = decode(cryptogram[i], exponent, modulus);
        for (j = 0; j < bytes; j++) {
            decoded[i * bytes + j] = (x >> (7 * j)) % 128; // 还原字符
        }
    }
    return decoded;
}

/**
 * @brief RSA 加密解密演示线程入口
 */
static void rsa_demo_entry(void *parameter)
{
    rt_kprintf("[RSA] start RSA encrypt!\n");

    int p, q, n, phi, e, d, bytes, len;
    int *encoded, *decoded;
    char *buffer;

    srand(rt_tick_get());  // 使用系统 Tick 作为随机种子

    /* 步骤1：生成大素数 p 和 q，满足 n = p*q ≥ 128 */
    do {
//        p = randPrime(SINGLE_MAX);
//        q = randPrime(SINGLE_MAX);
		p = 11;
		q = 13;
        n = p * q;
    } while (n < 128);  // 确保模数 n 大于 128，以支持至少一个字节加密

    rt_kprintf("[RSA] random sushu p = %d, q = %d, n = %d\n", p, q, n);

    /* 步骤2：生成公钥与私钥 */
    phi = (p - 1) * (q - 1);      			// 欧拉函数
    e = randExponent(phi, EXPONENT_MAX); 	// 公钥指数
    d = inverse(e, phi);         			// 私钥指数

    rt_kprintf("[RSA] public key: (e = %d, n = %d)\n", e, n);
    rt_kprintf("[RSA] private key : (d = %d, n = %d)\n", d, n);

    /* 步骤3：准备明文 */
    char message[] = "Hello from RT-Thread RSA!";
    len = strlen(message);
    buffer = (char *)rt_malloc(len + 8);
    strcpy(buffer, message);

    /* 步骤4：根据模数 n 的大小确定每组加密字节数 */
    if (n >> 21) bytes = 3;
    else if (n >> 14) bytes = 2;
    else bytes = 1;

    /* 步骤5：加密 */
    encoded = encodeMessage(len, bytes, buffer, e, n);
    rt_kprintf("[RSA] after encrypt data: ");
    for (int i = 0; i < len / bytes; i++)
        rt_kprintf("%02x ", encoded[i]);
    rt_kprintf("\n");

    /* 步骤6：解密 */
    decoded = decodeMessage(len / bytes, bytes, encoded, d, n);
    rt_kprintf("[RSA] after decrypt data: ");
    for (int i = 0; i < len; i++)
        rt_kprintf("%c", decoded[i]);
    rt_kprintf("\n");

    /* 清理资源 */
    rt_free(buffer);
    rt_free(encoded);
    rt_free(decoded);

    rt_kprintf("[RSA] RSA example run finish!\n");
}

/**
 * @brief 初始化线程，自动运行
 */
int rsa_demo_init(void)
{
    rt_thread_t tid = rt_thread_create("rsa_demo",
                                       rsa_demo_entry, RT_NULL,
                                       4096, 20, 10);
    if (tid)
        rt_thread_startup(tid);
    return 0;
}

INIT_APP_EXPORT(rsa_demo_init);


