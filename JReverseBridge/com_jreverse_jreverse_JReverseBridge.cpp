#include "com_jreverse_jreverse_JReverseBridge.h"
#include <windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <namedpipeapi.h>
#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include "SharedMemManager.h"

bool Inject(DWORD pId, char* dllName)
{
    HANDLE h = OpenProcess(PROCESS_ALL_ACCESS, false, pId);
    if (h)
    {
        LPVOID LoadLibAddr = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
        LPVOID dereercomp = VirtualAllocEx(h, NULL, strlen(dllName), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        WriteProcessMemory(h, dereercomp, dllName, strlen(dllName), NULL);
        HANDLE asdc = CreateRemoteThread(h, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddr, dereercomp, 0, NULL);
        WaitForSingleObject(asdc, INFINITE);
        VirtualFreeEx(h, dereercomp, strlen(dllName), MEM_RELEASE);
        CloseHandle(asdc);
        CloseHandle(h);
        return true;
    }
    return false;
}

// Function to get the base address of a module in a specified process
DWORD GetModuleBaseAddress(DWORD pid, const char* moduleName) {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    if (hSnap == INVALID_HANDLE_VALUE) {
        std::cerr << "createSnapshot failed\n";
        return 0;
    }

    MODULEENTRY32 me32;
    me32.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(hSnap, &me32)) {
        do {
            if (strcmp(moduleName, (const char*)me32.szModule) == 0) {
                CloseHandle(hSnap);
                return (DWORD)me32.modBaseAddr;
            }
        } while (Module32Next(hSnap, &me32));
    }

    CloseHandle(hSnap);
    std::cerr << "no such module found in PID=" << pid << "\n";
    return 0;
}

void CallFunctionOnInjectedDLL(const wchar_t* dllPath, const char* dllName, const char* FunctionToCall, DWORD processPID) {
    
    HMODULE hOurModule = LoadLibrary(dllPath);
    if (!hOurModule) {
        std::cerr << "Failed to get hOurModule\n";
        return;
    }

    FARPROC FunctionAddress = GetProcAddress(hOurModule, FunctionToCall);
    if (!FunctionAddress) {
        std::cerr << "Failed to get FunctionAddress\n";
        return;
    }

    DWORD offset = (DWORD)((char*)FunctionAddress - (char*)hOurModule);

    DWORD pInjectedFunction = (DWORD)GetModuleBaseAddress(processPID, dllName) + offset;
    if (!pInjectedFunction) {
        std::cerr << "Failed to get pInjectedFunction\n";
        return;
    }

    HANDLE hInjectedProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processPID);
    if (!hInjectedProcess) {
        std::cerr << "Failed to get hInjectedProcess\n";
        return;
    }

    HANDLE hInvokingThread = CreateRemoteThread(hInjectedProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pInjectedFunction, NULL, 0, NULL);
    if (!hInvokingThread) {
        std::cerr << "Failed to get hInvokingThread\n";
        return;
    }
    
}


JNIEXPORT jboolean JNICALL Java_com_jreverse_jreverse_Bridge_JReverseBridge_testMethod(JNIEnv*, jclass)
{
	return jboolean(true);
}

JNIEXPORT jint JNICALL Java_com_jreverse_jreverse_Bridge_JReverseBridge_InjectDLL(JNIEnv* jniEnv, jclass, jint PID, jstring Location)
{
    DWORD pid = (DWORD)PID;
    char* location = (char*)jniEnv->GetStringUTFChars(Location, 0);
    bool InjectCode = Inject(pid, location);
    jniEnv->ReleaseStringUTFChars(Location, location);
    if (InjectCode == false) { return (jint)1; }
    else { return (jint)0; }
}

JNIEXPORT void JNICALL Java_com_jreverse_jreverse_Bridge_JReverseBridge_SetupPipe(JNIEnv*, jclass)
{
    SharedMemManager::Setup();
    SharedMemManager::WriteString("Testing Shared Memory Pipeline! - From JReverse");
    return;
}

JNIEXPORT void JNICALL Java_com_jreverse_jreverse_Bridge_JReverseBridge_PrimeLoadedClasses(JNIEnv*, jclass)
{
    return;
}

JNIEXPORT jstring JNICALL Java_com_jreverse_jreverse_Bridge_JReverseBridge_GetStringPipe(JNIEnv* env, jclass)
{
    return env->NewStringUTF(SharedMemManager::ReadString().c_str());
}

JNIEXPORT void JNICALL Java_com_jreverse_jreverse_Bridge_JReverseBridge_WriteStringPipe(JNIEnv* env, jclass, jstring data)
{
    const char* cstr = env->GetStringUTFChars(data, NULL);
    std::string str = std::string(cstr);
    env->ReleaseStringUTFChars(data, str.c_str());
    SharedMemManager::WriteString(cstr);
}







