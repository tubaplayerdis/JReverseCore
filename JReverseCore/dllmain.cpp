// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <windows.h>
#include <jni.h>
#include <jvmti.h>
#include <jvmticmlr.h>
#include <thread>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include "PipeManager.h"
#include <boost/interprocess/windows_shared_memory.hpp>
#include "PipeClientAPI.h"



void printBool(bool Bool) {
    if (Bool) {
        std::printf("true");
    }
    else
    {
        std::printf("false");
    }
}

void CheckJNIError(JNIEnv* jniEnv) {
    if (jniEnv->ExceptionCheck()) { jniEnv->ExceptionDescribe(); jniEnv->ExceptionClear(); }
}

static bool JNICALL CustomMethod(jvmtiEnv* jvmti_env, JNIEnv* jni_env, jobject Handle32) {
    bool sus = true;
    return (jboolean)sus;
}

const char* GetJVMTIError(jvmtiError Error) {
    switch (Error)
    {
    case JVMTI_ERROR_NONE:
        return "JVMTI_ERROR_NONE";
    case JVMTI_ERROR_NULL_POINTER:
        return "JVMTI_ERROR_NULL_POINTER";
    case JVMTI_ERROR_OUT_OF_MEMORY:
        return "JVMTI_ERROR_OUT_OF_MEMORY";
    case JVMTI_ERROR_ACCESS_DENIED:
        return "JVMTI_ERROR_ACCESS_DENIED";
    case JVMTI_ERROR_UNATTACHED_THREAD:
        return "JVMTI_ERROR_UNATTACHED_THREAD";
    case JVMTI_ERROR_INVALID_ENVIRONMENT:
        return "JVMTI_ERROR_INVALID_ENVIRONMENT";
    case JVMTI_ERROR_WRONG_PHASE:
        return "JVMTI_ERROR_WRONG_PHASE";
    case JVMTI_ERROR_INTERNAL:
        return "JVMTI_ERROR_INTERNAL";
    case JVMTI_ERROR_INVALID_PRIORITY:
        return "JVMTI_ERROR_INVALID_PRIORITY";
    case JVMTI_ERROR_THREAD_NOT_SUSPENDED:
        return "JVMTI_ERROR_THREAD_NOT_SUSPENDED";
    case JVMTI_ERROR_THREAD_SUSPENDED:
        return "JVMTI_ERROR_THREAD_SUSPENDED";
    case JVMTI_ERROR_THREAD_NOT_ALIVE:
        return "JVMTI_ERROR_THREAD_NOT_ALIVE";
    case JVMTI_ERROR_CLASS_NOT_PREPARED:
        return "JVMTI_ERROR_CLASS_NOT_PREPARED";
    case JVMTI_ERROR_NO_MORE_FRAMES:
        return "JVMTI_ERROR_NO_MORE_FRAMES";
    case JVMTI_ERROR_OPAQUE_FRAME:
        return "JVMTI_ERROR_OPAQUE_FRAME";
    case JVMTI_ERROR_DUPLICATE:
        return "JVMTI_ERROR_DUPLICATE";
    case JVMTI_ERROR_NOT_FOUND:
        return "JVMTI_ERROR_NOT_FOUND";
    case JVMTI_ERROR_NOT_MONITOR_OWNER:
        return "JVMTI_ERROR_NOT_MONITOR_OWNER";
    case JVMTI_ERROR_INTERRUPT:
        return "JVMTI_ERROR_INTERRUPT";
    case JVMTI_ERROR_UNMODIFIABLE_CLASS:
        return "JVMTI_ERROR_UNMODIFIABLE_CLASS";
    case JVMTI_ERROR_NOT_AVAILABLE:
        return "JVMTI_ERROR_NOT_AVAILABLE";
    case JVMTI_ERROR_ABSENT_INFORMATION:
        return "JVMTI_ERROR_ABSENT_INFORMATION";
    case JVMTI_ERROR_INVALID_EVENT_TYPE:
        return "JVMTI_ERROR_INVALID_EVENT_TYPE";
    case JVMTI_ERROR_NATIVE_METHOD:
        return "JVMTI_ERROR_NATIVE_METHOD";
    case JVMTI_ERROR_CLASS_LOADER_UNSUPPORTED:
        return "JVMTI_ERROR_CLASS_LOADER_UNSUPPORTED";
    case JVMTI_ERROR_INVALID_THREAD:
        return "JVMTI_ERROR_INVALID_THREAD";
    case JVMTI_ERROR_INVALID_FIELDID:
        return "JVMTI_ERROR_INVALID_FIELDID";
    case JVMTI_ERROR_INVALID_METHODID:
        return "JVMTI_ERROR_INVALID_METHODID";
    case JVMTI_ERROR_INVALID_LOCATION:
        return "JVMTI_ERROR_INVALID_LOCATION";
    case JVMTI_ERROR_INVALID_OBJECT:
        return "JVMTI_ERROR_INVALID_OBJECT";
    case JVMTI_ERROR_INVALID_CLASS:
        return "JVMTI_ERROR_INVALID_CLASS";
    case JVMTI_ERROR_TYPE_MISMATCH:
        return "JVMTI_ERROR_TYPE_MISMATCH";
    case JVMTI_ERROR_INVALID_SLOT:
        return "JVMTI_ERROR_INVALID_SLOT";
    case JVMTI_ERROR_MUST_POSSESS_CAPABILITY:
        return "JVMTI_ERROR_MUST_POSSESS_CAPABILITY";
    case JVMTI_ERROR_INVALID_THREAD_GROUP:
        return "JVMTI_ERROR_INVALID_THREAD_GROUP";
    case JVMTI_ERROR_INVALID_MONITOR:
        return "JVMTI_ERROR_INVALID_MONITOR";
    case JVMTI_ERROR_ILLEGAL_ARGUMENT:
        return "JVMTI_ERROR_ILLEGAL_ARGUMENT";
    case JVMTI_ERROR_INVALID_TYPESTATE:
        return "JVMTI_ERROR_INVALID_TYPESTATE";
    case JVMTI_ERROR_UNSUPPORTED_VERSION:
        return "JVMTI_ERROR_UNSUPPORTED_VERSION";
    case JVMTI_ERROR_INVALID_CLASS_FORMAT:
        return "JVMTI_ERROR_INVALID_CLASS_FORMAT";
    case JVMTI_ERROR_CIRCULAR_CLASS_DEFINITION:
        return "JVMTI_ERROR_CIRCULAR_CLASS_DEFINITION";
    case JVMTI_ERROR_UNSUPPORTED_REDEFINITION_METHOD_ADDED:
        return "JVMTI_ERROR_UNSUPPORTED_REDEFINITION_METHOD_ADDED";
    case JVMTI_ERROR_UNSUPPORTED_REDEFINITION_SCHEMA_CHANGED:
        return "JVMTI_ERROR_UNSUPPORTED_REDEFINITION_SCHEMA_CHANGED";
    case JVMTI_ERROR_FAILS_VERIFICATION:
        return "JVMTI_ERROR_FAILS_VERIFICATION";
    case JVMTI_ERROR_UNSUPPORTED_REDEFINITION_HIERARCHY_CHANGED:
        return "JVMTI_ERROR_UNSUPPORTED_REDEFINITION_HIERARCHY_CHANGED";
    case JVMTI_ERROR_UNSUPPORTED_REDEFINITION_METHOD_DELETED:
        return "JVMTI_ERROR_UNSUPPORTED_REDEFINITION_METHOD_DELETED";
    case JVMTI_ERROR_NAMES_DONT_MATCH:
        return "JVMTI_ERROR_NAMES_DONT_MATCH";
    case JVMTI_ERROR_UNSUPPORTED_REDEFINITION_CLASS_MODIFIERS_CHANGED:
        return "JVMTI_ERROR_UNSUPPORTED_REDEFINITION_CLASS_MODIFIERS_CHANGED";
    case JVMTI_ERROR_UNSUPPORTED_REDEFINITION_METHOD_MODIFIERS_CHANGED:
        return "JVMTI_ERROR_UNSUPPORTED_REDEFINITION_METHOD_MODIFIERS_CHANGED";
    default:
        return "Undefined Error";
    }
}

