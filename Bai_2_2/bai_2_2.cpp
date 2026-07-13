// ============================================================
//  Bai_2_2 - Thread Monitor
//  Buoc 1: Liet ke Process kem %CPU
//  Buoc 2: Nhap PID -> liet ke Thread (TID, State, Priority, %CPU)
// ============================================================

#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <iomanip>
#include <algorithm>
#include <thread>
#include <chrono>

// ================= NT internal structures (undocumented) =================
typedef LONG NTSTATUS;
#define STATUS_SUCCESS 0x00000000
#define STATUS_INFO_LENGTH_MISMATCH 0xC0000004

typedef enum _THREAD_STATE {
    StateInitialized, StateReady, StateRunning, StateStandby,
    StateTerminated, StateWaiting, StateTransition, StateUnknown
} THREAD_STATE;

typedef struct _CLIENT_ID_LOCAL {
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
} CLIENT_ID_LOCAL;

typedef struct _SYSTEM_THREAD {
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER CreateTime;
    ULONG WaitTime;
    PVOID StartAddress;
    CLIENT_ID_LOCAL ClientId;
    LONG Priority;
    LONG BasePriority;
    ULONG ContextSwitchCount;
    THREAD_STATE State;
    ULONG WaitReason;
} SYSTEM_THREAD;

typedef struct _UNICODE_STRING_LOCAL {
    USHORT Length, MaximumLength;
    PWSTR Buffer;
} UNICODE_STRING_LOCAL;

typedef struct _SYSTEM_PROCESS_INFORMATION_LOCAL {
    ULONG NextEntryOffset;
    ULONG NumberOfThreads;
    LARGE_INTEGER SpareLi1, SpareLi2, SpareLi3;
    LARGE_INTEGER CreateTime, UserTime, KernelTime;
    UNICODE_STRING_LOCAL ImageName;
    LONG BasePriority;
    HANDLE ProcessId;
    HANDLE InheritedFromProcessId;
    ULONG HandleCount;
    ULONG SessionId;
    ULONG_PTR PageDirectoryBase;
    SIZE_T PeakVirtualSize, VirtualSize;
    ULONG PageFaultCount;
    SIZE_T PeakWorkingSetSize, WorkingSetSize;
    SIZE_T QuotaPeakPagedPoolUsage, QuotaPagedPoolUsage;
    SIZE_T QuotaPeakNonPagedPoolUsage, QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage, PeakPagefileUsage, PrivatePageCount;
    LARGE_INTEGER ReadOperationCount, WriteOperationCount, OtherOperationCount;
    LARGE_INTEGER ReadTransferCount, WriteTransferCount, OtherTransferCount;
    SYSTEM_THREAD Threads[1];
} SYSTEM_PROCESS_INFORMATION_LOCAL;

typedef NTSTATUS(NTAPI* pNtQuerySystemInformation)(ULONG, PVOID, ULONG, PULONG);
pNtQuerySystemInformation NtQuerySystemInformation = nullptr;

// ================= Ham phu =================
const char* ThreadStateToStr(THREAD_STATE s) {
    switch (s) {
    case StateInitialized: return "Initialized";
    case StateReady:       return "Ready";
    case StateRunning:     return "Running";
    case StateStandby:     return "Standby";
    case StateTerminated:  return "Terminated";
    case StateWaiting:     return "Waiting";
    case StateTransition:  return "Transition";
    default:                return "Unknown";
    }
}

// Lay toan bo snapshot process/thread cua he thong
std::vector<BYTE> GetSystemSnapshotBuffer() {
    ULONG bufSize = 1 << 20;
    std::vector<BYTE> buffer(bufSize);
    ULONG needed = 0;
    NTSTATUS status;

    while (true) {
        status = NtQuerySystemInformation(5 /*SystemProcessInformation 
                                            -> ghi du lieu processes va thread vao buffer*/,
            buffer.data(), bufSize, &needed);
        if (status == STATUS_INFO_LENGTH_MISMATCH) {
            bufSize = needed + 4096;
            buffer.resize(bufSize);
            continue;
        }
        break;
    }
    if (status != STATUS_SUCCESS) buffer.clear();
    return buffer;
}

// ================= Phan 1: Danh sach Process kem %CPU =================
struct ProcCpuInfo {
    std::wstring name;
    ULONGLONG cpuTime100ns;
};

