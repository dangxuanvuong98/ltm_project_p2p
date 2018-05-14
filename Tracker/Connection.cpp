#include "stdafx.h"

using namespace std;

void InitConnetion(NODE newNode)
{
	char buf[16];

	memcpy(buf, (char *)&(myNode.rsaKey[0]), 8);
	memcpy(buf + 8, (char *)&(myNode.rsaKey[1]), 8);
	send(newNode.socket, buf, 16, 0);

	fd_set readfds;
	timeval limit = { 10, 0 };
	FD_ZERO(&readfds);
	FD_SET(newNode.socket, &readfds);

	int ret = select(0, &readfds, NULL, NULL, &limit);
	if (ret > 0)
	{
		int ret2 = recv(newNode.socket, buf, sizeof(buf), 0);
		if (ret2 != 16)
		{
			Disconnect(newNode);
		}

		newNode.rsaKey[0] = *((unsigned long long int *)buf);
		char tmpchar[8];
		memcpy(tmpchar, buf + 8, 8);
		newNode.rsaKey[1] = *((unsigned long long int *)tmpchar);
	}
	else
	{
		Disconnect(newNode);
	}

	newNode.login = false;
	onlinePeer.insert(newNode);

}

void Disconnect(NODE newNode)
{
	closesocket(newNode.socket);

	EnterCriticalSection(&criticalSection);
	if (onlinePeer.find(newNode) != onlinePeer.end())
	{
		onlinePeer.erase(newNode);
		onlinePeerAmount--;
	}
	LeaveCriticalSection(&criticalSection);

	printf("Peer %s:%d da ngat ket noi\n", inet_ntoa(newNode.addr.sin_addr), ntohs(newNode.addr.sin_port));
	fprintf(trackerLog, "Peer %s:%d da ngat ket noi\n", inet_ntoa(newNode.addr.sin_addr), ntohs(newNode.addr.sin_port));
}