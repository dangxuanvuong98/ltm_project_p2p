// Peer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define MAX_NUMBER 1024
#define MAX_LENGTH 256
#define BUFFER_LENGTH 4096
#define PUBLIC MAX_NUMBER
#define TRACKER_ADDR "127.0.0.1"
#define TRACKER_PORT 9000
#define MIN_PEER_PORT 9001
#define MAX_PEER_PORT 65000

DWORD WINAPI UserCLI(LPVOID);
DWORD WINAPI DownloadFile(LPVOID lpParam);

SOCKET connectedPeer[MAX_NUMBER];
char connectedName[MAX_NUMBER][MAX_LENGTH];
SOCKADDR_IN knownPeer[MAX_NUMBER];
char peerName[MAX_NUMBER][MAX_LENGTH];
int connectedAmount = 0;
int knownAmount = 0;

char fileName[MAX_NUMBER][MAX_LENGTH];
char filePass[MAX_NUMBER][MAX_LENGTH];
SOCKADDR_IN fileLocate[MAX_NUMBER];
int fileAmount = 0;
int MODE = PUBLIC;

char user[MAX_LENGTH];

int main()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	int tmpport = MIN_PEER_PORT;

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	while (true)
	{
		addr.sin_port = htons(tmpport);
		int ret = bind(listener, (SOCKADDR FAR*)&addr, sizeof(addr));
		if (ret == 0)
		{
			break;
		}
		if (tmpport == MAX_PEER_PORT)
		{
			return -1;
		}
		tmpport++;
	}
	listen(listener, MAX_NUMBER);

	addr.sin_addr.s_addr = inet_addr(TRACKER_ADDR);
	addr.sin_port = htons(TRACKER_PORT);
	SOCKET connector = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	printf("Nhap username: ");
	fgets(user, MAX_LENGTH - 1, stdin);

	int c = connect(connector, (SOCKADDR FAR*)&addr, sizeof(addr));

	if (c < 0)
	{
		printf("Error: %d\n", GetLastError());
	}

	char buf[BUFFER_LENGTH];

	printf("Peer: %hu\n", tmpport);

	sprintf(buf, "%hu %s\n", htons(tmpport), user);
	send(connector, buf, strlen(buf), 0);

	fd_set readfds;

	CreateThread(0, 0, UserCLI, NULL, 0, 0);

	timeval timelimit = { 0, 1 };

	while (true)
	{
		FD_ZERO(&readfds);
		FD_SET(listener, &readfds);
		FD_SET(connector, &readfds);
		for (int i = 0; i < connectedAmount; i++)
		{
			FD_SET(connectedPeer[i], &readfds);
		}

		int ret = select(0, &readfds, NULL, NULL, NULL);

		if (ret < 0)
		{
			printf("Error: %d\n", GetLastError());
			continue;
		}

		if (ret == 0)
		{
			continue;
			printf("Timeout!\n");
		}

		for (int i = 0; i < connectedAmount; i++)
		{
			if (FD_ISSET(connectedPeer[i], &readfds))
			{
				int len = recv(connectedPeer[i], buf, sizeof(buf), 0);
				if (len <= 0)
				{
					connectedAmount--;
					printf("Peer %d disconnected!\n", connectedPeer[i]);
					closesocket(connectedPeer[i]);
					connectedPeer[i] = connectedPeer[connectedAmount];
					strcpy(connectedName[i], connectedName[connectedAmount]);
				}
				buf[len] = 0;
				printf("%s", buf);
			}
		}

		if (FD_ISSET(listener, &readfds))
		{
			SOCKADDR_IN addr = { 0 };
			int len = sizeof(addr);
			connectedPeer[connectedAmount] = accept(listener, (SOCKADDR FAR*)&addr, &len);
			
			printf("Peer %d connected!\n", connectedPeer[connectedAmount]);
			connectedAmount++;
		}

		if (FD_ISSET(connector, &readfds))
		{
			int len = recv(connector, buf, sizeof(buf), 0);
			int cmd;
			char tmpaddr[MAX_LENGTH];
			char tmpname[MAX_LENGTH];
			unsigned short tmpport;
			sscanf(buf, "%d%s%hu%s", &cmd, tmpaddr, &tmpport, tmpname);
			if (cmd == 1)
			{
				knownPeer[knownAmount].sin_addr.s_addr = inet_addr(tmpaddr);
				knownPeer[knownAmount].sin_port = tmpport;
				knownPeer[knownAmount].sin_family = AF_INET;
				strcpy(peerName[knownAmount], tmpname);
				knownAmount++;
				printf("Ghi nhan mot peer moi\n");
			}
			else
			{
				for (int i = 0; i < knownAmount; i++)
				{
					if (knownPeer[i].sin_addr.s_addr == inet_addr(tmpaddr)
						&& knownPeer[i].sin_port == tmpport)
					{
						knownAmount--;
						knownPeer[i] = knownPeer[knownAmount];
						strcpy(peerName[i], peerName[knownAmount]);
						printf("Mot peer da ngat ket noi\n");
						break;
					}
				}
			}
		}
	}

	WSACleanup();
	return 0;
}

