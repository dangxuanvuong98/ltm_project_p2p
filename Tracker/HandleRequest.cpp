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
	int ret, offset;
	
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
			
		}

		if (code == I_HAVE_A_BLOCK)
		{

		}

		if (code == HE_DONT_HAVE_THIS_BLOCK)
		{

		}
	}
}

//Xy ly yeu cau cap nhat thong tin (post thong tin tu peer len)
void HandleGet(SOCKET s, OFFPACK recvPack)
{
	char *buf = recvPack.data;
	char flags = buf[0];

	if (flags | GET_ALL)
	{

	}
	
	if (flags | GET_ONLINE_PEER)
	{

	}

	if (flags | GET_FILE_LIST)
	{

	}

	if (flags | GET_FILE_INFORMATION)
	{

	}
}