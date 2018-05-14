#include "stdafx.h"

int LoginMenu();
void HandleLogin();
void HandleRegister();

void ApplicationUI()
{
	int choice;

	do
	{
		choice = LoginMenu();

		if (choice == 1)
		{
			HandleLogin();
		}
		else if (choice == 2)
		{
			HandleRegister();
		}
	} while (choice != 1);
}

int LoginMenu()
{
	{
		int key;

		while (true)
		{
			system("cls");
			printf("1 - Login\n");
			printf("2 - Create account\n");

			key = _getch();

			if (key == '1')
			{
				return 1;
			}
			else if (key == '2')
			{
				return 2;
			}
			else
			{
				printf("Invalid Choice!");
				system("pause");
			}
		}
	}
}