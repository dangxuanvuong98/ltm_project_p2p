#pragma once

//Define Constants and Macros
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define TRACKER_DEFAULT_PORT 9000
#define MAX_PEER 1024

#define MAX_PEER 1024

#define ERROR_CMD 0

#define REGISTER 1
#define REGISTER_SUCCESS REGISTER
#define REGISTER_FAIL -REGISTER

#define LOGIN 2
#define LOGIN_SUCCESS LOGIN
#define LOGIN_FAIL -LOGIN

#define GET 3

#define POST 4

#define GET_ALL 1
#define GET_ONLINE_PEER 2
#define GET_FILE_LIST 4
#define GET_FILE_INFORMATION 8

#define NEW_FILE 1
#define EDIT_BLOCK 2
#define I_HAVE_A_BLOCK 3
#define HE_HAS_THIS_BLOCK I_HAVE_A_BLOCK
#define HE_DOESNT_HAVE_THIS_BLOCK 4
#define ONLINE_PEER 5
#define FILE_NAME 6



//Include Headers
#include "stdc++.h"
#include "md5.h"
#include "rsa.h"
#include <WinSock2.h>
#include <Windows.h>


//Define Datatypes

//Define cac cau truc goi tin:

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
	int numdup;
	SOCKET local[MAX_PEER];
};

//Cau truc luu tru thong tin file
//checksum - ma md5 cua FILE
struct FILE_INFO
{
	char name[32];
	unsigned length;
	char checksum[32];
	std::map<int, BLOCK> block;

	FILE_INFO()
	{
		strcpy(this->name, "");
		this->length = 0;
		block = std::map<int, BLOCK>();
	}
};

//Cac cau truc khac

//Cau truc luu truc thong tin nguoi dung: map tu tai khoan sang mat khau
typedef std::map<std::string, std::string> USER;

//Declare Functions

//Overload operator

//Overload toan tu nho hon cho cau truc FILE_INFO
bool operator<(FILE_INFO lhs, FILE_INFO rhs);

//Declare cac ham phu trach viec ket noi

//InitTracker - Khoi tao tracker, dua tracker vao trang thai san sang lam viec
void InitListener(LISTENER &listener);
//WaitForNewConnection - Cho doi ket noi moi, yeu cau dang nhap va gui thong tin dang nhap den cac Peer khac
DWORD WINAPI WaitForNewConnection(LPVOID lpParam);
//WaitForRequest - Cho doi request tu cac peer da dang nhap va gui thong tin den cac peer con lai
DWORD WINAPI WaitForRequest(LPVOID lpParam);
//Thiet lap ket noi ban dau giua peer va tracker
DWORD WINAPI SetupConnection(LPVOID lpParam);
//Dieu huong xu ly request
DWORD WINAPI RequestControl(LPVOID lpParam);
//Huy ket noi voi mot socket
void Disconnect(SOCKET s);


//Declare cac ham vao ra

//SendPack - Ma hoa mot OFFPACK thanh ONLPACK va gui den dia chi dich
void SendPack(SOCKET s, OFFPACK sendPack, int len);
//RecvPack - Nhan mot ONLPACK va giai ma thanh OFFPACK
int RecvPack(SOCKET s, OFFPACK &recvPack);

//Declare cac ham xu ly: Register, Login, Update

//Xu ly yeu cau dang ky tai khoan
void HandleRegister(SOCKET s, OFFPACK recvPack);
//Xy ly yeu cau dang nhap
void HandleLogin(SOCKET s, OFFPACK recvPack);
//Xu ly yeu cap nhat thong tin (get thong tin tu tracker xuong)
void HandlePost(SOCKET s, OFFPACK recvPack);
//Xy ly yeu cau cap nhat thong tin (post thong tin tu peer len)
void HandleGet(SOCKET s, OFFPACK recvPack);

//Declare cac ham ma hoa rsa

//Tao bo key RSA
int CreateKey(unsigned long long int *rsaKey);
//Ma hoa buf ori co chieu dai len thanh buf enc voi bo ma hoa key
void rsaenc(char * ori, char * enc, unsigned long long int * key, unsigned long int len);
//Giai ma buf enc co chieu dai len thanh buf ori voi bo ma hoa key
void rsadec(char * enc, char * ori, unsigned long long int * key, unsigned long int len);

//Declare Extern Variables

//Thong tin listerner: Dia chi, socket, bo ma hoa
extern LISTENER listener;

//Luu truc thong tin cac tai khoan
extern USER userList;

//Luu tru danh sach file
extern std::map<int, FILE_INFO> fileList;
extern int fileAmount;

//Danh sach cac Peer dang online
extern CONNECTION onlinePeer;

//Tong so peer dang online
extern int onlinePeerAmount;

//Khoa CS
extern CRITICAL_SECTION criticalSection;