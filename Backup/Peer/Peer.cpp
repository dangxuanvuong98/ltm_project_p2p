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
DWORD WINAPI ResponseDownload(LPVOID lpParam);

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
SOCKADDR_IN myAddr;
unsigned short myPort;

char myFileName[MAX_NUMBER][MAX_LENGTH];
char myFilePass[MAX_NUMBER][MAX_LENGTH];
char myFileAmount = 0;

SOCKET connectToTracker;

int main()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	unsigned short tmpport = MIN_PEER_PORT;

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
	listen(listener, 5);
	myPort = htons(tmpport);

	addr.sin_addr.s_addr = inet_addr(TRACKER_ADDR);
	addr.sin_port = htons(TRACKER_PORT);
	SOCKET &connector = connectToTracker;
	connector = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	printf("Nhap username: ");
	fgets(user, MAX_LENGTH - 1, stdin);
	if (user[strlen(user) - 1] == '\n')
	{
		user[strlen(user) - 1] = 0;
	}

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

	timeval timelimit = { 1, 0 };

	while (true)
	{
		FD_ZERO(&readfds);
		FD_SET(listener, &readfds);
		FD_SET(connector, &readfds);
		for (int i = 0; i < connectedAmount; i++)
		{
			FD_SET(connectedPeer[i], &readfds);
		}

		int ret = select(0, &readfds, NULL, NULL, &timelimit);

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
		while (ret > 0)
		{
			if (FD_ISSET(listener, &readfds))
			{
				ret--;
				SOCKADDR_IN addr = { 0 };
				int len = sizeof(addr);
				connectedPeer[connectedAmount] = accept(listener, (SOCKADDR FAR*)&addr, &len);

				printf("Peer %d connected!\n", connectedPeer[connectedAmount]);
				connectedAmount++;
			}

			if (FD_ISSET(connector, &readfds))
			{
				ret--;
				int len = recv(connector, buf, sizeof(buf), 0);
				if (len <= 0)
				{
					return -1;
				}
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
				else if (cmd == -1)
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
				else
				{
					myAddr.sin_family = AF_INET;
					myAddr.sin_addr.s_addr = inet_addr(tmpaddr);
					myAddr.sin_port = myPort;
					//strcpy(user, tmpname);
				}
			}

			for (int i = 0; i < connectedAmount; i++)
			{
				if (FD_ISSET(connectedPeer[i], &readfds))
				{
					ret--;
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

					char tmp[MAX_LENGTH], cmd[MAX_LENGTH], straddr[MAX_LENGTH];
					char filename[MAX_LENGTH], filepass[MAX_LENGTH];
					unsigned short port;

					int ret = sscanf(buf, "%s%s%s", cmd, filename, filepass);
					if (ret == 3 && strncmp(buf, "DOWNLOAD", strlen("DOWNLOAD")) == 0)
					{
						char param[BUFFER_LENGTH];
						sprintf(param, "%d %s %s", connectedPeer[i], filename, filepass);
						CreateThread(0, 0, ResponseDownload, param, 0, 0);
						while (buf[0] != 0) {};
						continue;
					}

					ret = sscanf(buf, "%s%s%s%s%s%hu", tmp, cmd, filename, filepass, straddr, &port);

					if (ret == 6 && strncmp(cmd, "UPLOAD", strlen("UPLOAD")) == 0)
					{
						strcpy(fileName[fileAmount], filename);
						strcpy(filePass[fileAmount], filepass);
						fileLocate[fileAmount].sin_family = AF_INET;
						fileLocate[fileAmount].sin_addr.s_addr = inet_addr(straddr);
						fileLocate[fileAmount].sin_port = port;
						printf("%s:%hu upload file %s\n", straddr, ntohs(port), filename);
						fileAmount++;
					}
					else
					{
						printf("%s", buf);
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
	char buf[BUFFER_LENGTH];
	char tmp[MAX_LENGTH];
	while (true)
	{
		printf(">>");
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

		if (strncmp(buf, "UPDATEPEERLIST", strlen("UPDATEPEERLIST")) == 0)
		{
			knownAmount = 0;
			send(connectToTracker, buf, strlen(buf), 0);
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
			continue;
		}

		if (strncmp(buf, "DOWNLOAD", strlen("DOWNLOAD")) == 0)
		{
			CreateThread(0, 0, DownloadFile, buf, 0, 0);
			while (true)
			{
				if (buf[0] == 0)
				{
					break;
				}
			}

			continue;
		}

		char sendBuf[BUFFER_LENGTH];

		if (strncmp(buf, "UPLOAD", strlen("UPLOAD")) == 0)
		{
			sscanf(buf, "%s%s%s", tmp, myFileName[myFileAmount], myFilePass[myFileAmount]);
			myFileAmount++;
			sprintf(sendBuf, "%s: %s %s %hu\n", user, buf, inet_ntoa(myAddr.sin_addr), myAddr.sin_port);
		}
		else
		{
			/*if (buf[0] = "\'")
			{
				strcpy(buf, buf + 1);
			}*/

			sprintf(sendBuf, "%s: %s", user, buf);
		}

		if (MODE == PUBLIC)
		{
			for (int i = 0; i < knownAmount; i++)
			{
				SOCKET connector = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				if (connect(connector, (SOCKADDR FAR*)&knownPeer[i], sizeof(knownPeer[i])))
				{
					printf("Can't connect! %d\n", GetLastError());
				}
				
				send(connector, sendBuf, strlen(sendBuf), 0);
				closesocket(connector);
			}
		}
		else
		{
			SOCKET connector = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			connect(connector, (SOCKADDR FAR*)&knownPeer[MODE], sizeof(knownPeer[MODE]));
			send(connector, sendBuf, strlen(sendBuf), 0);
			closesocket(connector);
		}
	}
}

DWORD WINAPI DownloadFile(LPVOID lpParam)
{
	char *buf = (char *)lpParam;	
	int fileindex;
	char tmp[MAX_LENGTH];
	sscanf(buf, "%s%d", tmp, &fileindex);
	buf[0] = 0;


	SOCKET connector = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	connect(connector, (SOCKADDR FAR*)&fileLocate[fileindex], sizeof(fileLocate[fileindex]));
	char sendBuf[BUFFER_LENGTH];

	sprintf(sendBuf, "DOWNLOAD %s %s", fileName[fileindex], filePass[fileindex]);
	send(connector, sendBuf, strlen(sendBuf), 0);

	sprintf(sendBuf, "%d %s", connector, fileName[fileindex]);

	FILE *f = fopen(fileName[fileindex], "wb");
	fd_set recvfds;
	timeval timelimit = { 5, 0 };
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

		printf("Received a package from %d\n", connector);

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

	printf("Complete Downloading!\n");
	closesocket(connector);
	fclose(f);
	return 0;
}

DWORD WINAPI ResponseDownload(LPVOID lpParam)
{
	char *buf = (char *)lpParam;
	int peer;
	char filename[MAX_LENGTH], filepass[MAX_LENGTH];
	sscanf(buf, "%d%s%s", &peer, filename, filepass);
	buf[0] = 0;

	bool find = false;

	for (int i = 0; i < myFileAmount; i++)
	{
		if (strcmp(myFileName[i], filename) == 0 && strcmp(myFilePass[i], filepass) == 0)
		{
			find = true;
			break;
		}
	}

	if (!find)
	{
		return -1;
	}

	FILE *f = fopen(filename, "rb");
	int ret;
	char data[BUFFER_LENGTH];
	
	while ((ret = fread(data, 1, BUFFER_LENGTH, f)) > 0)
	{
		printf("Send a package to %d\n", peer);
		send(peer, data, ret, 0);
		if (ret < BUFFER_LENGTH)
		{
			printf("Complete sending file to %d\n", peer);
			//closesocket(peer);
			break;
		}
	}

	fclose(f);
	return 0;
}