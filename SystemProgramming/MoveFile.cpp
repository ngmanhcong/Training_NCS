#include<Windows.h>
#include<iostream>

using namespace std;

int main() {
	bool bMFile;
	bMFile = MoveFile(L"D:\\Work\\NCS\\CheckAns\\newFile.txt", L"D:\\Work\\NCS\\CheckAns\\newFile1.txt");
	if (bMFile == FALSE) {
		cout << "MoveFile Failed and Error No - " << GetLastError() << endl;
	}
	else {
		cout << "MoveFile Success" << endl;
	}
	return 0;
}