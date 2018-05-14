#include "stdafx.h"

DWORD WINAPI SleepPeerThread(LPVOID lpParam)
{
	int ret;

	fd_set readfds;
	timeval timeout = { 10, 0 };
	std::set<NODE>::iterator it;

	PEER_THREAD_PARAM peerThreadParam;

	while (true)
	{
		FD_ZERO(&readfds);

		EnterCriticalSection(&criticalSection);
		for (it = onlinePeer.begin(); it != onlinePeer.end(); it++)
		{
			if (it->sleep == true)
			{
				FD_SET(it->socket, &readfds);
			}
		}
		LeaveCriticalSection(&criticalSection);

		ret = select(0, &readfds, NULL, NULL, &timeout);

		for (it = onlinePeer.begin(); it != onlinePeer.end(); it++)
		{
			if (FD_ISSET(it->socket, &readfds))
			{
				peerThreadParam = PEER_THREAD_PARAM();

				peerThreadParam.requestNode = *it;
				peerThreadParam.requestNode.sleep = false;

				onlinePeer.erase(peerThreadParam.requestNode);
				onlinePeer.insert(peerThreadParam.requestNode);

				CreateThread(0, 0, PriorityPeerThread, &peerThreadParam, 0, 0);
				while (peerThreadParam.pause);
			}
		}
	}
}