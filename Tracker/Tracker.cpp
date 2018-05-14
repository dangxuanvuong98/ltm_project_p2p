#include "stdafx.h"

using namespace std;

int main()
{
	InitializeCriticalSection(&criticalSection);

	InitTracker();
	CreateThread(0, 0, SleepPeerThread, NULL, 0, 0);

	PEER_THREAD_PARAM peerThreadParam;
	NODE tmpNode;
	int len = sizeof(tmpNode.addr);

	while (true)
	{
		tmpNode.addr = { 0 };
		tmpNode.socket = accept(myNode.socket, (SOCKADDR FAR*)&(tmpNode.addr), &len);

		EnterCriticalSection(&criticalSection);
		if (onlinePeerAmount < MAX_PEER)
		{
			printf("Peer %s:%d da ket noi\n", inet_ntoa(tmpNode.addr.sin_addr), ntohs(tmpNode.addr.sin_port));
			fprintf(trackerLog, "Peer %s:%d da ket noi\n", inet_ntoa(tmpNode.addr.sin_addr), ntohs(tmpNode.addr.sin_port));
			
			peerThreadParam = PEER_THREAD_PARAM();
			
			InitConnetion(tmpNode);
			peerThreadParam.requestNode = tmpNode;
			
			CreateThread(0, 0, PriorityPeerThread, &peerThreadParam, 0, 0);
			while (peerThreadParam.pause);

			onlinePeerAmount++;
		}
		else
		{
			Disconnect(tmpNode);
		}
		LeaveCriticalSection(&criticalSection);
	}

    return 0;
}