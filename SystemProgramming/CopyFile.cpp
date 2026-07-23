#include<Windows.h>
#include<iostream>

using namespace std;

int main() {
	bool bFile;
	bFile = CopyFile(
		L"D:\\Work\\NCS\\CheckAns\\oldFile.txt", 
		L"D:\\Work\\NCS\\CheckAns\\newFile.txt", 
		FALSE 
		// bFailIfExists: neu tham so = False, neu file dst da ton tai -> ghi de file src len file dst
		// neu tham so = True, neu file dst da ton tai -> CopyFile se that bai
	);
	if (bFile == FALSE) {
		cout << "CopyFile Failed and Error No - " << GetLastError() << endl;
	}
	else {
		cout << "CopyFile Success" << endl;
	}
	return 0;
}

//