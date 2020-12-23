#include <iostream>
#include <windows.h>

int main(int argc, TCHAR* argv[])
{
    LPCUWSTR str = L"D:\\Education\\Operation Systems and System Programming\\6 Lab\\ApiHooking\\Debug\\ApiHookingTest.exe";
    char injectDLLName[] = "D:\\Education\\Operation Systems and System Programming\\6 Lab\\ApiHooking\\DebugApiHookingDLL.dll";
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcess(str, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi))
    {
        printf("CreateProcess failed (%d).\n", GetLastError());
        return -1;
    }

    HANDLE Process = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, pi.dwProcessId);
    HMODULE kernelMH = GetModuleHandle(TEXT("kernel32.dll"));
    FARPROC LoadLibraryAddress = GetProcAddress(kernelMH, "LoadLibraryA");

    PVOID rmMemory = VirtualAllocEx(Process, NULL, strlen(injectDLLName) + 1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    WriteProcessMemory(Process, (LPVOID)rmMemory, injectDLLName, strlen(injectDLLName) + 1, NULL);
    HANDLE rmThread = CreateRemoteThread(Process, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryAddress, (LPVOID)rmMemory, NULL, NULL);
    ResumeThread(pi.hThread);

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return 0;
}
