#include <iostream>
#include <string>
#include <windows.h>

using namespace std;

// Function Prototypes:
HANDLE InitializeSerialHandle();
void WriteToUART(HANDLE, char[], DWORD&);
void ReadFromUART(HANDLE, char*, DWORD, DWORD&);

int main() {
	// Serial
	HANDLE serialHandle = InitializeSerialHandle();
	DWORD bytesWritten = 0;
	DWORD bytesRead = 0;

	char w[] = "c";
	char r[9] = { 0 };

	while (true) {
		cin >> w;
		if (std::string(w) == "stop") {
			cout << "breaking";
			break;
		}
		WriteToUART(serialHandle, w, bytesWritten);

		ReadFromUART(serialHandle, r, strlen(w), bytesRead);
		cout << "Read " << bytesRead  << " of " << strlen(w) << " : " << string(r) << endl;
	}

	CloseHandle(serialHandle); // Close things up.

	int ret; cin >> ret; // Prevent the window from closing.

	return 0;
}

void WriteToUART(HANDLE serialHandle, char data[], DWORD& bytesWritten) {
	if (!WriteFile(serialHandle, data, strlen(data), &bytesWritten, NULL)) cout << "Error writing: " << GetLastError();
}

void ReadFromUART(HANDLE serialHandle, char* buffer, DWORD length, DWORD& bytesRead) {
	if (!ReadFile(serialHandle, buffer, length, &bytesRead, NULL)) cout << "Error reading: " << GetLastError();
}

HANDLE InitializeSerialHandle() {
	HANDLE serialHandle = CreateFile("COM4",
									 GENERIC_READ | GENERIC_WRITE,
									 0,
									 0,
									 OPEN_EXISTING,
									 FILE_ATTRIBUTE_NORMAL,
									 0);

	if (serialHandle == INVALID_HANDLE_VALUE) cout << "Error initializing HANDLE: " << GetLastError();

	DCB serialParameters = { 0 };
	serialParameters.DCBlength = sizeof(serialParameters);

	// Communication Settings:
	if (!GetCommState(serialHandle, &serialParameters)) cout << "Error getting comm state: " << GetLastError();
		serialParameters.BaudRate = CBR_115200;
		serialParameters.ByteSize = 8;
		serialParameters.StopBits = ONESTOPBIT;
		serialParameters.Parity = NOPARITY;
	if (!SetCommState(serialHandle, &serialParameters)) cout << "Error setting comm state: " << GetLastError(); // Apply the communication settings.

	// Timeout Settings:
	COMMTIMEOUTS serialTimeouts = { 0 };
		serialTimeouts.ReadIntervalTimeout = 1; // How long to wait between chars in ms.
		serialTimeouts.ReadTotalTimeoutConstant = 5; // How long to wait before returning in ms.
		serialTimeouts.ReadTotalTimeoutMultiplier = 10; // How much longer to wait before returning for each byte that was requested by the read.
		serialTimeouts.WriteTotalTimeoutConstant = 50; // How long to wait before returning in ms.
		serialTimeouts.WriteTotalTimeoutMultiplier = 10;// How much longer to wait before returning for each byte that was requested by the write.
	if (!SetCommTimeouts(serialHandle, &serialTimeouts)) cout << "Error setting timeouts: " << GetLastError(); // Apply the timeout settings.

	return serialHandle;
}