bool CheckAndPrintError(jvmtiError error)
{
    if (error != JVMTI_ERROR_NONE) {
        std::printf("\n");
        std::printf(GetJVMTIError(error));
        std::printf("\n");
        return true;
    }
    else
    {
        std::printf("NO ERROR");
        return false;
    }
}


std::vector<std::string> getLoadedClassesSignatures(jvmtiEnv* TIenv, JNIEnv* jniEnv) {
    std::vector<std::string> returnvec;
    jclass* classespointer;
    jint class_count;
    TIenv->GetLoadedClasses(&class_count, &classespointer);

    std::printf("Getting All loaded classes\n\n");
    for (int i = 0; i < class_count; i++) {
        jclass clazz = classespointer[i];
        if (clazz != nullptr) {
            char* name;
            jvmtiError error1 = TIenv->GetClassSignature(clazz, &name, NULL);
            if (error1 != JVMTI_ERROR_NONE) {
                std::printf("Error With Class Signatures\n");
                std::printf(GetJVMTIError(error1));
            }
            else
            {
                std::string namebuild = std::string(name);
                TIenv->Deallocate((unsigned char*)name);
                returnvec.push_back(namebuild);
            }



        }
    }
    return returnvec;
}


void MainThread(HMODULE instance)
{
    std::printf("JReverse Has Been Injected Successfully!\n");
    using jniGetCreatedJavaVMs_t = jint(*)(JavaVM** vmBuf, jsize bufLen, jsize* nVMs);

    
    PipeManager::SetupClient();
    std::printf(PipeManager::ReadString().c_str());
    std::printf("\n");
    
    PipeManager::WriteString("Testing Shared Memory Pipeline! - From Injected Application");

    const auto jvmHandle = GetModuleHandleW(L"jvm.dll");
    if (jvmHandle == nullptr)
    {
        std::printf("[!] Failed to retrieve handle for \"jvm.dll\"!\n");
        return;
    }

    const auto jniGetCreatedJavaVMs = reinterpret_cast<jniGetCreatedJavaVMs_t>(GetProcAddress(
        jvmHandle, "JNI_GetCreatedJavaVMs"));

    JavaVM* javaVm = nullptr;
    jniGetCreatedJavaVMs(&javaVm, 1, nullptr);
    if (javaVm == nullptr)
    {
        std::printf("[!] Failed to retrieve created Java VMs!\n");
        return;
    }

    JNIEnv* jniEnv = nullptr;

    javaVm->AttachCurrentThread(reinterpret_cast<void**>(&jniEnv), nullptr);
    if (jniEnv == nullptr)
    {
        std::printf("[!] Failed to attach to the Java VM.\n");
        return;
    }


    jvmtiEnv* TIenv = nullptr;
    jint res = javaVm->GetEnv((void**)&TIenv, JVMTI_VERSION_1);
    if (res != JNI_OK)
    {
        std::printf("[!] Failed to attach JVMTI interface to the Java VM.\n");
        return;
    }


    static jvmtiCapabilities capa;
    (void)memset(&capa, 0, sizeof(jvmtiCapabilities));
    capa.can_tag_objects = 1;
    capa.can_get_source_file_name = 1;

    jvmtiError erro;
    erro = TIenv->AddCapabilities(&capa);
    if (erro != JVMTI_ERROR_NONE)
    {
        std::printf("[!] Failed to get the jvmti capabilities error below\n");
        std::printf(GetJVMTIError(erro));
        std::printf("\n");
    }


    std::printf("Successfully connected to the Java VM\n");

    std::printf("Connecting to Client Pipe API...\n");

    PipeClientAPI::PrintPipes();

    std::printf("Starting Function Call Loop");

    while (true) {
        while (PipeClientAPI::FunctionPipe.ReadPipe() == PipeClientAPI::noneStr) {
            Sleep(1);
        }
        std::string tobe = PipeClientAPI::ReadFunctionPipeAR();
        if (tobe == "TESTFUNC") {
            PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string>{"Hello", "World"});
        }

    }
    

}

bool __stdcall DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    static FILE* p_file{ nullptr };
    static std::thread main_thread;

    if (reason == DLL_PROCESS_ATTACH)
    {
        AllocConsole();
        freopen_s(&p_file, "CONOUT$", "w", stdout);

        MainThread(instance);
    }
    else if (reason == DLL_PROCESS_DETACH)
    {
        fclose(p_file);
        FreeConsole();
    }

    return true;
}

