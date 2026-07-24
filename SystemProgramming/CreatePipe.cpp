#include<Windows.h>
#include<iostream>

using namespace std;

int main() {
	bool bCreatePipe;

	HANDLE hReadFile;
	HANDLE hWriteFile;

	bCreatePipe = CreatePipe(
		&hReadFile,
		&hWriteFile,
		NULL,
		0
	);

	if (bCreatePipe == FALSE) {
		cout << "CreatePipe Failed and Error No - " << GetLastError() << endl;
	}
	else {
		cout << "CreatePipe Success" << endl;
	}

	CloseHandle(hReadFile);
	CloseHandle(hWriteFile);

	return 0;
}

/*
IPC la co che giao tiep giua process khac nhau, trong do co pipe
pipe gom 2 loai: anonymous pipe va named pipe
duoc su dung cho one way communication or undirectional communication
khong the su dung qua network
tai 1 dau chung ta chi co the read or write, dau con lai write or read
*/