#include "stdafx.h"

/*void ReadUserInformation();
void ReadFileInformation();*/

void InitTracker()
{
	//Doc du lieu vao ram
	/*ReadUserInformation();
	ReadFileInformation();*/
	UpdateKey();

	trackerLog = fopen("trackerlog.dat", "a");

	//Khoi tao socket
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	myNode.socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	myNode.addr.sin_addr.s_addr = htonl(INADDR_ANY);
	myNode.addr.sin_port = htons(TRACKER_DEFAULT_PORT);
	myNode.addr.sin_family = AF_INET;

	bind(myNode.socket, (SOCKADDR FAR*)&myNode.addr, sizeof(myNode.addr));

	onlinePeerAmount = 0;
	
	listen(myNode.socket, 1024);
	fprintf(trackerLog, "Server da san sang\n");
	printf("Server da san sang\n");
}

//Ham doc thong tin nguoi dung: userID, username, ma hash cua mat khau
void ReadUserInformation()
{
	FILE * f = fopen("trackerdata.UserInformation.dat", "r");
	if (f == NULL)
	{
		f = fopen("trackerdata.UserInformation.dat", "w");
		fclose(f);
		f = fopen("trackerdata.UserInformation.dat", "r");
	}

	int ret;
	char tmpUser[32], tmpPassword[32];

	numberUser = 0;
	userInformation.clear();

	while (!feof(f))
	{
		ret = fscanf(f, "%s%s", tmpUser, tmpPassword);

		if (ret < 2)
		{
			break;
		}

		userInformation[tmpUser] = tmpPassword;
		
		numberUser++;
	}

	fclose(f);
}

//Ham doc thong tin cac file trong he thong
/*void ReadFileInformation()
{
	FILE * f = fopen("trackerdata.FileInformation.dat", "r");
	if (f == NULL)
	{
		f = fopen("trackerdata.FileInformation.dat", "w");
		fclose(f);
		f = fopen("trackerdata.FileInformation.dat", "r");
	}

	int ret;
	char tmpFile[32];

	numberFile = 0;
	fileInformation.clear();

	while (!feof(f))
	{
		ret = fscanf(f, "%s", tmpFile);

		if (ret < 1)
		{
			break;
		}

		fileInformation[tmpFile] = FILE_CODE();

		numberFile++;
	}
	fclose(f);	
	fileInformation["_chatfile_"] = FILE_CODE();

	char *c;

	FILE_INFORMATION::iterator it = fileInformation.begin();
	while (it != fileInformation.end())
	{
		sprintf(tmpFile, "filedata.%s", it->first.c_str());
		f = fopen(tmpFile, "r");
		if (f == NULL)
		{
			if (strcmp(it->first.c_str(), "_chatfile_") != 0)
			{
				fileInformation.erase(it);
			}
			else
			{
				f = fopen(tmpFile, "w");
				fclose(f);
			}
		}
		else
		{
			while (!feof(f))
			{
				c = fgets(tmpFile, 32, f);
				if (c == NULL || strlen(tmpFile) != 32)
				{
					break;
				}

				it->second.insert(it->second.end(), tmpFile);
			}
			fclose(f);
		}
		it++;
	}
}*/