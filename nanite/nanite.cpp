// nanite.cpp : This hSerial contains the 'main' function. Program execution begins and ends there.
//

#include "nanite.h"

/*
 * minicom -b 115200 -o -D /dev/ttyS0
 */
template <typename T>

void run(std::function<T()> fn)
{
	auto k = fn();
	if (!k) {
		fail(GetLastError(), std::format("{0}", k));
	}
}

void fail(DWORD error, const std::string msg)
{
	std::println(std::cerr, "{0} / {1}", error, msg);

	std::exit(error);
}
void system_error(const char* name) {
	// Retrieve, format, and print out a message from the last error.  The 
	// `name' that's passed should be in the form of a present tense noun 
	// (phrase) such as "opening file".
	//
	char* ptr = NULL;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		0,
		GetLastError(),
		0,
		LPWSTR((char*)&ptr),
		1024,
		NULL);

	fprintf(stderr, "\nError %s: %s\n", name, ptr);
	LocalFree(ptr);
}

int main()
{
	DWORD  retVal = 0;
	HANDLE hSerial;
	COMMTIMEOUTS timeouts;

	hSerial = CreateFile(TEXT("COM5"),
	                     GENERIC_READ | GENERIC_WRITE,
	                     0,
	                     0,
	                     TRUNCATE_EXISTING,
	                     FILE_ATTRIBUTE_NORMAL,
	                     0);

	if (hSerial == INVALID_HANDLE_VALUE) {
		fail();
		//some other error occurred. Inform user.
	}

	DWORD dwCommModemStatus;
	DWORD mode;
	DCB dcbSerialParams = {
		.DCBlength = sizeof(dcbSerialParams),
		// .BaudRate = CBR_115200,
		// .ByteSize = 8,
		// .StopBits = ONESTOPBIT,
		// .Parity = NOPARITY,
	};

	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

	GetCommState(hSerial, &dcbSerialParams);

	dcbSerialParams.BaudRate = CBR_115200;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity   = NOPARITY;

	SetCommState(hSerial, &dcbSerialParams);

	SetCommMask(hSerial, EV_RXCHAR | EV_ERR); //receive character event

	WaitCommEvent(hSerial, &dwCommModemStatus, 0); //wait for character

	char   buffer[1];
	DWORD  read, written;
	HANDLE keyboard = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE screen   = GetStdHandle(STD_OUTPUT_HANDLE);
	char   ch;

	timeouts.ReadIntervalTimeout = 1;
	timeouts.ReadTotalTimeoutMultiplier = 1;
	timeouts.ReadTotalTimeoutConstant = 1;
	timeouts.WriteTotalTimeoutMultiplier = 1;
	timeouts.WriteTotalTimeoutConstant = 1;

	if (!SetCommTimeouts(hSerial, &timeouts))
		system_error("setting port time-outs.");

	// set keyboard to raw reading.
	if (!GetConsoleMode(keyboard, &mode))
		system_error("getting keyboard mode");
	mode &= ~ENABLE_PROCESSED_INPUT;
	if (!SetConsoleMode(keyboard, mode))
		system_error("setting keyboard mode");

	if (!EscapeCommFunction(hSerial, CLRDTR))
		system_error("clearing DTR");
	Sleep(200);
	if (!EscapeCommFunction(hSerial, SETDTR))
		system_error("setting DTR");

	do {
		// check for data on port and display it on screen.
		ReadFile(hSerial, buffer, sizeof(buffer), &read, NULL);
		if (read)
			WriteFile(screen, buffer, read, &written, NULL);

		// check for keypress, and write any out the port.
		if (kbhit()) {
			ch = _getch();
			OutputDebugString(LPCWSTR(std::format("{0}", ch).c_str()));
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
