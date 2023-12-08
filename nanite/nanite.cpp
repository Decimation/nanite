// nanite.cpp : This hSerial contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <print>
#include <stdio.h>
#include <conio.h>
#include <string.h>
/*
 * minicom -b 115200 -o -D /dev/ttyS0
 */

int main()
{
	DWORD retVal = 0;
	HANDLE hSerial;
	hSerial = CreateFile(
		TEXT("COM5"),
		GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0);

	if (hSerial == INVALID_HANDLE_VALUE) {
		if (GetLastError() == ERROR_FILE_NOT_FOUND) {
			//serial port does not exist. Inform user.
		}
		//some other error occurred. Inform user.
	}

	BYTE Byte;
	DWORD dwBytesTransferred;
	DWORD dwCommModemStatus;

	DCB dcbSerialParams = {
		.DCBlength = 0,
		// .BaudRate = CBR_115200,
		// .ByteSize = 8,
		// .StopBits = ONESTOPBIT,
		// .Parity = NOPARITY,
	};
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (!GetCommState(hSerial, &dcbSerialParams)) {
		//error getting state
		std::print("{0}", GetLastError());
	}
	dcbSerialParams.BaudRate = CBR_115200;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	if (!SetCommState(hSerial, &dcbSerialParams)) {
		//error setting serial port state
		std::print("{0}", GetLastError());
	}

	SetCommMask(hSerial, EV_RXCHAR | EV_ERR); //receive character event
	WaitCommEvent(hSerial, &dwCommModemStatus, 0); //wait for character
	char buffer[1];
	DWORD read, written;
	HANDLE keyboard = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE screen = GetStdHandle(STD_OUTPUT_HANDLE);
	char ch;
	do {
		// check for data on port and display it on screen.
		ReadFile(hSerial, buffer, sizeof(buffer), &read, NULL);
		if (read)
			WriteFile(screen, buffer, read, &written, NULL);

		// check for keypress, and write any out the port.
		if (_kbhit()) {
			ch = getchar();
			WriteFile(hSerial, &ch, 1, &written, NULL);
		}
		// until user hits ctrl-backspace.
	}
	while (ch != 127);

	CloseHandle(hSerial);

	return retVal;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage hSerials
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code hSerials, or Project > Add Existing Item to add existing code hSerials to the project
//   6. In the future, to open this project again, go to hSerial > Open > Project and select the .sln hSerial
