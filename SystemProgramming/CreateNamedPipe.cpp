#include<Windows.h>
#include<iostream>

using namespace std;

int main() {
	HANDLE hCreateNamedPipe;
	hCreateNamedPipe = CreateNamedPipe(
		L"\\\\.\\pipe\\MyNamedPipe",
		PIPE_ACCESS_DUPLEX, // quyen truy cap pipe - cho phep doc ghi
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,  // gui theo tung message - doc theo tung message - cho phep block (doi den khi nao co du lieu)
		PIPE_UNLIMITED_INSTANCES, // so luong client toi da co the ket noi
		512,
		512,
		0,
		NULL
	);

	if (hCreateNamedPipe == INVALID_HANDLE_VALUE) {
		cout << "CreateNamedPipe failed with error: " << GetLastError() << endl;
	}
	else {
		cout << "CreateNamedPipe succeeded." << endl;
	}

	bool bConnectNamedPipe = ConnectNamedPipe(hCreateNamedPipe, NULL);
}