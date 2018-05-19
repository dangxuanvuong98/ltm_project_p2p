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
			if (ret == 1)
			{
				continue;
			}
			if (ret == 2)
			{
				continue;
			}
		}
		case REGISTER:
		{

		}
		case LOGIN:
		{

		}
		case GET:
		{

		}
		case POST:
		{

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
		cout << buf << endl;
	}
	else
	{
		responseBuffer.push_back(buf);
	}
}