#include "com_jreverse_jreverse_JReverseBridge.h"
#include <windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <codecvt> // For std::wstring_convert and std::codecvt_utf8_utf16
#include <locale> // For std::wstring
#include <namedpipeapi.h>
#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <WinUser.h>
#include <dwmapi.h>
#include "SharedMemManager.h"
#include "PipeAPI.h"
#include "JReverseLogger.h"

void SetWindowBobColor(HWND hwnd)
{
    BOOL USE_DARK_MODE = true;
    //DwmSetWindowAttribute(hwnd, DWMWINDOWATTRIBUTE::DWMWA_USE_IMMERSIVE_DARK_MODE,&USE_DARK_MODE, sizeof(USE_DARK_MODE));
}


bool InjectOnStartup(char* dllName, std::wstring apppath)
{
    STARTUPINFO startupInfo = { sizeof(startupInfo) };
    PROCESS_INFORMATION processInfo;
    CreateProcessW(apppath.c_str(), NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &startupInfo, &processInfo);

    HANDLE h = OpenProcess(PROCESS_ALL_ACCESS, false, processInfo.dwProcessId);
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

        //Resume Process
        ResumeThread(processInfo.hThread);

        // Close process and thread handles
        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);
        return true;
    }
    return false;
}


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


JNIEXPORT jboolean JNICALL Java_com_jreverse_jreverse_Bridge_JReverseBridge_testMethod(JNIEnv* env, jclass)
{
    return true;
}

JNIEXPORT void JNICALL Java_com_jreverse_jreverse_Bridge_JReverseBridge_InitBridge(JNIEnv* env, jclass)
{
    JReverseLogger logger = JReverseLogger(env);
    logger.Log("Initalizing Bridge...");
    HWND sus = FindWindowA(NULL, "JReverse");
    SetWindowBobColor(sus);
}

std::wstring to_wstring(const std::string& stringToConvert)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, stringToConvert.c_str(), static_cast<int>(stringToConvert.length()), NULL, 0);
    std::wstring wide_str(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, stringToConvert.c_str(), static_cast<int>(stringToConvert.length()), &wide_str[0], size_needed);
    return wide_str;
}

