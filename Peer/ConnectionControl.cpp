#include "stdafx.h"

void InitConnector(CONNECTOR &connector)
{
	//Tao bo ma hoa
	CreateKey(connector.rsaKey);

	//Khoi tao connector
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	connector.toPeerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	connector.toTrackerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	connector.addr.sin_addr.s_addr = htonl(INADDR_ANY);
	connector.addr.sin_port = htons(9001);
	connector.addr.sin_family = AF_INET;

	int ret;

	while (true)
	{
		ret = bind(connector.toPeerSocket, (SOCKADDR FAR*)&connector.addr, sizeof(connector.addr));
		if (ret == 0)
		{
			break;
		}
		else
		{
			connector.addr.sin_port++;
		}
	}
	
	listen(connector.toPeerSocket, 1024);

	printf("my port: %d\n", ntohs(connector.addr.sin_port));
}

DWORD WINAPI SetupConnection(LPVOID lpParam)
{
	char buf[20];

	fd_set readfds;
	timeval limit{ 10, 0 };

	FD_ZERO(&readfds);
	FD_SET(connector.toTrackerSocket, &readfds);

	int ret = select(0, &readfds, NULL, NULL, NULL);
	if (ret <= 0)
	{
		Disconnect(connector.toTrackerSocket);
		connected = false;
		return 0;
	}

	ret = recv(connector.toTrackerSocket, buf, sizeof(buf), 0);
	if (ret != 16)
	{
		Disconnect(connector.toTrackerSocket);
		connected = false;
		return 0;
	}

	memcpy((char*)(trackerNode.rsaKey), buf, sizeof(buf));

	memcpy(buf, (char *)(connector.rsaKey), 16);
	memcpy(buf + 16, (char *)(&connector.addr.sin_port), 2);
	send(connector.toTrackerSocket, buf, 18, 0);

	connected = true;

	return 0;
}

void Disconnect(SOCKET s)
{
	EnterCriticalSection(&criticalSection);

	printf("Peer %s:%d da ngat ket noi\n", inet_ntoa(onlinePeer[s].addr.sin_addr), ntohs(onlinePeer[s].addr.sin_port));
	onlinePeer.erase(s);
	closesocket(s);

	LeaveCriticalSection(&criticalSection);
}