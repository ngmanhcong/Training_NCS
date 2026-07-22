#include<Windows.h>
#include<iostream>

using namespace std;

DWORD WINAPI ThreadFunction(LPVOID lpParam) {
	cout << "Thread is running..." << endl;
	return 0;
}

int main() {
	HANDLE hThread;
	DWORD ThreadId;
	hThread = CreateThread(
		NULL,                   // default security attributes
		0,                      // use default stack size  
		ThreadFunction,         // thread function name
		NULL,                   // argument to thread function 
		0,                      // use default creation flags 
		&ThreadId               // returns the thread identifier
	);           
	if (hThread == NULL) {
		cout << "CreateThread failed with error: " << GetLastError() << endl;
		return 1;
	}
	cout << "Thread created successfully with Thread ID: " << ThreadId << endl;
	return 0;
}