std::map<DWORD, ProcCpuInfo> ParseProcessCpuTimes(std::vector<BYTE>& buffer) {
    std::map<DWORD, ProcCpuInfo> result;
    if (buffer.empty()) return result;

    BYTE* p = buffer.data();
    while (true) {
        auto cur = (SYSTEM_PROCESS_INFORMATION_LOCAL*)p;
        DWORD pid = (DWORD)(ULONG_PTR)cur->ProcessId;

        std::wstring name = L"(idle/system)";
        if (cur->ImageName.Buffer && cur->ImageName.Length > 0)
            name.assign(cur->ImageName.Buffer, cur->ImageName.Length / sizeof(WCHAR));

        ULONGLONG cpu = cur->KernelTime.QuadPart + cur->UserTime.QuadPart;
        result[pid] = { name, cpu };

        if (cur->NextEntryOffset == 0) break;
        p += cur->NextEntryOffset;
    }
    return result;
}

void ShowProcessListWithCpuUsage() {
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    DWORD numCPU = si.dwNumberOfProcessors;
    if (numCPU == 0) numCPU = 1;

    std::cout << "Dang do CPU usage cua cac process (~800ms)...\n";

    auto buf1 = GetSystemSnapshotBuffer(); // du lieu tho do Windows tra ve
    auto t1 = std::chrono::steady_clock::now();
    auto snap1 = ParseProcessCpuTimes(buf1); // chuyen thanh du lieu ma chuong trinh co the doc va su dung
                                             // co dang PID → tên process, tổng CPU time
                                             // vi du 1234 → chrome.exe, 50.000.000

    std::this_thread::sleep_for(std::chrono::milliseconds(800));

    auto buf2 = GetSystemSnapshotBuffer();
    auto t2 = std::chrono::steady_clock::now();
    auto snap2 = ParseProcessCpuTimes(buf2);

    double elapsedSec = std::chrono::duration<double>(t2 - t1).count();

    struct Row { DWORD pid; std::wstring name; double cpuPercent; };
    std::vector<Row> rows;

    for (auto& kv : snap2) {
        DWORD pid = kv.first;
        ProcCpuInfo& info2 = kv.second;

        auto it1 = snap1.find(pid);
        if (it1 == snap1.end()) continue;

        ULONGLONG time1 = it1->second.cpuTime100ns;
        ULONGLONG delta = (info2.cpuTime100ns >= time1) ? (info2.cpuTime100ns - time1) : 0;
        double deltaSec = delta / 10000000.0;
        double cpuPercent = (elapsedSec > 0) ? (deltaSec / (elapsedSec * numCPU)) * 100.0 : 0.0;

        rows.push_back({ pid, info2.name, cpuPercent });
    }

    std::sort(rows.begin(), rows.end(),
        [](const Row& a, const Row& b) { return a.cpuPercent > b.cpuPercent; });

    std::wcout << std::left
        << std::setw(10) << L"PID"
        << std::setw(30) << L"Ten Process"
        << std::setw(10) << L"%CPU" << L"\n";
    std::wcout << std::wstring(50, L'-') << L"\n";

    int count = 0;
    for (auto& r : rows) {
        if (r.pid == 0) continue; // System Idle Process
        std::wcout << std::left
            << std::setw(10) << r.pid
            << std::setw(30) << r.name
            << std::setw(10) << std::fixed << std::setprecision(2) << r.cpuPercent
            << L"\n";
        if (++count >= 30) break;
    }
}

// ================= Phan 2: Danh sach Thread cua 1 PID kem %CPU =================
std::map<DWORD, ULONGLONG> GetThreadCpuTimes(DWORD pid, std::vector<BYTE>& buffer) {
    std::map<DWORD, ULONGLONG> result;
    if (buffer.empty()) return result;

    BYTE* p = buffer.data();
    while (true) {
        auto cur = (SYSTEM_PROCESS_INFORMATION_LOCAL*)p;
        if ((DWORD)(ULONG_PTR)cur->ProcessId == pid) {
            for (ULONG i = 0; i < cur->NumberOfThreads; i++) {
                SYSTEM_THREAD& t = cur->Threads[i];
                DWORD tid = (DWORD)(ULONG_PTR)t.ClientId.UniqueThread;
                ULONGLONG cpu = t.KernelTime.QuadPart + t.UserTime.QuadPart;
                result[tid] = cpu;
            }
            break;
        }
        if (cur->NextEntryOffset == 0) break;
        p += cur->NextEntryOffset;
    }
    return result;
}

struct ThreadInfoLive {
    THREAD_STATE state;
    LONG priority;
};

