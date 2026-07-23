#include<Windows.h>
#include<iostream>

using namespace std;

int main() {
	HANDLE hFile;
	hFile = CreateFile(
		L"D:\\Work\\NCS\\CheckAns\\newFile12.txt", 
		GENERIC_READ | GENERIC_WRITE,		// quyen truy cap mong muon (doc, ghi)			  
		0,								    // chia se file voi cac process khac (0 = khong chia se)
		NULL, 
		CREATE_NEW,						    // hanh dong khi file chua ton tai
		FILE_ATTRIBUTE_NORMAL,				// thuoc tinh file (an, read-only, ...)
		NULL
	);
	if( hFile == INVALID_HANDLE_VALUE) {
		cout << "CreateFile Failed and Error No - " << GetLastError() << endl;
	}
	else {
		cout << "CreateFile Success" << endl;
		CloseHandle(hFile);
	}
}

// tao file, mo file, ghi file, doc file