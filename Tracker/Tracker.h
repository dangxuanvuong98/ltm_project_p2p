#pragma once

//Define Constants and Macro
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define TRACKER_DEFAULT_PORT 9000

#define MAX_PEER 1024

#define ERROR_CMD 0
#define REGISTER 1
#define REGISTER_SUCCESS REGISTER
#define REGISTER_FAIL -REGISTER
#define LOGIN 2
#define LOGIN_SUCCESS LOGIN
#define LOGIN_FAIL -LOGIN
#define UPLOAD 3
#define UPLOAD_SUCCESS UPLOAD
#define UPLOAD_FAIL -UPLOAD



//Include Headers
#include "stdc++.h"
#include "md5.h"
#include "rsa.h"
#include <WinSock2.h>
#include <Windows.h>


//Define Datatypes
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



//Declare Functions
void InitTracker();
DWORD WINAPI PriorityPeerThread(LPVOID lpParam);
DWORD WINAPI SleepPeerThread(LPVOID lpParam);

void SendPack(NODE node, int cmd, char *buf, int len);
int RecvPack(NODE sourcenode, OFFLINE_PACKAGE &recvPack);

bool operator <(NODE lhs, NODE rhs);

void InitConnetion(NODE newNode);
void Disconnect(NODE newNode);

void HandleRegister(NODE reqNode, OFFLINE_PACKAGE recvPack);
void HandleLogin(NODE reqNode, OFFLINE_PACKAGE recvPack);
void HandleUpdate(NODE reqNode, OFFLINE_PACKAGE recvPack);
void HandleCreate(NODE reqNode, OFFLINE_PACKAGE recvPack);
void HandleEdit(NODE reqNode, OFFLINE_PACKAGE recvPack);
void HandleDelete(NODE reqNode, OFFLINE_PACKAGE recvPack);
void HandleDownload(NODE reqNode, OFFLINE_PACKAGE recvPack);

//Declare Extern Variables
extern USER_INFORMATION userInformation;
extern FILE_INFORMATION fileInformation;

extern int numberUser;
extern int numberFile;

extern NODE myNode;

extern std::set<NODE> onlinePeer;
extern int onlinePeerAmount;

extern CRITICAL_SECTION criticalSection;

extern FILE *trackerLog;