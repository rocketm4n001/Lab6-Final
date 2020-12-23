#include "pch.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <iostream>
#include <detours.h>
#include <string>
#include <fstream>

using namespace std;

string getPathName(const string& s) {

    char sep = '/';

#ifdef _WIN32
    sep = '\\';
#endif

    size_t i = s.rfind(sep, s.length());
    if (i != string::npos) {
        return(s.substr(0, i));
    }

    return("");
}

std::ofstream out;

HANDLE(WINAPI* CreateFilePointer) (LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) = CreateFile;

HFILE(WINAPI* OpenFilePointer) (LPCSTR lpFileName, LPOFSTRUCT lpReOpenBuff, UINT uStyle) = OpenFile;

BOOL(WINAPI* ReadFilePointer) (HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped) = ReadFile;

BOOL(WINAPI* WriteFilePointer) (HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped) = WriteFile;

LSTATUS(WINAPI* RegGetValuePointer) (HKEY hkey, LPCSTR lpSubKey, LPCSTR lpValue, DWORD dwFlags, LPDWORD pdwType, PVOID pvData, LPDWORD pcbData) = RegGetValueA;

LSTATUS(WINAPI* RegSetValuePointer) (HKEY hKey, LPCSTR lpSubKey, DWORD dwType, LPCSTR lpData, DWORD cbData) = RegSetValueA;

LONG(WINAPI* RegOpenKeyPointer) (HKEY hKey, LPCTSTR lpSubKey, PHKEY phkResult) = RegOpenKey;

LSTATUS(WINAPI* RegCloseKeyPointer) (HKEY hKey) = RegCloseKey;



HANDLE WINAPI HookedCreateFile (LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
    if (out.is_open())
    {
        out << "create file called!\n" << std::endl;
    }

    return CreateFilePointer(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

HFILE WINAPI HookedOpenFile (LPCSTR lpFileName, LPOFSTRUCT lpReOpenBuff, UINT uStyle) {
    if (out.is_open())
    {
        out << "open file called!\n" << std::endl;
    }

    return OpenFilePointer(lpFileName, lpReOpenBuff, uStyle);
}

BOOL WINAPI HookedReadFile (HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped) {
    if (out.is_open())
    {
        out << "read file called!\n" << std::endl;
    }

    return ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
}

BOOL WINAPI HookedWriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped) {
    if (out.is_open())
    {
        out << "write file called!\n" << std::endl;
        out.flush();
    }

    return WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
}

LSTATUS WINAPI HookedRegGetValue(HKEY hkey, LPCSTR lpSubKey, LPCSTR lpValue, DWORD dwFlags, LPDWORD pdwType, PVOID pvData, LPDWORD pcbData) {
    if (out.is_open())
    {
        out << "RegGetValue called!\n" << std::endl;
    }

    return RegGetValueA(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
}

LSTATUS WINAPI HookedRegSetValue(HKEY hKey, LPCSTR lpSubKey, DWORD dwType, LPCSTR lpData, DWORD cbData) {
    if (out.is_open())
    {
        out << "RegSetValue called!\n" << std::endl;
    }

    return RegSetValueA(hKey, lpSubKey, dwType, lpData, cbData);
}

LONG WINAPI HookedRegOpenKey (HKEY hKey, LPCTSTR lpSubKey, PHKEY phkResult) {
    if (out.is_open())
    {
        out << "RegOpenKey called!\n" << std::endl;
    }

    return RegOpenKey(hKey, lpSubKey, phkResult);
}

LSTATUS WINAPI HookedRegCloseKey (HKEY hKey) {
    if (out.is_open())
    {
        out << "RegCloseKey called!\n" << std::endl;
    }

    return RegCloseKey(hKey);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        char str[255];
        GetModuleFileNameA(hModule, str, 255);
        printf((string(str) + string("\\log.txt")).c_str());
                  
        out.open((string(getPathName(string(str))) + string("\\log.txt")).c_str());
        
        DisableThreadLibraryCalls(hModule);
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)CreateFilePointer, HookedCreateFile);
        DetourAttach(&(PVOID&)OpenFilePointer, HookedOpenFile);
        DetourAttach(&(PVOID&)ReadFilePointer, HookedReadFile);
        //DetourAttach(&(PVOID&)WriteFilePointer, HookedWriteFile);
        DetourAttach(&(PVOID&)RegGetValuePointer, HookedRegGetValue);
        DetourAttach(&(PVOID&)RegSetValuePointer, HookedRegSetValue);
        DetourAttach(&(PVOID&)RegOpenKeyPointer, HookedRegOpenKey);
        DetourAttach(&(PVOID&)RegCloseKeyPointer, HookedRegCloseKey);
        DetourTransactionCommit();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}



