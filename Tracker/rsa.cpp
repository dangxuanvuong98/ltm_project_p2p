/*
C++ code for the RSA encryption algorithm
Written by Kaushik Balakrishnan, PhD
*/



#include "stdafx.h"

using namespace std;

void rsaenc(char * ori, char * enc, unsigned long long int *key, unsigned long int len)
{
	unsigned short int *a = (unsigned short int *)ori;
	unsigned long int *b = (unsigned long int *)enc;
	int l = len / 2;
	for (int i = 0; i < l; i++)
	{
		b[i] = encrypt(key[0], key[1], a[i]);
	}
}

void rsadec(char * enc, char * ori, unsigned long long int *key, unsigned long int len)
{
	unsigned long int *a = (unsigned long int *)enc;
	unsigned short int *b = (unsigned short int *)ori;
	int l = len / 4;
	for (int i = 0; i < l; i++)
	{
		b[i] = decrypt(key[0], key[2], a[i]);
	}
}

int UpdateKey()
{

	double r;
	int a;
	unsigned long long int p, q, phin, k = 5;

	unsigned long long int
		&n = myNode.rsaKey[0],
		&e = myNode.rsaKey[1],
		&d = myNode.rsaKey[2];

	// generate 2 prime numbers < 65000

	a = 0;

	do
	{
		while (a == 0) {
			r = ((double)rand() / (RAND_MAX));
			p = unsigned long long int(r*65000.0);
			a = check_prime(p);
		}

		a = 0;

		while (a == 0) {
			r = ((double)rand() / (RAND_MAX));
			q = unsigned long long int(r*65000.0);
			a = check_prime(q);
		}

		n = p * q;
	} while (n <= 65536);

	phin = (p - 1)*(q - 1);


	// generate e such that 1<e<phin, e and phin are coprime


	a = 0;

	while (a == 0) {
		r = ((double)rand() / (RAND_MAX));
		e = unsigned long long int(0.1*r*double(phin));
		a = check_prime(e);
	}

	k = 5;
	while ((1 + k * phin) % e != 0) {
		k++;
	}

	d = (1 + k * phin) / e;

	return 0;
}



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