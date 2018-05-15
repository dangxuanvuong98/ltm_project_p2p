#pragma once

//Define Constants and Macros
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
#define GET 3
#define GET_SUCCESS GET
#define GET_FAIL -GET
#define POST 4
#define POST_SUCCESS POST
#define POST_FAIL -POST
#define MAX_PEER 1024
#define MAX_LENGTH 1024
#define FINISH_RESPONSE 9999

#define GET_ALL 1
#define GET_ONLINE_PEER 2
#define GET_FILE_LIST 4
#define GET_FILE_INFORMATION 8

//Include headers
#include "stdc++.h"
#include "md5.h"
#include "rsa.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

//Define Datatypes

//OFFPACK - Goi tin chua duoc ma hoa/ da duoc giai ma - duoc su dung noi bo
struct OFFPACK
{
	long long int cmdCode;
	char data[4096];
};

//ONLPACK - Goi tin da duoc ma hoa - duoc su dung de truyen tin
struct ONLPACK
{
	char md5Code[32];
	char data[8208];
};

//Define cau truc luu tru thong tin cac node mang
//username - ten tai khoan ung voi node, username = "" neu peer chua dang nhap hoac node do la tracker
//addr - dia chi cua node
//rsaKey - public key cua node, cac node khac su dung key nay de ma hoa du lieu truoc khi truyen (n = resKey[0], e = rsaKey[1])
struct NODE
{
	char username[32];
	SOCKADDR_IN addr;
	unsigned long long int rsaKey[2];
	bool connecting;
};

//CONNECTION - Cau truc anh xa socket va node tuong ung
typedef std::map<SOCKET, NODE> CONNECTION;

//Define cau truc du lieu de luu tru thong tin listener
struct LISTENER
{
	SOCKET s;
	SOCKADDR_IN addr;
	unsigned long long int rsaKey[3];
};

//Define cau truc du lieu de truyen vao luong xu ly moi
//Bien pause khien luong cu tam dung cho den khi luong duoc tao nhan duoc thong tin ve socket
struct THREAD_PARAM
{
	SOCKET s;
	bool pause;
};

//Define cau truc luu tru thong tin cua file

//BLOCK - luu tru thong tin mot khoi tin
//checksum - ma md5 cua BLOCK
struct BLOCK
{
	char checksum[32];
	SOCKET local[MAX_PEER];
};

//Cau truc luu tru thong tin file
//checksum - ma md5 cua FILE
struct FILE_INFO
{
	char name[32];
	unsigned length;
	char checksum[32];
	BLOCK block[MAX_LENGTH];
};

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