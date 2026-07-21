#include<Windows.h>
#include<iostream>

using namespace std;

int main() {
	bool bHFile = CreateHardLink(
		L"D:\\Work\\NCS\\CheckAns\\ManhCong.txt",
		L"D:\\Work\\NCS\\CheckAns\\ThuHuong.txt",
		NULL
	);
	if (bHFile == FALSE) {
		cout << "CreateHardLink Failed and Error No - " << GetLastError() << endl;
	}
	else {
		cout << "CreateHardLink Success" << endl;
	}
}