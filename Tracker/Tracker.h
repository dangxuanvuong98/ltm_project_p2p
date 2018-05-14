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
#define UPLOAD 3
#define UPLOAD_SUCCESS UPLOAD
#define UPLOAD_FAIL -UPLOAD
#define MAX_PEER 1024
#define MAX_LENGTH 1024
#define FINISH_RESPONSE 9999



//Include Headers
#include "stdc++.h"
#include "md5.h"
#include "rsa.h"
#include <WinSock2.h>
#include <Windows.h>

using namespace std;


//Define Datatypes

//Define cac cau truc goi tin:
//OFFPACK - Goi tin chua duoc ma hoa/ da duoc giai ma - duoc su dung noi bo
//ONLPACK - Goi tin da duoc ma hoa - duoc su dung de truyen tin
struct OFFPACK
{
	long long int cmdCode;
	char data[4096];
};

struct ONLPACK
{
	char md5Code[32];
	char data[8208];
};

//Define cau truc luu tru thong tin cac node mang
//username - ten tai khoan ung voi node, username = "" neu peer chua dang nhap hoac node do la tracker
//addr - dia chi cua node
//rsaKey - public key cua node, cac node khac su dung key nay de ma hoa du lieu truoc khi truyen (n = resKey[0], e = rsaKey[1])
//CONNECTION - Cau truc anh xa socket va node tuong ung
//MY_INFO - Cau truc luu tru cac thong tin cua listener
struct NODE
{
	char username[32];
	SOCKADDR_IN addr;
	unsigned long long int rsaKey[2];
	bool busy;
};

typedef map<SOCKET, NODE> CONNECTION;

struct LISTENER
{
	SOCKET s;
	SOCKADDR_IN addr;
	unsigned long long int rsaKey[3];
};

//Define cau truc du lieu de truyen vao luong xu ly moi
//Bien pause khien luong cu tam dung cho den khi luong duoc tao nhan duoc thong tin ve socket
struct PEER_THREAD_PARAM
{
	SOCKET socket;
	bool pause;
};

//Define cau truc luu tru thong tin cua file
//BLOCK - luu tru thong tin mot khoi tin
//checksum - ma md5 cua BLOCK/ FILE
struct BLOCK
{
	char checksum[32];
	SOCKET local[MAX_PEER];
};

struct FILE_INFO
{
	char name[32];
	unsigned length;
	char checksum[32];
	BLOCK block[MAX_LENGTH];
};

//Cac cau truc khac
typedef map<string, string> USER;

//Declare Functions

//Overload operator
bool operator<(FILE_INFO lhs, FILE_INFO rhs)
{
	return (strncmp(lhs.name, rhs.name, 32) < 0);
}

//Declare cac ham phu trach viec ket noi
//InitTracker - Khoi tao tracker, dua tracker vao trang thai san sang lam viec
//WaitForNewConnection - Cho doi ket noi moi, yeu cau dang nhap va gui thong tin dang nhap den cac Peer khac
//WaitForRequest - Cho doi request tu cac peer da dang nhap va gui thong tin den cac peer con lai
void InitListener(LISTENER &listener);
DWORD WINAPI WaitForNewConnection(LPVOID lpParam);
DWORD WINAPI WaitForRequest(LPVOID lpParam);


//Declare cac ham vao ra
//SendPack - Ma hoa mot OFFPACK thanh ONLPACK va gui den dia chi dich
//RecvPack - Nhan mot ONLPACK va giai ma thanh OFFPACK
void SendPack(SOCKET s, int cmd, char *buf, int len);
int RecvPack(SOCKET s, OFFPACK &recvPack);

//Declare cac ham xu ly: Register, Login, Update
void HandleRegister(SOCKET s, OFFPACK recvPack);
void HandleLogin(SOCKET s, OFFPACK recvPack);
void HandleUpload(SOCKET s, OFFPACK recvPack);
void HandleUpdate(SOCKET s, OFFPACK recvPack);

//Declare cac ham ma hoa rsa
int UpdateKey(unsigned long long int *rsaKey);
void rsaenc(char * ori, char * enc, unsigned long long int * key, unsigned long int len);
void rsadec(char * enc, char * ori, unsigned long long int * key, unsigned long int len);

//Declare Extern Variables
extern LISTENER listener;

extern USER userList;
extern set<FILE_INFO> fileList;

extern CONNECTION onlinePeer;
extern int onlinePeerAmount;

extern CRITICAL_SECTION criticalSection;