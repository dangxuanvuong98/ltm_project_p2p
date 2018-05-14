#pragma once

extern SOCKADDR_IN trackerAddr;
extern SOCKADDR_IN peerAddr;

extern SOCKET toTrackerSocket;
extern SOCKET toPeerSocket;

extern FILE *peerLog;

extern char myRegKey[32];

extern NODE toTracker, myNode;