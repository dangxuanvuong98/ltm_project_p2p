#include "stdafx.h"

//Define Extern Variables

//Luu tru danh sach file
std::map<int, FILE_INFO> fileList;
int fileAmount;

//Danh sach cac Peer dang online
CONNECTION onlinePeer;
//Tracker
NODE trackerNode;

//Tong so peer dang online
int onlinePeerAmount;

//Thong tin listerner: Dia chi, socket, bo ma hoa
CONNECTOR connector;

//Khoa CS
CRITICAL_SECTION criticalSection;

int IC;

BUFFER responseBuffer;
BUFFER chatBuffer;
BUFFER requsetBuffer;

bool connected = false;