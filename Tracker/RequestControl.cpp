#include "stdafx.h"

DWORD WINAPI WaitForRequest(LPVOID lpParam)
{
	int ret;

	fd_set readfds;
	timeval timeout = { 1, 0 };
	CONNECTION::iterator it;

	THREAD_PARAM newRequest;

	while (true)
	{
		FD_ZERO(&readfds);

		EnterCriticalSection(&criticalSection);
		for (it = onlinePeer.begin(); it != onlinePeer.end(); it++)
		{
			if (it->second.connecting == false)
			{
				FD_SET(it->first, &readfds);
			}
		}
		LeaveCriticalSection(&criticalSection);

		ret = select(0, &readfds, NULL, NULL, NULL);

		for (it = onlinePeer.begin(); it != onlinePeer.end(); it++)
		{
			if (FD_ISSET(it->first, &readfds))
			{
				newRequest.pause = true;
				newRequest.s = it->first;
				CreateThread(0, 0, RequestControl, &newRequest, 0, 0);
				while (newRequest.pause);
			}
		}
	}
}

DWORD WINAPI RequestControl(LPVOID lpParam)
{
	SOCKET s = ((THREAD_PARAM FAR*)lpParam)->s;
	((THREAD_PARAM FAR*)lpParam)->pause = false;

	OFFPACK newPack;
	RecvPack(s, newPack);

	switch (newPack.cmdCode)
	{
	case REGISTER:
		{
			HandleRegister(s, newPack);
			break;
		}
	case LOGIN:
		{
			HandleLogin(s, newPack);
			break;
		}
	case GET:
		{
			HandleGet(s, newPack);
			break;
		}
	case POST:
		{
			HandlePost(s, newPack);
			break;
		}
	}

	return 0;
}