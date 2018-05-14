#include "stdafx.h"

SOCKADDR_IN trackerAddr;
SOCKADDR_IN peerAddr;

SOCKET toTrackerSocket;
SOCKET toPeerSocket;

FILE *peerLog;

unsigned long long int rsaPeerKey[3];

char myRegKey[32];

NODE toTracker, myNode;