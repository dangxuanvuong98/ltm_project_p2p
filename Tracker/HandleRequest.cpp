#include "stdafx.h"

//Xu ly yeu cau dang ky tai khoan
void HandleRegister(SOCKET s, OFFPACK recvPack)
{
	OFFPACK response;
	char user[32], pass[32];
	char *buf = recvPack.data;
	sscanf(buf,"%s%s", user, pass);
	if (userList.find(user) != userList.end())
	{
		userList[user] = pass;
		response.cmdCode = REGISTER_SUCCESS;
		sprintf(response.data, "Dang ky thanh cong");
		printf("Tai khoan %s da duoc tao\n", user);
	}
	else
	{
		response.cmdCode = REGISTER_FAIL;
		sprintf(response.data, "Ten tai khoan da duoc dang ky");
	}

	SendPack(s, response, strlen(response.data));
}

//Xy ly yeu cau dang nhap
void HandleLogin(SOCKET s, OFFPACK recvPack)
{
	OFFPACK response;
	char user[32], pass[32];
	char *buf = recvPack.data;
	sscanf(buf, "%s%s", user, pass);
	if (userList.find(user) == userList.end())
	{
		userList[user] = pass;
		response.cmdCode = LOGIN_FAIL;
		sprintf(response.data, "Tai khoan khong ton tai");
	}
	else
	{
		if (userList[user] == pass)
		{
			response.cmdCode = LOGIN;
			sprintf(response.data, "Dang nhap thanh cong");
			printf("Tai khoan %s da dang nhap\n", user);
		}
		else
		{
			response.cmdCode = LOGIN_FAIL;
			sprintf(response.data, "Mat khau khong chinh xac");
		}
	}

	SendPack(s, response, strlen(response.data));
}

//Xu ly yeu cap nhat thong tin (get thong tin tu tracker xuong)
void HandlePost(SOCKET s, OFFPACK recvPack)
{
	char *buf = recvPack.data;
	int index = 0;
	char code;
	int ret;
	int offset;
	
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
			char filename[256], md5code[33];
			int filesize;
			ret = sscanf(buf + index, "%d%32s%255s%n", &filesize, md5code, filename, &offset);
			if (ret != 4)
			{
				break;
			}
			index += offset;

			if (strlen(md5code) != 32)
			{
				continue;
			}
			
			FILE_INFO newFile = FILE_INFO();

			fileList[fileAmount] = newFile;
			fileList[fileAmount].length = filesize;
			strcpy(fileList[fileAmount].name, filename);
			strcpy(fileList[fileAmount].checksum, md5code);
			fileAmount++;

			OFFPACK newPack;
			newPack.cmdCode = POST;
			sprintf(newPack.data, "%c %d %d %s %s", NEW_FILE, fileAmount-1, filesize, md5code, filename);

			for (CONNECTION::iterator it = onlinePeer.begin(); it != onlinePeer.end(); it++)
			{
				SendPack(it->first, newPack, strlen(newPack.data));
			}
		}

		if (code == EDIT_BLOCK)
		{
			int fileindex, blockindex;
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
				fileList[fileindex].block[blockindex].numdup = 1;
				fileList[fileindex].block[blockindex].local[0] = s;
			}

			OFFPACK newPack;
			newPack.cmdCode = POST;
			sprintf(newPack.data, "%c%d%d%s", EDIT_BLOCK, fileindex, blockindex, md5code);

			for (CONNECTION::iterator it = onlinePeer.begin(); it != onlinePeer.end(); it++)
			{
				SendPack(it->first, newPack, strlen(newPack.data));
			}

			sprintf(newPack.data, "%c%d,%d%d", HE_HAS_THIS_BLOCK, s, fileindex, blockindex);

			for (CONNECTION::iterator it = onlinePeer.begin(); it != onlinePeer.end(); it++)
			{
				SendPack(it->first, newPack, strlen(newPack.data));
			}
			
		}

		if (code == I_HAVE_A_BLOCK)
		{
			int fileindex, blockindex;
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

			if (strncmp(md5code, fileList[fileindex].block[blockindex].checksum, 32) == 0)
			{
				bool add = true;
				int &numdup = fileList[fileindex].block[blockindex].numdup;
				for (int i = 0; i < numdup; i++)
				{
					if (fileList[fileindex].block[blockindex].local[i] == s)
					{
						add = false;
						break;
					}
				}
				if (!add)
				{
					continue;
				}
				fileList[fileindex].block[blockindex].local[numdup] = s;
				numdup++;
			}

			OFFPACK newPack;
			newPack.cmdCode = POST;
			sprintf(newPack.data, "%c%s%d%d%d", HE_HAS_THIS_BLOCK, inet_ntoa(onlinePeer[s].addr.sin_addr),
				onlinePeer[s].addr.sin_port,
				fileindex, blockindex);

			for (CONNECTION::iterator it = onlinePeer.begin(); it != onlinePeer.end(); it++)
			{
				SendPack(it->first, newPack, strlen(newPack.data));
			}
		}

		if (code == HE_DOESNT_HAVE_THIS_BLOCK)
		{
			int fileindex, blockindex;

			ret = sscanf(buf + index, "%d%d%n", &fileindex, &blockindex, &offset);
			if (ret != 3)
			{
				break;
			}
			index += offset;

			BLOCK &tmpBlock = fileList[fileindex].block[blockindex];
			for (int i = 0; i < tmpBlock.numdup; i++)
			{
				if (tmpBlock.local[i] == s)
				{
					tmpBlock.numdup--;
					tmpBlock.local[i] = tmpBlock.local[tmpBlock.numdup];
					break;
				}
			}

			OFFPACK newPack;
			newPack.cmdCode = POST;
			sprintf(newPack.data, "%c%s%d%d%d", HE_DOESNT_HAVE_THIS_BLOCK, inet_ntoa(onlinePeer[s].addr.sin_addr),
				onlinePeer[s].addr.sin_port,
				fileindex, blockindex);

			for (CONNECTION::iterator it = onlinePeer.begin(); it != onlinePeer.end(); it++)
			{
				SendPack(it->first, newPack, strlen(newPack.data));
			}
		}
	}
}

