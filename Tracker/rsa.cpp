/*
C++ code for the RSA encryption algorithm
Written by Kaushik Balakrishnan, PhD
*/



#include "stdafx.h"

// check for prime: return 1 if prime; 0 otherwise
int check_prime(unsigned long long int n)
{
	if (n < 2)
	{
		return 0;
	}

	unsigned long int i;

	for (i = 2; i <= sqrt(n); i++)
	{
		if (n%i == 0) {
			return 0;
		}
	}

	return 1;
}


// encrypt
unsigned long int encrypt(unsigned long long int n, unsigned long long int e, unsigned long int m)
{
	unsigned long long int c;

	c = jpl(m, e, n);
	return (unsigned long int)c;
}


// decrypt
unsigned short int decrypt(unsigned long long int n, unsigned long long int d, unsigned long int c)
{
	unsigned long long int m;

	m = jpl(c, d, n);
	return (unsigned short int)m;
}

unsigned long long int mul(unsigned long long int a, unsigned long long int b, unsigned long long int m)
{
	a = a % m;
	b = b % m;
	if (b == 0) return 0;
	if (b % 2 == 0)
	{
		return (mul(a * 2, b / 2, m));
	}
	return ((mul(a * 2, b / 2, m)) + a) % m;
}


// (b^e)%m
unsigned long long int jpl(unsigned long long int b, unsigned long long int e, unsigned long long int m)
{
	if (m == 0)
	{
		return b;
	}
	b = b % m;
	if (e == 1) return b;
	if (e % 2 == 0)
	{
		unsigned long long int tmp = jpl(b, e / 2, m);
		return mul(tmp, tmp, m);
	}
	unsigned long long int tmp = jpl(b, e / 2, m);
	tmp = mul(tmp, tmp, m);
	return mul(tmp, b, m);
}