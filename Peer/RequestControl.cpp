#include "stdafx.h"

DWORD WINAPI ListenToPeer(LPVOID lpParam)
{
	FD_SET readfds;
	const timeval limit = { 10, 0 };
	int ret;
	THREAD_PARAM newRequest;

	while (true)
	{
		FD_ZERO(&readfds);

		for (CONNECTION::iterator it = onlinePeer.begin(); it != onlinePeer.end(); it++)
		{
			FD_SET(it->first, &readfds);
		}

		ret = select(0, &readfds, NULL, NULL, &limit);

		if (ret != 0)
		{
			for (CONNECTION::iterator it = onlinePeer.begin(); it != onlinePeer.end(); it++)
			{
				if (FD_ISSET(it->first, &readfds))
				{
					newRequest.s = it->first;
					newRequest.pause = true;
					CreateThread(0, 0, RequestControl, &newRequest, 0, 0);
					while (newRequest.pause);
				}
			}
		}
	}
}

DWORD WINAPI RequestControl(LPVOID lpParam)
{
	SOCKET s = ((THREAD_PARAM *)lpParam)->s;
	((THREAD_PARAM *)lpParam)->pause = false;

	OFFPACK recvPack;
	RecvPack(s, recvPack);

	if (recvPack.cmdCode == CHAT_REQUEST)
	{
		HandleChat(recvPack.data);
	}
	else if (recvPack.cmdCode == GET_FILE_REQUEST)
	{
		HandleGetFile(recvPack.data);
	}

	return 0;
}