//Xy ly yeu cau cap nhat thong tin (post thong tin tu peer len)
void HandleGet(SOCKET s, OFFPACK recvPack)
{
	char *buf = recvPack.data;
	char flags = buf[0];
	char tmpbuf[4096];
	int index = 0;
	int fileindex;
	
	OFFPACK tmpPack;
	tmpPack.cmdCode = POST;

	if (flags | GET_ALL)
	{
		for (CONNECTION::iterator it = onlinePeer.begin(); it != onlinePeer.end(); it++)
		{
			sprintf(tmpbuf, "%c%s%d", ONLINE_PEER, inet_ntoa((it->second).addr.sin_addr), (it->second).addr.sin_port);
			if (strlen(tmpbuf) + index >= 4096)
			{
				SendPack(s, tmpPack, index);
				index = 0;
			}
			memcpy(tmpPack.data + index, tmpbuf, strlen(tmpbuf));
		}

		for (std::map<int, FILE_INFO>::iterator it = fileList.begin(); it != fileList.end(); it++)
		{
			sprintf(tmpbuf, "%c%d%s%s", FILE_NAME, (it->second).length, (it->second).checksum, (it->second).name);
			if (strlen(tmpbuf) + index >= 4096)
			{
				SendPack(s, tmpPack, index);
				index = 0;
			}
			memcpy(tmpPack.data + index, tmpbuf, strlen(tmpbuf));

			for (unsigned long int i = 0; i < fileList[fileindex].length; i++)
			{
				sprintf(tmpbuf, "%c%s", EDIT_BLOCK, fileList[fileindex].block[i].checksum);
				if (strlen(tmpbuf) + index >= 4096)
				{
					SendPack(s, tmpPack, index);
					index = 0;
				}
				memcpy(tmpPack.data + index, tmpbuf, strlen(tmpbuf));
			}
		}
	}
	
	if (flags | GET_ONLINE_PEER)
	{
		for (CONNECTION::iterator it = onlinePeer.begin(); it != onlinePeer.end(); it++)
		{
			sprintf(tmpbuf, "%c%s%d", ONLINE_PEER, inet_ntoa((it->second).addr.sin_addr), (it->second).addr.sin_port);
			if (strlen(tmpbuf) + index >= 4096)
			{
				SendPack(s, tmpPack, index);
				index = 0;
			}
			memcpy(tmpPack.data + index, tmpbuf, strlen(tmpbuf));
		}
	}

	if (flags | GET_FILE_LIST)
	{
		for (std::map<int, FILE_INFO>::iterator it = fileList.begin(); it != fileList.end(); it++)
		{
			sprintf(tmpbuf, "%c%d%s%s", FILE_NAME, (it->second).length, (it->second).checksum, (it->second).name);
			if (strlen(tmpbuf) + index >= 4096)
			{
				SendPack(s, tmpPack, index);
				index = 0;
			}
			memcpy(tmpPack.data + index, tmpbuf, strlen(tmpbuf));
		}
	}

	if (flags | GET_FILE_INFORMATION)
	{
		sscanf(buf + 1, "%d", &fileindex);
		for (unsigned long int i = 0; i < fileList[fileindex].length; i++)
		{
			sprintf(tmpbuf, "%c%s", EDIT_BLOCK, fileList[fileindex].block[i].checksum);
			if (strlen(tmpbuf) + index >= 4096)
			{
				SendPack(s, tmpPack, index);
				index = 0;
			}
			memcpy(tmpPack.data + index, tmpbuf, strlen(tmpbuf));
		}
	}

	SendPack(s, tmpPack, index);
}