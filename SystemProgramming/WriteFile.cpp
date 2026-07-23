#include<Windows.h>
#include<iostream>

using namespace std;

int main() {
	// Create a file
	HANDLE hFile;
	hFile = CreateFile(
		L"D:\\Work\\NCS\\CheckAns\\newFile4.txt", 
		GENERIC_READ | GENERIC_WRITE, 
		0, 
		NULL, 
		CREATE_NEW, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL
	);

	// initialize data to write
	char chBuffer[] = "Hello World";
	DWORD dwBytesToWritte = strlen(chBuffer);
	DWORD dwBytesWritten = 0;

	// Write to file
	bool bWFile = WriteFile(
		hFile, 
		chBuffer,			// con tro -> vung nho chua du lieu can ghi
		dwBytesToWritte,    // so byte can ghi
		&dwBytesWritten,    // con tro -> so byte da ghi thuc su
		NULL
	);

	// Close HANDLE
	CloseHandle(hFile);
}