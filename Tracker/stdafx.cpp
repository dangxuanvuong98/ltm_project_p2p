
#include "stdafx.h"

bool operator<(FILE_INFO lhs, FILE_INFO rhs)
{
	return (strncmp(lhs.name, rhs.name, 32) < 0);
}

LISTENER listener;

USER userList;
std::map<int, FILE_INFO> fileList = std::map<int, FILE_INFO>();
int fileAmount = 0;

CONNECTION onlinePeer;
int onlinePeerAmount;

CRITICAL_SECTION criticalSection;