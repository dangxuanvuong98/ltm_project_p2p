#include "stdafx.h"

void SendPack(SOCKET s, OFFPACK sendPack, int len)
{
	if (len > sizeof(sendPack.data))
	{
		return;
	}

	unsigned long int buflen = sizeof(sendPack.cmdCode) + len;

	ONLPACK encryptPack;
	memset(&encryptPack, 0, sizeof(encryptPack));

	memcpy(encryptPack.md5Code, md5((char*)&sendPack).c_str(), 32);
	rsaenc((char *)&encryptPack.data, (char *)&sendPack, onlinePeer[s].rsaKey, buflen);
	send(s, (char *)&encryptPack, 2 * buflen, 0);
}

int RecvPack(SOCKET s, OFFPACK &recvPack)
{
	ONLPACK encryptPack;
	memset(&encryptPack, 0, sizeof(encryptPack));

	unsigned long int ret = recv(s, (char *)&encryptPack, sizeof(encryptPack), 0);
	if (ret == SOCKET_ERROR)
	{
		Disconnect(s);
	}

	memset(&recvPack, 0, sizeof(recvPack));
	rsadec((char *)&(encryptPack.data), (char *)&recvPack, connector.rsaKey, ret);

	char tmp[32];
	memcpy(tmp, md5((char*)&recvPack).c_str(), 32);
	if (strncmp(tmp, encryptPack.md5Code, 32) != 0)
	{
		recvPack.cmdCode = ERROR_CMD;
	}
	return 0;
}

void rsaenc(char * ori, char * enc, unsigned long long int *key, unsigned long int len)
{
	unsigned short int *a = (unsigned short int *)ori;
	unsigned long int *b = (unsigned long int *)enc;
	unsigned long int l = len / 2;
	for (unsigned long int i = 0; i < l; i++)
	{
		b[i] = encrypt(key[0], key[1], a[i]);
	}
}

void rsadec(char * enc, char * ori, unsigned long long int *key, unsigned long int len)
{
	unsigned long int *a = (unsigned long int *)enc;
	unsigned short int *b = (unsigned short int *)ori;
	unsigned long int l = len / 4;
	for (unsigned long int i = 0; i < l; i++)
	{
		b[i] = decrypt(key[0], key[2], a[i]);
	}
}

int CreateKey(unsigned long long int *rsaKey)
{

	double r;
	int a;
	unsigned long long int p, q, phin, k = 5;

	unsigned long long int
		&n = rsaKey[0],
		&e = rsaKey[1],
		&d = rsaKey[2];

	// generate 2 prime numbers < 65000
	a = 0;
	do
	{
		while (a == 0)
		{
			r = ((double)rand() / (RAND_MAX));
			p = unsigned long long int(r*65000.0);
			a = check_prime(p);
		}

		a = 0;

		while (a == 0)
		{
			r = ((double)rand() / (RAND_MAX));
			q = unsigned long long int(r*65000.0);
			a = check_prime(q);
		}

		n = p * q;
	} while (n <= 65536);

	phin = (p - 1)*(q - 1);


	// generate e such that 1<e<phin, e and phin are coprime
	a = 0;
	while (a == 0)
	{
		r = ((double)rand() / (RAND_MAX));
		e = unsigned long long int(0.1*r*double(phin));
		a = check_prime(e);
	}

	k = 5;
	while ((1 + k * phin) % e != 0)
	{
		k++;
	}
	d = (1 + k * phin) / e;

	return 0;
}