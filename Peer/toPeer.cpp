#include "stdafx.h"

void GoOnline()
{
	UpdateKey();

	char tmpAddr[256];

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	while (true)
	{
		system("cls");
		printf("Nhap dia chi tracker: ");
		gets_s(tmpAddr);

		myNode.addr.sin_family = AF_INET;
		myNode.addr.sin_port = 0;
		myNode.addr.sin_addr.s_addr = htonl(INADDR_ANY);
		myNode.socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		bind(myNode.socket, (SOCKADDR FAR*)&myNode.addr, sizeof(myNode.addr));

		toTracker.addr.sin_family = AF_INET;
		toTracker.addr.sin_port = htons(9000);
		toTracker.addr.sin_addr.s_addr = inet_addr(tmpAddr);
		toTracker.socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		int ret = connect(toTracker.socket, (SOCKADDR FAR*)&(toTracker.addr), sizeof(toTracker.addr));
		if (ret == 0)
		{
			InitConnectionToTracker();
			printf("Ket noi thanh cong den tracker %s:9000\n", tmpAddr);
			fprintf(peerLog, "Ket noi thanh cong den tracker %s:9000\n", tmpAddr);
			system("pause");
			break;
		}
		else
		{
			printf("Ket noi that bai!\n");
			fprintf(peerLog, "Ket noi that bai!\n");
			system("pause");
		}
	}
}