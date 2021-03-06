// Tracker.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int main()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(9000);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	bind(listener, (SOCKADDR FAR*)&addr, sizeof(addr));
	listen(listener, 5);

	SOCKET peerSock[1024];
	SOCKADDR_IN peerAddr[1024];
	char peerName[1024][256];
	int amount = 0;

	int addrlen = sizeof(SOCKADDR);

	fd_set readfds;
	timeval timelimit = { 1, 0 };

	char recvBuf[4096], sendBuf[4096];

	while (true)
	{
		FD_ZERO(&readfds);
		FD_SET(listener, &readfds);
		for (int i = 0; i < amount; i++)
		{
			FD_SET(peerSock[i], &readfds);
		}
		int ret = select(0, &readfds, NULL, NULL, &timelimit);
		if (ret < 0)
		{
			printf("Error: %d\n", GetLastError());
		}
		if (ret == 0)
		{
			continue;
			printf("Timeout!\n");
		}
		if (ret > 0)
		{
			if (FD_ISSET(listener, &readfds))
			{
				peerAddr[amount] = { 0 };
				peerSock[amount] = accept(listener, (SOCKADDR FAR*)&peerAddr[amount], &addrlen);
				amount++;
			}

			for (int i = 0; i < amount; i++)
			{
				if (FD_ISSET(peerSock[i], &readfds))
				{
					int len = recv(peerSock[i], recvBuf, sizeof(recvBuf), 0);
					if (len < 0)
					{
						printf("Peer %s:%hu disconnected!\n", inet_ntoa(peerAddr[i].sin_addr), htons(peerAddr[i].sin_port));
						for (int k = 0; k < amount; k++)
						{
							if (k != i)
							{
								sprintf(sendBuf, "-1 %s %hu %s\n", inet_ntoa(peerAddr[i].sin_addr), peerAddr[i].sin_port, peerName[i]);
								send(peerSock[k], sendBuf, strlen(sendBuf), 0);
							}
						}

						amount--;
						closesocket(peerSock[i]);
						peerSock[i] = peerSock[amount];
						continue;
					}

					if (strncmp(recvBuf, "UPDATEPEERLIST", strlen("UPDATEPEERLIST")) == 0)
					{
						for (int k = 0; k < amount; k++)
						{
							if (k != i)
							{
								sprintf(sendBuf, "1 %s %hu %s\n", inet_ntoa(peerAddr[k].sin_addr), peerAddr[k].sin_port, peerName[k]);
								send(peerSock[i], sendBuf, strlen(sendBuf), 0);
							}
						}
						continue;
					}

					unsigned short tmpport;
					char tmpname[256];
					if (sscanf(recvBuf, "%hu%s", &tmpport, tmpname) != 2)
					{
						continue;
					}
					else
					{
						peerAddr[i].sin_port = tmpport;
						strcpy(peerName[i], tmpname);
						for (int k = 0; k < amount; k++)
						{
							if (k != i)
							{
								sprintf(sendBuf, "1 %s %hu %s\n", inet_ntoa(peerAddr[i].sin_addr), peerAddr[i].sin_port, peerName[i]);
								send(peerSock[k], sendBuf, strlen(sendBuf), 0);
								sprintf(sendBuf, "1 %s %hu %s\n", inet_ntoa(peerAddr[k].sin_addr), peerAddr[k].sin_port, peerName[k]);
								send(peerSock[i], sendBuf, strlen(sendBuf), 0);
							}
						}
						sprintf(sendBuf, "0 %s %hu %s\n", inet_ntoa(peerAddr[i].sin_addr), peerAddr[i].sin_port, peerName[i]);
						send(peerSock[i], sendBuf, strlen(sendBuf), 0);

						printf("Peer %s:%hu connected!\n", inet_ntoa(peerAddr[i].sin_addr), htons(peerAddr[i].sin_port));
					}
				}
			}
		}
	}

	WSACleanup();
	return 0;
}