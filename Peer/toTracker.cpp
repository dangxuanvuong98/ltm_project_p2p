#include "stdafx.h"

using namespace std;

int InitConnectionToTracker()
{
	char buf[16];

	memcpy(buf, (char *)&(myNode.rsaKey[0]), 8);
	memcpy(buf + 8, (char *)&(myNode.rsaKey[1]), 8);
	send(toTracker.socket, buf, 16, 0);

	//cout << myNode.rsaKey[0] << ' ' << myNode.rsaKey[1] << endl;

	fd_set readfds;
	timeval limit = { 10, 0 };
	FD_ZERO(&readfds);
	FD_SET(toTracker.socket, &readfds);

	int ret = select(0, &readfds, NULL, NULL, &limit);
	if (ret > 0)
	{
		int ret2 = recv(toTracker.socket, buf, sizeof(buf), 0);
		if (ret2 != 16)
		{
			return -1;
		}

		toTracker.rsaKey[0] = *((unsigned long long int *)buf);
		char tmpchar[8];
		memcpy(tmpchar, buf + 8, 8);
		toTracker.rsaKey[1] = *((unsigned long long int *)tmpchar);
		//cout << toTracker.rsaKey[0] << ' ' << toTracker.rsaKey[1] << endl;
	}
	else
	{
		return -1;
	}
	return 0;
}

int ConnectionFromTracker(NODE peer)
{
	return 0;
}

int ConnectionToTracker(NODE peer)
{
	return 0;
}