#ifndef __RSA_H__
#define __RSA_H__

#include <rtthread.h>

#define ACCURACY       5       // Solovay 素性测试的重复次数
#define SINGLE_MAX     10000   // 素数生成的最大范围
#define EXPONENT_MAX   1000    // 最大公钥指数范围

/* 快速幂模运算：计算 a^b mod c */
int modpow(long long a, long long b, int c);
/* 计算 Jacobi 符号 (a/n) */
int jacobi(int a, int n);
/* 判断 a 是否为 n 的欧拉见证 */
int solovayPrime(int a, int n);
/* 判断 n 是否可能是素数（概率测试） */
int probablePrime(int n, int k);
/* 生成小于 n 的随机素数 */
int randPrime(int n);
/* 求最大公约数 gcd(a, b) */
int gcd(int a, int b);
/* 在 [3, n-1] 中找一个与 phi 互素的随机数 */
int randExponent(int phi, int n);
/* 使用扩展欧几里得算法计算逆元 n^-1 mod modulus */
int inverse(int n, int modulus);
/* 加密单个整数 m，计算 m^e mod n */
int encode(int m, int e, int n);
/* 解密单个整数 c，计算 c^d mod n */
int decode(int c, int d, int n);
/* 加密字符串消息：按 bytes 打包，转换为整数后加密 */
int *encodeMessage(int len, int bytes, char *message, int exponent, int modulus);
/* 解密密文：恢复为 bytes 字节原始字符数组 */
int *decodeMessage(int len, int bytes, int *cryptogram, int exponent, int modulus);

#endif


