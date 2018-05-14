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

struct NODE
{
	bool sleep, login;
	SOCKET socket;
	SOCKADDR_IN addr;
	char sessionKey[16];
	unsigned long long int rsaKey[3];
};

struct PEER_THREAD_PARAM
{
	NODE requestNode;
	bool pause;
	PEER_THREAD_PARAM()
	{
		this->pause = true;
	}
};

typedef std::map<std::string, std::string> USER_INFORMATION;
typedef std::map<std::string, std::string> FILE_INFORMATION;