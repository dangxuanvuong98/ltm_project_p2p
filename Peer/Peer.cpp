#include "stdafx.h"

using namespace std;

int main()
{
	peerLog = fopen("peerlog.dat", "a");

	GoOnline();
	ApplicationUI();
	

	return 0;
}