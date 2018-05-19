#include "stdafx.h"

DWORD WINAPI ListenToTracker(LPVOID lpParam)
{
	OFFPACK recvPack;
	char *buf;
	int index;

	while (true)
	{
		if (!connected)
		{
			continue;
		}

		RecvPack(connector.toTrackerSocket, recvPack);

		buf = recvPack.data;
		index = 0;

		if (recvPack.cmdCode == POST)
		{
			NotificationControl(&recvPack);
		}
		else
		{
			ResponseNotification(recvPack.data);
		}
	}
	return 0;
}

void NotificationControl(OFFPACK *recvPack)
{
	char *buf = recvPack->data;
	char code;
	int index = 0;
	int offset;
	int ret;

	while (true)
	{
		ret = sscanf(buf + index, "%c%n", &code, &offset);
		if (ret != 2)
		{
			break;
		}
		index += offset;

		if (code == NEW_FILE)
		{
			int fileindex;
			int filesize;
			char filename[256];
			char filechecksum[33];

			ret = sscanf(buf + index, "%d%d%s%s%n", &fileindex, &filesize, filename, filechecksum, &offset);
			if (ret != 4)
			{
				break;
			}
			index += offset;

			FILE_INFO newFile = FILE_INFO();
			newFile.length = filesize;
			strcpy(newFile.name, filename);
			strcpy(newFile.checksum, filechecksum);

			fileList[fileindex] = newFile;
		}

		if (code == EDIT_BLOCK)
		{
			int fileindex;
			int blockindex;
			char md5code[33];

			ret = sscanf(buf + index, "%d%d%32s%n", &fileindex, &blockindex, md5code, &offset);
			if (ret != 4)
			{
				break;
			}
			index += offset;

			if (strlen(md5code) != 32)
			{
				continue;
			}

			if (fileList[fileindex].block.find(blockindex) != fileList[fileindex].block.end())
			{
				strcpy(fileList[fileindex].block[blockindex].checksum, md5code);
			}
			else
			{
				BLOCK newBlock = BLOCK();
				fileList[fileindex].block[blockindex] = newBlock;
				strcpy(fileList[fileindex].block[blockindex].checksum, md5code);
				fileList[fileindex].block[blockindex].numdup;
			}
		}

		if (code == HE_HAS_THIS_BLOCK)
		{
			char straddr[32];
			int port;
			int fileindex;
			int blockindex;
			bool add = true;

			ret = sscanf(buf + index, "%s%d%d%d%n", straddr, &port, &fileindex, &blockindex, &offset);
			if (ret != 5)
			{
				break;
			}
			index += offset;

			for (int i = 0; i < onlinePeerAmount; i++)
			{
				if (strcmp(inet_ntoa(onlinePeer[i].addr.sin_addr), straddr) == 0
					&& onlinePeer[i].addr.sin_port == port)
				{
					for (int k = 0; k < fileList[fileindex].block[blockindex].numdup; k++)
					{
						if (fileList[fileindex].block[blockindex].local[k] == i)
						{
							add = false;
							break;
						}
						if (add == true)
						{
							fileList[fileindex].block[blockindex].local[fileList[fileindex].block[blockindex].numdup] = i;
							fileList[fileindex].block[blockindex].numdup++;
						}
					}
					break;
				}
			}
		}
	
		if (code == HE_DOESNT_HAVE_THIS_BLOCK)
		{
			char straddr[32];
			int port;
			int fileindex;
			int blockindex;

			ret = sscanf(buf + index, "%s%d%d%d%n", straddr, &port, &fileindex, &blockindex, &offset);
			if (ret != 5)
			{
				break;
			}
			index += offset;

			for (int i = 0; i < onlinePeerAmount; i++)
			{
				if (strcmp(inet_ntoa(onlinePeer[i].addr.sin_addr), straddr) == 0
					&& onlinePeer[i].addr.sin_port == port)
				{
					for (int k = 0; k < fileList[fileindex].block[blockindex].numdup; k++)
					{
						if (fileList[fileindex].block[blockindex].local[k] == i)
						{
							fileList[fileindex].block[blockindex].numdup--;
							fileList[fileindex].block[blockindex].local[k] =
								fileList[fileindex].block[blockindex].local[fileList[fileindex].block[blockindex].numdup];
							break;
						}
					}
					break;
				}
			}
		}

		if (code == NEW_PEER)
		{
			int family;
			char addr[32];
			int port;
			unsigned long long int rsaKey[2];
			char username[32];

			ret = sscanf(buf + index, "%d%s%d%llu%llu%s%n", &family, addr, &port, &rsaKey[0], &rsaKey[1], username, &offset);
			if (ret != 7)
			{
				break;
			}
			index += offset;

			NODE newPeer;
			newPeer.addr.sin_family = family;
			newPeer.addr.sin_addr.s_addr = inet_addr(addr);
			newPeer.addr.sin_port = port;
			newPeer.rsaKey[0] = rsaKey[0];
			newPeer.rsaKey[1] = rsaKey[1];
			strncpy(newPeer.username, username, 32) ;
			newPeer.connecting = false;
			onlinePeer[onlinePeerAmount] = newPeer;
			onlinePeerAmount++;
		}

		if (code == PEER_DISCONNECT)
		{
			int family;
			char addr[32];
			int port;
			unsigned long long int rsaKey[2];
			char username[32];

			ret = sscanf(buf + index, "%d%s%d%llu%llu%s%n", &family, addr, &port, &rsaKey[0], &rsaKey[1], username, &offset);
			if (ret != 7)
			{
				break;
			}
			index += offset;

			for (int i = 0; i < onlinePeerAmount; i++)
			{
				if (strcmp(inet_ntoa(onlinePeer[i].addr.sin_addr), addr) == 0
					&& onlinePeer[i].addr.sin_port == port)
				{
					onlinePeerAmount--;
					onlinePeer[i] = onlinePeer[onlinePeerAmount];
					break;
				}
			}
		}
	}
}