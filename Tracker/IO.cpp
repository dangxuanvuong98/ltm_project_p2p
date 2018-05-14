#include "stdafx.h"

void SendPack(NODE node, int cmd, char *buf, int len)
{
	struct OFFLINE_PACKAGE tmpPack;
	memset(&tmpPack, 0, sizeof(tmpPack));
	tmpPack.cmdCode = cmd;
	memcpy(tmpPack.data, buf, len);
	struct ONLINE_PACKAGE sendPack;
	memset(&sendPack, 0, sizeof(sendPack));
	memcpy(sendPack.md5Code, md5((char*)&tmpPack).c_str(), 32);
	rsaenc((char *)&tmpPack, sendPack.data, node.rsaKey, sizeof(tmpPack));
	send(node.socket, (char *)&sendPack, sizeof(sendPack), 0);
}

int RecvPack(NODE sourcenode, OFFLINE_PACKAGE &recvPack)
{
	struct ONLINE_PACKAGE tmpPack;
	memset(&tmpPack, 0, sizeof(tmpPack));
	memset(&recvPack, 0, sizeof(recvPack));
	int ret = recv(sourcenode.socket, (char *)&tmpPack, sizeof(tmpPack), 0);
	if (ret == SOCKET_ERROR)
	{
		return ret;
	}
	rsadec((char *)&(tmpPack.data), (char *)&recvPack, myNode.rsaKey, sizeof(tmpPack));
	char tmp[32];
	memcpy(tmp, md5((char*)&recvPack).c_str(), 32);
	if (strncmp(tmp, tmpPack.md5Code, 32) != 0)
	{
		recvPack.cmdCode = -1;
	}
	return 0;
}