#include "stdafx.h"

DWORD WINAPI ListenToPeer(LPVOID lpParam)
{
	FD_SET readfds;
	const timeval limit = { 10, 0 };
	int ret;
	OFFPACK recvPack;

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
					RecvPack(it->first, recvPack);

					//Xu ly o day
				}
			}
		}
	}
}

void HandleChat()
{
	
}

void HandleGetFile()
{

}