DWORD WINAPI UserCLI(LPVOID lpParam)
{
	char buf[4096];
	char tmp[MAX_LENGTH];
	while (true)
	{
		fgets(buf, 4096, stdin);
		if (strncmp(buf, "FILELIST", strlen("FILELIST")) == 0)
		{
			for (int i = 0; i < fileAmount; i++)
			{
				printf("%d. %s - %s:%d\n", i, fileName[i],
					inet_ntoa(fileLocate[i].sin_addr),
					ntohs(fileLocate[i].sin_port));
			}
			continue;
		}

		if (strncmp(buf, "PEERLIST", strlen("PEERLIST")) == 0)
		{
			for (int i = 0; i < knownAmount; i++)
			{
				printf("%d. %s - %s:%d\n", i, peerName[i],
					inet_ntoa(knownPeer[i].sin_addr),
					ntohs(knownPeer[i].sin_port));
			}
			continue;
		}

		if (strncmp(buf, "PRIVATE", strlen("PRIVATE")) == 0)
		{
			sscanf(buf, "%s%d", tmp, &MODE);
			if (MODE >= knownAmount)
			{
				printf("Chuyen sang che do cong khai\n");
			}
			else
			{
				printf("Chuyen sang che do rieng tu voi peer %d\n", MODE);
			}
		}

		if (strncmp(buf, "DOWNLOAD", strlen("DOWNLOAD")) == 0)
		{
			int fileindex;
			sscanf(buf, "%s%d", tmp, &fileindex);
			SOCKET connector = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			connect(connector, (SOCKADDR FAR*)&fileLocate[fileindex], sizeof(fileLocate[fileindex]));
			char sendBuf[BUFFER_LENGTH];

			sprintf(sendBuf, "DOWNLOAD %s %s", fileName[fileindex], filePass[fileindex]);
			send(connector, buf, strlen(buf), 0);

			sprintf(sendBuf, "%d %s", connector, fileName[fileindex]);
			CreateThread(0, 0, DownloadFile, sendBuf, 0, 0);
			while (sendBuf[0] != 0) {};

			closesocket(connector);
		}

		if (MODE == PUBLIC)
		{
			for (int i = 0; i < knownAmount; i++)
			{
				SOCKET connector = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				connect(connector, (SOCKADDR FAR*)&knownPeer[i], sizeof(knownPeer[i]));
				send(connector, buf, strlen(buf), 0);
				closesocket(connector);
			}
		}
		else
		{
			SOCKET connector = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			connect(connector, (SOCKADDR FAR*)&knownPeer[MODE], sizeof(knownPeer[MODE]));
			send(connector, buf, strlen(buf), 0);
			closesocket(connector);
		}
	}
}

DWORD WINAPI DownloadFile(LPVOID lpParam)
{
	char *tmp = (char *)lpParam;
	SOCKET connector;
	char name[MAX_LENGTH];
	sscanf(tmp, "%d%s", &connector, name);
	tmp[0] = 0;

	FILE *f = fopen(name, "wb");
	fd_set recvfds;
	timeval timelimit = { 10, 0 };
	char data[BUFFER_LENGTH];

	while (true)
	{
		FD_ZERO(&recvfds);
		FD_SET(connector, &recvfds);

		int ret = select(0, &recvfds, NULL, NULL, &timelimit);

		if (ret <= 0)
		{
			break;
		}

		ret = recv(connector, data, sizeof(data), 0);

		for (int i = 0; i < ret; i++)
		{
			fputc(data[i], f);
		}

		if (ret < BUFFER_LENGTH)
		{
			break;
		}
	}

	fclose(f);
	return 0;
}