JNIEXPORT jint JNICALL Java_com_jreverse_jreverse_Bridge_JReverseBridge_StartAndInjectDLL(JNIEnv* env, jclass, jstring Location, jstring app)
{
    char* location = (char*)env->GetStringUTFChars(Location, 0);
    std::string apppath = env->GetStringUTFChars(app, 0);
    bool InjectCode = InjectOnStartup(location, to_wstring(apppath));
    //env->ReleaseStringUTFChars(Location, location);
    //env->ReleaseStringUTFChars(app, apppath.c_str());
    if (InjectCode == false) { return (jint)1; }
    else { return (jint)0; }
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

JNIEXPORT jint JNICALL Java_com_jreverse_jreverse_Bridge_JReverseBridge_WriteStartupPipe(JNIEnv* env, jclass, jobjectArray rulesArray, jobject settings)
{
    PipeAPI::CommunicationPipe.WritePipe("NONE");
    jclass RuleClass = env->FindClass("com/jreverse/jreverse/StartupRule");
    jfieldID RuleNameID = env->GetFieldID(RuleClass, "ClassName", "Ljava/lang/String;");
    jfieldID RuleBytecodesID = env->GetFieldID(RuleClass, "ByteCodes", "Ljava/lang/String;");
    jfieldID RuleIsBypassID = env->GetFieldID(RuleClass, "isBypass", "Z");
    jsize ArraySize = env->GetArrayLength(rulesArray);

    JReverseLogger logger = JReverseLogger(env);
    logger.Log("Got Field ID's");

    std::vector<std::string> retunable = std::vector<std::string>{"NO ARGS"};

    bool DoRules = true;

    if (ArraySize < 1) {
        PipeAPI::StartupRulesPipe.WritePipe(retunable);
        DoRules = false;
        logger.Log("Ingoring Rules!");
    }

    retunable.clear();

    if (DoRules) {
        logger.Log("Getting Field Values for startup");
        for (int i = 0; i < ArraySize; i++) {
            jobject element = env->GetObjectArrayElement(rulesArray, i);
            jstring jrulename = (jstring)env->GetObjectField(element, RuleNameID);
            jstring jrulebytecodes = (jstring)env->GetObjectField(element, RuleBytecodesID);
            jboolean jruleisbypass = env->GetBooleanField(element, RuleIsBypassID);
            std::string rulename = env->GetStringUTFChars(jrulename, NULL);
            std::string rulebytecodes = env->GetStringUTFChars(jrulebytecodes, NULL);
            std::string ruleisbypass = "false";
            if ((bool)jruleisbypass) ruleisbypass = "true";
            retunable.push_back(rulename);
            retunable.push_back(rulebytecodes);
            retunable.push_back(ruleisbypass);
        }

        PipeAPI::StartupRulesPipe.WritePipe(retunable);

        logger.Log("Wrote Startup Rules!");
    }

    jclass StartupSettingsClass = env->FindClass("com/jreverse/jreverse/StartupSettings");
    //Auto start and inject is ignored as it is not neccecary
    jfieldID IsClassFileLoadMessagesField = env->GetFieldID(StartupSettingsClass, "IsClassFileLoadMessages", "Z");
    jfieldID IsClassFileCollectionField = env->GetFieldID(StartupSettingsClass, "IsClassFileCollection", "Z");
    jfieldID IsConsoleWindowField = env->GetFieldID(StartupSettingsClass, "IsConsoleWindow", "Z");
    jfieldID FuncLoopTimeoutField = env->GetFieldID(StartupSettingsClass, "FuncLoopTimeout", "I");
    jfieldID JNIEnvTimeoutField = env->GetFieldID(StartupSettingsClass, "JNIEnvTimeout", "I");
    jfieldID IsDynamicClassFileCollectionField = env->GetFieldID(StartupSettingsClass, "IsDynamicClassFileCollection", "Z");
    jfieldID DynamicClassFileCollectionPathField = env->GetFieldID(StartupSettingsClass, "DynamicClassFileCollectionPath", "Ljava/lang/String;");

    //Instacne vars due to IPC problems
    std::string IsClassFileCollection = std::to_string(env->GetBooleanField(settings, IsClassFileCollectionField));
    std::string IsClassFileLoadMessages = std::to_string(env->GetBooleanField(settings, IsClassFileLoadMessagesField));
    std::string IsConsoleWindow = std::to_string(env->GetBooleanField(settings, IsConsoleWindowField));
    std::string FuncLoopTimeout = std::to_string(env->GetIntField(settings, FuncLoopTimeoutField));
    std::string JNIEnvTimeout = std::to_string(env->GetIntField(settings, JNIEnvTimeoutField));
    std::string IsDynamicClassFileCollection = std::to_string(env->GetBooleanField(settings, DynamicClassFileCollectionPathField));
    std::string DynamicClassFileCollectionPath = env->GetStringUTFChars((jstring)env->GetObjectField(settings, DynamicClassFileCollectionPathField), NULL);

    logger.Log("Startup Settings Being Sent:");
    logger.Log(IsClassFileCollection);
    logger.Log(IsClassFileLoadMessages);
    logger.Log(IsConsoleWindow);
    logger.Log(FuncLoopTimeout);
    logger.Log(JNIEnvTimeout);

    PipeAPI::SettingsPipe.WritePipe(std::vector<std::string>{IsClassFileCollection, IsClassFileLoadMessages, IsConsoleWindow, FuncLoopTimeout, JNIEnvTimeout, IsDynamicClassFileCollection, DynamicClassFileCollectionPath});

    logger.Log("Wrote Settings to Pipe!");

    return 0;
}

JNIEXPORT void JNICALL Java_com_jreverse_jreverse_Bridge_JReverseBridge_SetupPipe(JNIEnv*, jclass)
{
    SharedMemManager::Setup();
    SharedMemManager::WriteString("Testing Shared Memory Pipeline! - From JReverse");
    return;
}

JNIEXPORT void JNICALL Java_com_jreverse_jreverse_Bridge_JReverseBridge_PrimeLoadedClasses(JNIEnv*, jclass)
{
    //JReversePipe<int> mypipe = JReversePipe<int>("MyPipe", boost::interprocess::read_write, 1000);
    //mypipe.WritePipe(345);
    //int noway = mypipe.ReadPipe();
    //should be 345!
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

JNIEXPORT jobjectArray JNICALL Java_com_jreverse_jreverse_Bridge_JReverseBridge_CallCoreFunction(JNIEnv* env, jclass, jstring name, jobjectArray args)
{
    JReverseLogger logger = JReverseLogger(env);
    //Clear Return Pipe
    logger.Log("Clearing Return Pipe");
    PipeAPI::ReturnPipe.WritePipe(std::vector<std::string>{"NONE"});

    logger.Log("Starting Function Call");
    //Args
    std::vector<std::string> topass;
    jsize argslen = env->GetArrayLength(args);

    for (int i = 0; i < argslen; i++) {
        jstring string = (jstring)env->GetObjectArrayElement(args, i);
        std::string check = env->GetStringUTFChars(string, NULL);
        if (check == "NONE") {
            topass = PipeAPI::noneVec;
            break;
        }
        topass.push_back(check);
        //env->ReleaseStringUTFChars(string, check.c_str());
    }

    logger.Log("Writing args...");

    PipeAPI::FunctionArgPipe.WritePipe(topass);

    logger.Log("Wrote args to pipe");

    //Function
    std::string tocall = env->GetStringUTFChars(name, NULL);
    PipeAPI::FunctionPipe.WritePipe(tocall);

    logger.Log("Wrote function to pipe");

    logger.Log("Starting Wait For Callback");
    logger.Log("Waiting On Callback");
    int z = 0;
    while (true)
    {
        if (z > logger.GetPipeCallBackLimit()) {
            PipeAPI::ReturnPipe.WritePipe(std::vector<std::string>{"Error: Callback Timed Out", "Error: Callback Timed Out", "Error: Callback Timed Out", "Error: Callback Timed Out"});
            break;
        }
        if (!PipeAPI::isReturnPipeNone()) break;
        logger.RemoveAndLog("Waiting On Callback (" + std::to_string(z) + ")");
        Sleep(10);
        z++;
    }

    logger.Log("Callback Recived!");

    std::vector<std::string> builda = PipeAPI::ReadReturnPipeAR();

    // Find the class of java.lang.String
    jclass stringClass = env->FindClass("java/lang/String");
    jobjectArray stringArray = env->NewObjectArray(builda.size(), stringClass, NULL);


    for (int i = 0; i < builda.size(); i++) {
        jstring string1 = env->NewStringUTF(builda[i].c_str());
        env->SetObjectArrayElement(stringArray, i, string1);
    }

    return stringArray;
}








