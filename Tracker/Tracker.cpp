#include "stdafx.h"

int main()
{
	InitializeCriticalSection(&criticalSection);

	InitListener(listener);
	CreateThread(0, 0, WaitForRequest, NULL, 0, 0);
	WaitForNewConnection(NULL);
}