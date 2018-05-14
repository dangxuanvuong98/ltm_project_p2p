#include "stdafx.h"

void InitListener(LISTENER &listener)
{
	UpdateKey(listener.rsaKey);

	FILE *f;
	do
	{
		f = fopen("user.dat", "r");
		if (f == NULL)
		{
			f = fopen("user.dat", "w");
			fclose(f);
		}
	} while (true);

	char user[32], pass[32];
	int ret;

	while (!feof(f))
	{
		ret = fscanf_s(f, "%s%s", user, 31, pass, 31);
		if (ret != 2)
		{
			break;
		}
		userList[user] = pass;
	}
	fclose(f);

	//Khoi tao socket
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	listener.s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	listener.addr.sin_addr.s_addr = htonl(INADDR_ANY);
	listener.addr.sin_port = htons(TRACKER_DEFAULT_PORT);
	listener.addr.sin_family = AF_INET;

	bind(listener.s, (SOCKADDR FAR*)&listener.addr, sizeof(listener.addr));

	onlinePeerAmount = 0;
	
	listen(listener.s, 1024);
	printf("Server da san sang\n");
}

int UpdateKey(unsigned long long int *rsaKey)
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