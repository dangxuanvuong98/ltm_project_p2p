#include "stdafx.h"

DWORD WINAPI WaitForRequest(LPVOID lpParam)
{
	int ret;

	fd_set readfds;
	timeval timeout = { 10, 0 };
	CONNECTION::iterator it;

	while (true)
	{
		FD_ZERO(&readfds);

		EnterCriticalSection(&criticalSection);
		for (it = onlinePeer.begin(); it != onlinePeer.end(); it++)
		{
			FD_SET(it->first, &readfds);
		}
		LeaveCriticalSection(&criticalSection);

		ret = select(0, &readfds, NULL, NULL, &timeout);

		for (it = onlinePeer.begin(); it != onlinePeer.end(); it++)
		{
			if (FD_ISSET(it->first, &readfds))
			{
			}
		}
	}
}