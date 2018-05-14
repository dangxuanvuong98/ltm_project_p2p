#include "stdafx.h"

void InitTracker()
{
	UpdateKey();

	trackerLog = fopen("trackerlog.dat", "a");

	//Khoi tao socket
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	myNode.socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	myNode.addr.sin_addr.s_addr = htonl(INADDR_ANY);
	myNode.addr.sin_port = htons(TRACKER_DEFAULT_PORT);
	myNode.addr.sin_family = AF_INET;

	bind(myNode.socket, (SOCKADDR FAR*)&myNode.addr, sizeof(myNode.addr));

	onlinePeerAmount = 0;
	
	listen(myNode.socket, 1024);
	fprintf(trackerLog, "Server da san sang\n");
	printf("Server da san sang\n");
}