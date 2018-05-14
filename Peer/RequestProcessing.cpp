#include "stdafx.h"

void HandleLogin()
{
	char tmpUsername[40], tmpPassword[40];

	printf("Nhap ten dang nhap: ");
	scanf("%32s", tmpUsername);
	fflush(stdin);
	printf("Nhap mat khau: ");
	scanf("%32s", tmpPassword);
	fflush(stdin);

	UpdateKey();
	char buf[64];
	strncpy(buf, tmpUsername, 32);
	strncpy(buf + 32, tmpPassword, 32);
	SendPack(toTracker, 1, buf, 64);
	system("pause");
}

void HandleRegister()
{

}