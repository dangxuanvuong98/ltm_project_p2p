#include "stdafx.h"

//Khoi tao listener
void InitListener(LISTENER &listener)
{
	//Tao bo ma hoa
	CreateKey(listener.rsaKey);

	//Nhap thong tin user
	FILE *f;
	
	f = fopen("user.dat", "r");
	if (f == NULL)
	{
		f = fopen("user.dat", "w");
		fclose(f);
		f = fopen("user.dat", "r");
	}

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

	//Khoi tao listener
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	listener.s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	listener.addr.sin_addr.s_addr = htonl(INADDR_ANY);
	listener.addr.sin_port = htons(TRACKER_DEFAULT_PORT);
	listener.addr.sin_family = AF_INET;

	bind(listener.s, (SOCKADDR FAR*)&listener.addr, sizeof(listener.addr));
	listen(listener.s, 1024);

	onlinePeerAmount = 0;
	printf("Server da san sang o cong %d\n", ntohs(listener.addr.sin_port));
}

//Cho doi ket noi moi
DWORD WINAPI WaitForNewConnection(LPVOID lpParam)
{
	NODE newPeer;
	THREAD_PARAM newConnection;
	int len;

	while (true)
	{
		newPeer.addr = { 0 };
		len = sizeof(newPeer.addr);
		newConnection.s = accept(listener.s, (SOCKADDR FAR*)&(newPeer.addr), &len);
		strcpy(newPeer.username, "");
		onlinePeer[newConnection.s] = newPeer;
		onlinePeer[newConnection.s].connecting = true;
		newConnection.pause = true;

		CreateThread(0, 0, SetupConnection, &newConnection, 0, 0);
		while (newConnection.pause);
	}

	return 0;
}

//Thiet lap cho ket noi moi
DWORD WINAPI SetupConnection(LPVOID lpParam)
{
	SOCKET s = ((THREAD_PARAM FAR*)lpParam)->s;
	((THREAD_PARAM FAR*)lpParam)->pause = false;

	char buf[20];

	//Gui public key den peer
	memcpy(buf, (char*)(listener.rsaKey), sizeof(buf));
	send(s, buf, 16, 0);
	send(s, buf, 16, 0);

	fd_set readfds;
	timeval limit = { 10,0 };
	
	FD_ZERO(&readfds);
	FD_SET(s, &readfds);

	//Nhan public key cua peer
	int ret = select(0, &readfds, NULL, NULL, NULL);
	if (ret <= 0)
	{
		Disconnect(s);
		return 0;
	}
	
	ret = recv(s, buf, sizeof(buf), 0);
	if (ret != 18)
	{
		Disconnect(s);
		return 0;
	}

	EnterCriticalSection(&criticalSection);

	memcpy((char*)(onlinePeer[s].rsaKey), buf, sizeof(buf));
	memcpy((char*)(&onlinePeer[s].addr.sin_port), buf + 16, 2);

	//Chuyen peer sang trang thai co the tiep nhan ket noi
	onlinePeer[s].connecting = false;

	LeaveCriticalSection(&criticalSection);

	char sendbuf[4096];

	sprintf(sendbuf, "%d %d %s %d %llu %llu %s", NEW_PEER, AF_INET,
		inet_ntoa(onlinePeer[s].addr.sin_addr), onlinePeer[s].addr.sin_port,
		onlinePeer[s].rsaKey[0], onlinePeer[s].rsaKey[1],
		onlinePeer[s].username);

	OFFPACK sendPack;
	sendPack.cmdCode = POST;
	memcpy(sendPack.data, sendbuf, strlen(sendbuf));

	for (int i = 0; i < onlinePeerAmount; i++)
	{
		if (i != s)
			SendPack(i, sendPack, strlen(sendbuf));
	}

	printf("Co mot ket noi moi %s:%d\n", inet_ntoa(onlinePeer[s].addr.sin_addr), ntohs(onlinePeer[s].addr.sin_port));

	return 0;
}

//Ngat ket noi voi socket s
void Disconnect(SOCKET s)
{
	if (onlinePeer.find(s) == onlinePeer.end())
	{
		return;
	}

	EnterCriticalSection(&criticalSection);

	printf("Peer %s:%d da ngat ket noi\n", inet_ntoa(onlinePeer[s].addr.sin_addr), ntohs(onlinePeer[s].addr.sin_port));
	onlinePeer.erase(s);
	closesocket(s);

	LeaveCriticalSection(&criticalSection);

	char sendbuf[4096];

	sprintf(sendbuf, "%d %d %s %d %llu %llu %s", PEER_DISCONNECT, AF_INET,
		inet_ntoa(onlinePeer[s].addr.sin_addr), onlinePeer[s].addr.sin_port,
		onlinePeer[s].rsaKey[0], onlinePeer[s].rsaKey[1],
		onlinePeer[s].username);

	OFFPACK sendPack;
	sendPack.cmdCode = POST;
	memcpy(sendPack.data, sendbuf, strlen(sendbuf));

	for (CONNECTION::iterator it = onlinePeer.begin(); it != onlinePeer.end(); it++)
	{
		if (it->first != s)
			SendPack(it->first, sendPack, strlen(sendbuf));
	}
}