#include "stdafx.h"

void UserInterface()
{
	IC = REQUEST_INTERFACE;
	
	while (true)
	{
		switch (IC)
		{
		case REQUEST_INTERFACE:
		{
			IC = RequestInterface();
			break;
		}
		case CHAT_INTERFACE:
		{
			IC = ChatInterface();
			break;
		}
		case LISTENER_INTERFACE:
		{
			IC = ListenerInterface();
			break;
		}
		default:
		{
			continue;
		}
		}
	}
}

int RequestInterface()
{
	char buf[4096];
	char cmd[32];
	int offset;
	int index;
	while (true)
	{
		printf(">>");
		fgets(buf, 4096, stdin);
		index = 0;
		sscanf(buf + index, "%s%n", cmd, &offset);
		index += offset;
		
		switch (cmdtrans(cmd))
		{
		case CONNECT:
		{
			char addr_a[32];
			int port;
			int ret = sscanf(buf + index, "%32s%d", addr_a, &port);
			if (ret != 1 && ret != 2)
			{
				printf("Tham so khong hop le!\n");
				break;
			}

			if (ret == 1)
			{
				port = 9000;
			}

			trackerNode.addr.sin_addr.s_addr = inet_addr(addr_a);
			trackerNode.addr.sin_family = AF_INET;
			trackerNode.addr.sin_port = htons(port);

			ret = connect(connector.toTrackerSocket, (SOCKADDR FAR*)&trackerNode.addr, sizeof(trackerNode.addr));

			if (ret == 0)
			{
				connected = true;
				printf("Ket noi thanh cong!\n");
				CreateThread(0, 0, SetupConnection, NULL, 0, 0);
			}
			else
			{
				printf("Ket noi that bai\n");
			}
			break;
		}
		case REGISTER:
		{
			char user[32], pass[32];
			int ret = sscanf(buf + index, "%31s%31s", user, pass);
			if (ret != 2)
			{
				printf("Nhap thieu tham so!\n");
			}
			OFFPACK sendPack;
			sendPack.cmdCode = REGISTER;
			sprintf(sendPack.data, "%s %s", user, pass);
			SendPack(connector.toTrackerSocket, sendPack, strlen(sendPack.data));
			SendPack(connector.toTrackerSocket, sendPack, strlen(sendPack.data));
			break;
		}
		case LOGIN:
		{
			char user[32], pass[32];
			int ret = sscanf(buf + index, "%31s%31s", user, pass);
			if (ret != 2)
			{
				printf("Nhap thieu tham so!\n");
			}
			OFFPACK sendPack;
			sendPack.cmdCode = LOGIN;
			sprintf(sendPack.data, "%s %s", user, pass);
			SendPack(connector.toTrackerSocket, sendPack, strlen(sendPack.data));
			SendPack(connector.toTrackerSocket, sendPack, strlen(sendPack.data));
			break;
		}
		case GET:
		{
			break;
		}
		case POST:
		{
			break;
		}
		default:
		{
			printf("Khong ton tai cau lenh\n");
		}
		}
	}

	return REQUEST_INTERFACE;
}

int ChatInterface()
{
	
	return REQUEST_INTERFACE;
}

int ListenerInterface()
{

	return REQUEST_INTERFACE;
}

int cmdtrans(char *buf)
{
	if (strcmp(buf, "connect") == 0)
	{
		return CONNECT;
	}

	if (strcmp(buf, "disconnect") == 0)
	{
		return DISCONNECT;
	}

	if (strcmp(buf, "register") == 0)
	{
		return REGISTER;
	}

	if (strcmp(buf, "login") == 0)
	{
		return LOGIN;
	}

	if (strcmp(buf, "get") == 0)
	{
		return GET;
	}

	if (strcmp(buf, "post") == 0)
	{
		return POST;
	}

	if (strcmp(buf, "chat") == 0)
	{
		return CHAT;
	}

	if (strcmp(buf, "logview") == 0)
	{
		return LOG;
	}

	if (strcmp(buf, "cmd") == 0)
	{
		return CMD;
	}

	return ERROR_CMD;
}

void ResponseNotification(char *buf)
{
	if (IC == REQUEST_INTERFACE)
	{
		printf("%s", buf);
	}
	else
	{
		responseBuffer.push_back(buf);
	}
}