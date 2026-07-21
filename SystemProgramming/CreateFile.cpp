#include<Windows.h>
#include<iostream>

using namespace std;

int main() {
	HANDLE hFile;
	hFile = CreateFile(
		L"D:\\Work\\NCS\\CheckAns\\newFile12.txt", 
		GENERIC_READ | GENERIC_WRITE,			  
		0,										  
		NULL, 
		CREATE_NEW, 
		FILE_ATTRIBUTE_NORMAL, 
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