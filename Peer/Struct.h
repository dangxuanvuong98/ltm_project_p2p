#pragma once

struct OFFLINE_PACKAGE
{
	long long int cmdCode;
	char data[4096];
};

struct ONLINE_PACKAGE
{
	char md5Code[32];
	char data[8208];
};

typedef std::map<std::string, std::string> USER_INFORMATION;
typedef std::vector<std::string> FILE_CODE;
typedef std::map<std::string, FILE_CODE> FILE_INFORMATION;

struct NODE
{
	SOCKET socket;
	SOCKADDR_IN addr;
	char sessionKey[16];
	unsigned long long int rsaKey[3];
};