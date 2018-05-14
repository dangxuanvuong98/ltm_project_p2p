#include "stdafx.h"

/*void SendPack(NODE node, int cmd, char *buf, int len)
{
	struct OFFPACK tmpPack;
	memset(&tmpPack, 0, sizeof(tmpPack));
	tmpPack.cmdCode = cmd;
	memcpy(tmpPack.data, buf, len);
	struct ONLPACK sendPack;
	memset(&sendPack, 0, sizeof(sendPack));
	memcpy(sendPack.md5Code, md5((char*)&tmpPack).c_str(), 32);
	rsaenc((char *)&tmpPack, sendPack.data, node.rsaKey, sizeof(tmpPack));
	send(listener.socket, (char *)&sendPack, sizeof(sendPack), 0);
}

int RecvPack(NODE sourcenode, OFFPACK &recvPack)
{
	struct ONLPACK tmpPack;
	memset(&tmpPack, 0, sizeof(tmpPack));
	memset(&recvPack, 0, sizeof(recvPack));
	int ret = recv(sourcenode.socket, (char *)&tmpPack, sizeof(tmpPack), 0);
	if (ret == SOCKET_ERROR)
	{
		return ret;
	}
	rsadec((char *)&(tmpPack.data), (char *)&recvPack, listener.rsaKey, sizeof(tmpPack));
	char tmp[32];
	memcpy(tmp, md5((char*)&recvPack).c_str(), 32);
	if (strncmp(tmp, tmpPack.md5Code, 32) != 0)
	{
		recvPack.cmdCode = -1;
	}
	return 0;
}*/

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