/*
Tìm process có PID được truyền vào trong buffer hệ thống, 
sau đó lấy trạng thái và priority của từng thread thuộc process đó.
*/
std::map<DWORD, ThreadInfoLive> GetThreadLiveInfo(DWORD pid, std::vector<BYTE>& buffer) {
    std::map<DWORD, ThreadInfoLive> result;
    if (buffer.empty()) return result;

    BYTE* p = buffer.data();
    while (true) {
        auto cur = (SYSTEM_PROCESS_INFORMATION_LOCAL*)p;
        if ((DWORD)(ULONG_PTR)cur->ProcessId == pid) {
            for (ULONG i = 0; i < cur->NumberOfThreads; i++) {
                SYSTEM_THREAD& t = cur->Threads[i];
                DWORD tid = (DWORD)(ULONG_PTR)t.ClientId.UniqueThread;
                ThreadInfoLive info;
                info.state = t.State;
                info.priority = t.Priority;
                result[tid] = info;
            }
            break;
        }
        if (cur->NextEntryOffset == 0) break;
        p += cur->NextEntryOffset;
    }
    return result;
}

void ListThreadsOfProcess(DWORD pid) {
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    DWORD numCPU = si.dwNumberOfProcessors;
    if (numCPU == 0) numCPU = 1;

    std::cout << "\nDang do %CPU cho tung thread (~800ms)...\n";

    auto buf1 = GetSystemSnapshotBuffer();
    auto t1 = std::chrono::steady_clock::now();
    auto cpu1 = GetThreadCpuTimes(pid, buf1);

    if (cpu1.empty()) {
        std::cout << "Khong tim thay thread nao cua PID " << pid
            << " (co the PID khong ton tai)\n";
        return;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(800));

    auto buf2 = GetSystemSnapshotBuffer();
    auto t2 = std::chrono::steady_clock::now();
    auto cpu2 = GetThreadCpuTimes(pid, buf2);
    auto liveInfo = GetThreadLiveInfo(pid, buf2);

    double elapsedSec = std::chrono::duration<double>(t2 - t1).count();

    struct Row { DWORD tid; double cpuPercent; THREAD_STATE state; LONG priority; };
    std::vector<Row> rows;

    for (auto& kv : cpu2) {
        DWORD tid = kv.first;
        ULONGLONG time2 = kv.second;
        ULONGLONG time1 = 0;

        auto it = cpu1.find(tid);
        if (it != cpu1.end()) time1 = it->second;

        ULONGLONG delta = (time2 >= time1) ? (time2 - time1) : 0;
        double deltaSec = delta / 10000000.0;
        double cpuPercent = (elapsedSec > 0) ? (deltaSec / (elapsedSec * numCPU)) * 100.0 : 0.0;

        THREAD_STATE st = StateUnknown;
        LONG prio = 0;
        auto itLive = liveInfo.find(tid);
        if (itLive != liveInfo.end()) {
            st = itLive->second.state;
            prio = itLive->second.priority;
        }

        Row r;
        r.tid = tid;
        r.cpuPercent = cpuPercent;
        r.state = st;
        r.priority = prio;
        rows.push_back(r);
    }

    std::sort(rows.begin(), rows.end(),
        [](const Row& a, const Row& b) { return a.cpuPercent > b.cpuPercent; });

    std::cout << "\n=== Danh sach Thread cua PID " << pid << " ===\n";
    std::cout << std::left
        << std::setw(10) << "TID"
        << std::setw(14) << "State"
        << std::setw(10) << "Priority"
        << std::setw(10) << "%CPU" << "\n";
    std::cout << std::string(44, '-') << "\n";

    for (auto& r : rows) {
        std::cout << std::left
            << std::setw(10) << r.tid
            << std::setw(14) << ThreadStateToStr(r.state)
            << std::setw(10) << r.priority
            << std::fixed << std::setprecision(2) << r.cpuPercent << " %"
            << "\n";
    }
}

// ================= MAIN =================
int main() {
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    NtQuerySystemInformation =
        (pNtQuerySystemInformation)GetProcAddress(hNtdll, "NtQuerySystemInformation");
    if (!NtQuerySystemInformation) {
        std::cerr << "Khong the lay dia chi NtQuerySystemInformation\n";
        return 1;
    }

    // Buoc 1: hien danh sach process + %CPU
    ShowProcessListWithCpuUsage();
    /*
    Lấy tất cả process
        ↓
    Đo CPU usage của từng process
        ↓
    In process sử dụng CPU
    */

    // Buoc 2: nguoi dung nhap PID
    DWORD pid;
    std::cout << "\nNhap PID can theo doi thread: ";
    std::cin >> pid;
    /*
    Tìm process có PID được nhập
        ↓
    Lấy danh sách thread của process
        ↓
    Tính CPU usage của từng thread
        ↓
    Lấy state và priority
        ↓
    In kết quả
    */

    // Buoc 3: liet ke thread cua process do kem %CPU
    ListThreadsOfProcess(pid);

    return 0;
}