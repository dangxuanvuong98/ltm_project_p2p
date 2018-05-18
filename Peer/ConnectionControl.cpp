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
	connector.addr.sin_port = htons(TRACKER_DEFAULT_PORT);
	connector.addr.sin_family = AF_INET;

	bind(connector.toPeerSocket, (SOCKADDR FAR*)&connector.addr, sizeof(connector.addr));
	bind(connector.toTrackerSocket, (SOCKADDR FAR*)&connector.addr, sizeof(connector.addr));
	listen(connector.toPeerSocket, 1024);
}

void Disconnect(SOCKET s)
{
	EnterCriticalSection(&criticalSection);

	printf("Peer %s:%d da ngat ket noi\n", inet_ntoa(onlinePeer[s].addr.sin_addr), ntohs(onlinePeer[s].addr.sin_port));
	onlinePeer.erase(s);
	closesocket(s);

	LeaveCriticalSection(&criticalSection);
}