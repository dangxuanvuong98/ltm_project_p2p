#pragma once

//Define Constants and Macros
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "stdc++.h"
#include "md5.h"
#include "rsa.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
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

//Declare Functions
void GoOnline();
void ApplicationUI();

void HandleLogin();
void HandleRegister();

void SendPack(NODE node, int cmd, char *buf, int len);
OFFLINE_PACKAGE RecvPack(NODE sourcenode);

int InitConnectionToTracker();
void ConnectionFromPeer(NODE newNode);
int ConnectionToPeer(NODE peer);

//Declare Extern Variables
extern SOCKADDR_IN trackerAddr;
extern SOCKADDR_IN peerAddr;

extern SOCKET toTrackerSocket;
extern SOCKET toPeerSocket;

extern FILE *peerLog;

extern char myRegKey[32];

extern NODE toTracker, myNode;