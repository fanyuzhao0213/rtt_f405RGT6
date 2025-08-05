#include "rsa.h"
#include <stdlib.h>

/* ������ģ���㣺���� a^b mod c��������� */
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

/* ���� Jacobi ���� (a/n)������ Solovay-Strassen ���Բ��� */
int jacobi(int a, int n) {
    int twos, temp;
    int mult = 1;

    while (a > 1 && a != n) {
        a = a % n;
        if (a <= 1 || a == n) break;

        // ��ȡ 2 ������
        twos = 0;
        while (a % 2 == 0 && ++twos) a /= 2;

        // �����ȡ�������� 2��Ӧ�� Jacobi ���ʵ�������
        if (twos > 0 && twos % 2 == 1)
            mult *= (n % 8 == 1 || n % 8 == 7) ? 1 : -1;

        if (a <= 1 || a == n) break;

        // ���� Jacobi �Գ���
        if (n % 4 != 1 && a % 4 != 1)
            mult *= -1;

        // ���� a �� n
        temp = a; a = n; n = temp;
    }

    if (a == 0) return 0;
    else if (a == 1) return mult;
    else return 0;
}

/* �ж� a �Ƿ�Ϊ n ��ŷ����֤ */
int solovayPrime(int a, int n) {
    int x = jacobi(a, n);
    if (x == -1) x = n - 1;
    return x != 0 && modpow(a, (n - 1) / 2, n) == x;
}

/* Solovay-Strassen �������Բ��ԣ��ظ� k �β��� */
int probablePrime(int n, int k) {
    if (n == 2) return 1;
    else if (n % 2 == 0 || n == 1) return 0;

    while (k-- > 0) {
        if (!solovayPrime(rand() % (n - 2) + 2, n)) return 0;
    }
    return 1;
}

/* �� [3, n-1] ֮������������� */
int randPrime(int n) {
    int prime = rand() % n;
    n += n % 2; // ��֤ n ��ż�������ں�������Ϊ 2

    prime += 1 - prime % 2; // ��֤ prime ������
    while (1) {
        if (probablePrime(prime, ACCURACY))
            return prime;
        prime = (prime + 2) % n;
    }
}

/* �������Լ�� */
int gcd(int a, int b) {
    int temp;
    while (b != 0) {
        temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

/* �� [3, n-1] ���ҵ��� phi ���ص����ָ�� */
int randExponent(int phi, int n) {
    int e = rand() % n;
    while (1) {
        if (gcd(e, phi) == 1) return e;
        e = (e + 1) % n;
        if (e <= 2) e = 3;
    }
}

/* ��չŷ������㷨����Ԫ n^-1 mod modulus */
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

/* ���ܵ������� m��c = m^e mod n */
int encode(int m, int e, int n) {
    return modpow(m, e, n);
}

/* ���ܵ������� c��m = c^d mod n */
int decode(int c, int d, int n) {
    return modpow(c, d, n);
}

/* �����ַ�����Ϣ���� bytes ����� */
int *encodeMessage(int len, int bytes, char *message, int exponent, int modulus) {
    int *encoded = (int *)rt_malloc((len / bytes) * sizeof(int));
    int x, i, j;

    for (i = 0; i < len; i += bytes) {
        x = 0;
        for (j = 0; j < bytes; j++)
            x += message[i + j] * (1 << (7 * j)); // 7λ��λ��ʽ��ASCII���ݣ�
        encoded[i / bytes] = encode(x, exponent, modulus);
    }
    return encoded;
}

/* �������ģ�ת��Ϊԭʼ�ַ��� */
int *decodeMessage(int len, int bytes, int *cryptogram, int exponent, int modulus) {
    int *decoded = (int *)rt_malloc(len * bytes * sizeof(int));
    int x, i, j;

    for (i = 0; i < len; i++) {
        x = decode(cryptogram[i], exponent, modulus);
        for (j = 0; j < bytes; j++) {
            decoded[i * bytes + j] = (x >> (7 * j)) % 128; // ��ԭ�ַ�
        }
    }
    return decoded;
}

/**
 * @brief RSA ���ܽ�����ʾ�߳����
 */
static void rsa_demo_entry(void *parameter)
{
    rt_kprintf("[RSA] start RSA encrypt!\n");

    int p, q, n, phi, e, d, bytes, len;
    int *encoded, *decoded;
    char *buffer;

    srand(rt_tick_get());  // ʹ��ϵͳ Tick ��Ϊ�������

    /* ����1�����ɴ����� p �� q������ n = p*q �� 128 */
    do {
//        p = randPrime(SINGLE_MAX);
//        q = randPrime(SINGLE_MAX);
		p = 11;
		q = 13;
        n = p * q;
    } while (n < 128);  // ȷ��ģ�� n ���� 128����֧������һ���ֽڼ���

    rt_kprintf("[RSA] random sushu p = %d, q = %d, n = %d\n", p, q, n);

    /* ����2�����ɹ�Կ��˽Կ */
    phi = (p - 1) * (q - 1);      			// ŷ������
    e = randExponent(phi, EXPONENT_MAX); 	// ��Կָ��
    d = inverse(e, phi);         			// ˽Կָ��

    rt_kprintf("[RSA] public key: (e = %d, n = %d)\n", e, n);
    rt_kprintf("[RSA] private key : (d = %d, n = %d)\n", d, n);

    /* ����3��׼������ */
    char message[] = "Hello from RT-Thread RSA!";
    len = strlen(message);
    buffer = (char *)rt_malloc(len + 8);
    strcpy(buffer, message);

    /* ����4������ģ�� n �Ĵ�Сȷ��ÿ������ֽ��� */
    if (n >> 21) bytes = 3;
    else if (n >> 14) bytes = 2;
    else bytes = 1;

    /* ����5������ */
    encoded = encodeMessage(len, bytes, buffer, e, n);
    rt_kprintf("[RSA] after encrypt data: ");
    for (int i = 0; i < len / bytes; i++)
        rt_kprintf("%02x ", encoded[i]);
    rt_kprintf("\n");

    /* ����6������ */
    decoded = decodeMessage(len / bytes, bytes, encoded, d, n);
    rt_kprintf("[RSA] after decrypt data: ");
    for (int i = 0; i < len; i++)
        rt_kprintf("%c", decoded[i]);
    rt_kprintf("\n");

    /* ������Դ */
    rt_free(buffer);
    rt_free(encoded);
    rt_free(decoded);

    rt_kprintf("[RSA] RSA example run finish!\n");
}

/**
 * @brief ��ʼ���̣߳��Զ�����
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


