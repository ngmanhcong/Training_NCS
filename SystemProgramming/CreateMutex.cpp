#include<Windows.h>
#include<iostream>

using namespace std;

int main() {
	HANDLE hMutex;
	cout << "Mutex 1" << endl;
	hMutex = CreateMutex(
		NULL,
		FALSE,
		L"MyMutex" // name of the mutex
	);

	if (hMutex == NULL) {
		cout << "CreateMutex failed with error: " << GetLastError() << endl;
	}
	cout << "CreateMutex succeeded." << endl;

	ReleaseMutex(hMutex);
	CloseHandle(hMutex);
}