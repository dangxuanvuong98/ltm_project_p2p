#include "stdafx.h"

using namespace std;

int main()
{
	InitConnector(connector);

	InitializeCriticalSection(&criticalSection);

	CreateThread(0, 0, ListenToTracker, NULL, 0, 0);
	CreateThread(0, 0, ListenToPeer, NULL, 0, 0);

	UserInterface();

	return 0;
}