#include "stdafx.h"

void InitListener(LISTENER &listener)
{
	CreateKey(listener.rsaKey);

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

DWORD WINAPI WaitForNewConnection(LPVOID lpParam)
{
	NODE newPeer;
	THREAD_PARAM newConnection;
	int len = sizeof(newPeer.addr);

	while (true)
	{
		newPeer.addr = { 0 };
		newConnection.s = accept(listener.s, (SOCKADDR FAR*)&newPeer.addr, &len);
		strcpy(newPeer.username, "");
		onlinePeer[newConnection.s] = newPeer;
		newConnection.pause = true;
		CreateThread(0, 0, SetupConnection, &newConnection, 0, 0);
		while (newConnection.pause);
	}

	return 0;
}

DWORD WINAPI SetupConnection(LPVOID lpParam)
{
	EnterCriticalSection(&criticalSection);
	SOCKET s = ((THREAD_PARAM FAR*)lpParam)->s;
	((THREAD_PARAM FAR*)lpParam)->pause = false;
	LeaveCriticalSection(&criticalSection);

	char buf[16];

	memcpy(buf, (char*)(listener.rsaKey), sizeof(buf));
	send(s, buf, 16, 0);

	fd_set readfds;
	timeval limit = { 10,0 };
	
	FD_ZERO(&readfds);
	FD_SET(s, &readfds);

	int ret = select(0, &readfds, NULL, NULL, &limit);
	if (ret <= 0)
	{
		Disconnect(s);
		return 0;
	}
	
	ret = recv(s, buf, sizeof(buf), 0);
	if (ret != 16)
	{
		Disconnect(s);
		return 0;
	}

	memcpy((char*)(onlinePeer[s].rsaKey), buf, sizeof(buf));

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