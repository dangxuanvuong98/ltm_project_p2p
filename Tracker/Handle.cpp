#include "stdafx.h"

/*void HandleRegister(NODE reqNode, OFFLINE_PACKAGE recvPack)
{
	char tmpUser[40], tmpPassword[40];
	strncpy(tmpUser, recvPack.data, 32);
	strncpy(tmpPassword, recvPack.data + 32, 32);

	if (userInformation.find(tmpUser) == userInformation.end())
	{
		userInformation[tmpUser] = tmpPassword;
		SendPack(reqNode, RESPONSE_ACCEPT, NULL, 0);
	}
	else
	{
		SendPack(reqNode, RESPONSE_DENY, NULL, 0);
	}
}

void HandleLogin(NODE reqNode, OFFLINE_PACKAGE recvPack)
{
	char tmpUser[40], tmpPassword[40];
	strncpy(tmpUser, recvPack.data, 32);
	strncpy(tmpPassword, recvPack.data + 32, 32);

	if (userInformation[tmpUser] == tmpPassword)
	{
		onlinePeer.erase(reqNode);
		reqNode.login = true;
		onlinePeer.insert(reqNode);
		SendPack(reqNode, RESPONSE_ACCEPT, NULL, 0);
	}
	else
	{
		SendPack(reqNode, RESPONSE_DENY, NULL, 0);
	}
}

void HandleGetFileList(NODE reqNode, OFFLINE_PACKAGE recvPack)
{
	if (!(reqNode.login))
	{
		SendPack(reqNode, RESPONSE_DENY, NULL, 0);
		return;
	}
}

void HandleGetPeerList(NODE reqNode, OFFLINE_PACKAGE recvPack)
{
	if (!(reqNode.login))
	{
		SendPack(reqNode, RESPONSE_DENY, NULL, 0);
		return;
	}
}

void HandleUpload(NODE reqNode, OFFLINE_PACKAGE recvPack)
{
	if (!(reqNode.login))
	{
		SendPack(reqNode, RESPONSE_DENY, NULL, 0);
		return;
	}
	char fileName[64], fileMD5[32];
	strncpy(fileName, recvPack.data, 64);
	strncpy(fileMD5, recvPack.data + 64, 32);
	if (fileInformation.find(fileName) != fileInformation.end())
	{
		SendPack(reqNode, RESPONSE_DENY, NULL, 0);
	}
	else
	{
		fileInformation[fileName] = fileMD5;
		SendPack(reqNode, RESPONSE_ACCEPT, NULL, 0);
	}
}

void HandleChat(NODE reqNode, OFFLINE_PACKAGE recvPack)
{
	if (!(reqNode.login))
	{
		SendPack(reqNode, RESPONSE_DENY, NULL, 0);
		return;
	}
}

void HandleDownload(NODE reqNode, OFFLINE_PACKAGE recvPack)
{
	if (!(reqNode.login))
	{
		SendPack(reqNode, RESPONSE_DENY, NULL, 0);
		return;
	}
}*/