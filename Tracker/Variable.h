#pragma once

extern USER_INFORMATION userInformation;
extern FILE_INFORMATION fileInformation;

extern int numberUser;
extern int numberFile;

extern NODE myNode;

extern std::set<NODE> onlinePeer;
extern int onlinePeerAmount;

extern CRITICAL_SECTION criticalSection;

extern FILE *trackerLog;