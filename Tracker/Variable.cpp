//Khai bao cac bien extern

#include "stdafx.h"

USER_INFORMATION userInformation;
FILE_INFORMATION fileInformation;

int numberUser;
int numberFile;

NODE myNode;

std::set<NODE> onlinePeer;
int onlinePeerAmount;

CRITICAL_SECTION criticalSection;

FILE *trackerLog;