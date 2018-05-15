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