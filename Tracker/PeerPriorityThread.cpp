#include "stdafx.h"

using namespace std;

DWORD WINAPI PriorityPeerThread(LPVOID lpParam)
{
	NODE tmpNode = ((PEER_THREAD_PARAM FAR*)lpParam)->requestNode;
	tmpNode.sleep = false;
	((PEER_THREAD_PARAM FAR*)lpParam)->pause = false;

	fd_set readfds;
	int ret;
	timeval limit = { 10, 0 };
	OFFLINE_PACKAGE recvPack;

	while (true)
	{
		FD_ZERO(&readfds);
		FD_SET(tmpNode.socket, &readfds);

		ret = select(0, &readfds, NULL, NULL, &limit);

		if (ret == SOCKET_ERROR)
		{
			Disconnect(tmpNode);
			return 0;
		}
		else if (ret == 0)
		{
			tmpNode.sleep = true;
			return 0;
		}
		else
		{
			RecvPack(tmpNode, recvPack);
			/*switch (recvPack.cmdCode)
			{
			case ERROR_CMD:
			{
				Disconnect(tmpNode);
				return 0;
			}
			case REGISTER:
			{
				HandleRegister(tmpNode, recvPack);
				break;
			}
			case LOGIN:
			{
				HandleLogin(tmpNode, recvPack);
				break;
			}
			case UPDATE_INFORATION:
			{
				HandleUpdate(tmpNode, recvPack);
				break;
			}
			case CREATE_FILE:
			{
				HandleCreate(tmpNode, recvPack);
				break;
			}
			case EDIT_FILE:
			{
				HandleEdit(tmpNode, recvPack);
				break;
			}
			case DELETE_FILE:
			{
				HandleDelete(tmpNode, recvPack);
				break;
			}
			case DOWNLOAD:
			{
				HandleDownload(tmpNode, recvPack);
				break;
			}
			}*/
		}
	}
}