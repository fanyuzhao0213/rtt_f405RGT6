#ifndef __RSA_H__
#define __RSA_H__

#include <rtthread.h>

#define ACCURACY       5       // Solovay ���Բ��Ե��ظ�����
#define SINGLE_MAX     10000   // �������ɵ����Χ
#define EXPONENT_MAX   1000    // ���Կָ����Χ

/* ������ģ���㣺���� a^b mod c */
int modpow(long long a, long long b, int c);
/* ���� Jacobi ���� (a/n) */
int jacobi(int a, int n);
/* �ж� a �Ƿ�Ϊ n ��ŷ����֤ */
int solovayPrime(int a, int n);
/* �ж� n �Ƿ���������������ʲ��ԣ� */
int probablePrime(int n, int k);
/* ����С�� n ��������� */
int randPrime(int n);
/* �����Լ�� gcd(a, b) */
int gcd(int a, int b);
/* �� [3, n-1] ����һ���� phi ���ص������ */
int randExponent(int phi, int n);
/* ʹ����չŷ������㷨������Ԫ n^-1 mod modulus */
int inverse(int n, int modulus);
/* ���ܵ������� m������ m^e mod n */
int encode(int m, int e, int n);
/* ���ܵ������� c������ c^d mod n */
int decode(int c, int d, int n);
/* �����ַ�����Ϣ���� bytes �����ת��Ϊ��������� */
int *encodeMessage(int len, int bytes, char *message, int exponent, int modulus);
/* �������ģ��ָ�Ϊ bytes �ֽ�ԭʼ�ַ����� */
int *decodeMessage(int len, int bytes, int *cryptogram, int exponent, int modulus);

#endif


