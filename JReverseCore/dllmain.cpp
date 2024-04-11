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
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include "PipeManager.h"
#include <boost/interprocess/windows_shared_memory.hpp>
#include "PipeClientAPI.h"
#include "ClassFile.h"
#include "ClassFileManager.h"



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


std::vector<unsigned char> readClassFile(const char* filename) {

    std::cout << "Check existance of file: " << filename << std::endl;

    if (!std::filesystem::exists(filename)) {
        std::cout << "File Does Not Exist With Read Class File!" <<std::endl;
        return {};
    }

    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cout << "Handle Open Error With Read Class File!" << std::endl;
        return {};
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<unsigned char> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        std::cerr << "Error: Unable to read file " << filename << std::endl;
        return {};
    }

    return buffer;
    
    
}


void JNICALL ClassFileLoadHook(jvmtiEnv* jvmti_env, JNIEnv* jni_env, jclass class_being_redefined, jobject loader, const char* name, jobject protection_domain, jint class_data_len, const unsigned char* class_data, jint* new_class_data_len, unsigned char** new_class_data) {
        

    //Verify Data
    if (class_data_len <= 0 || class_data == nullptr) {
        // Invalid arguments, log an error and return
        std::cout << "Invalid Class, Abandoning" << std::endl;
        return;
    }

    if (class_being_redefined == nullptr) {
        std::cout << "New Class Load" << std::endl;
    }
    else {
        std::cout << "Class Redefenition" << std::endl;
    }

    if (name == nullptr) {
        std::cout << "Name is NULL. Sending to Unresolved" << std::endl;
        name = "unknown";
    }

    // Perform actions when a class file is loaded
    std::cout << "Loading Class: " << name << "\n";

    // Print the bytecode of the loaded class
    std::vector<unsigned char> bytecode(class_data, class_data + class_data_len);
    std::stringstream toadd;
    for (unsigned char byte : bytecode) {
        toadd << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }

    ClassFile curfile;
    curfile.bytecodes = toadd.str();
    curfile.classname = name;
    ClassFileManager::AddClassFile(curfile);
}


static jvmtiIterationControl JNICALL
heap_callback(jlong class_tag, jlong size, jlong* tag_ptr, void* user_data) {
    *tag_ptr = 1;
    return JVMTI_ITERATION_CONTINUE;
}

std::vector<std::string> getClassInstances(jvmtiEnv* TIenv, JNIEnv* jniEnv, std::string clazz) {
    std::vector<std::string> GlobalVector;
    jclass cladd = jniEnv->FindClass(clazz.c_str());
    if (cladd == nullptr) {
        return std::vector<std::string> {"Class", "Not Found"};
    }


    jvmtiError iterr = TIenv->IterateOverInstancesOfClass(cladd, JVMTI_HEAP_OBJECT_EITHER, heap_callback, NULL);
    std::printf("Error Code Of Iterate:\n");
    std::printf(GetJVMTIError(iterr));
    std::printf("\n");

    jint count = 0;
    jobject* objs;
    jlong tag = 1;
    jvmtiError err = TIenv->GetObjectsWithTags(1, &tag, &count, &objs, NULL);
    std::printf("Error Code Of GetOBJ:\n");
    bool ifer = CheckAndPrintError(err);
    std::printf("\n");

    std::printf("Class Requested OBJ Count:\n");
    std::printf(std::to_string((int)count).c_str());
    std::printf("\n");

    TIenv->Deallocate((unsigned char*)objs);

    if (count == 0) return std::vector<std::string> {"NO INSTANCES"};

    GlobalVector.push_back(std::to_string(count));
    return GlobalVector;
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

std::vector<std::string> getClassMethods(jvmtiEnv* TIenv, JNIEnv* jniEnv, std::string clazz) {
    std::vector<std::string> GlobalVector;
    jclass cladd = jniEnv->FindClass(clazz.c_str());
    if (cladd == nullptr) {
        return std::vector<std::string> {"Class", "Not Found"};
    }

    jint mcount = 0;
    jmethodID* meths;
    jvmtiError tferr = TIenv->GetClassMethods(cladd, &mcount, &meths);
    std::printf("Error Code Of GetClassMethods:\n");
    std::printf(GetJVMTIError(tferr));
    std::printf("\n");


    std::vector<std::string> StaticVec = {" ","STATIC:"," "};
    std::vector<std::string> NonStaticVec = {" ","NON STATIC:"," "};
    for (int t = 0; t < mcount; t++) {
        jint modifier;
        TIenv->GetMethodModifiers(meths[t], &modifier);
        //Why does this work?
        if ((modifier & 0x0008) != 0) {
            char* nameofMeth;
            char* nameofSig;
            TIenv->GetMethodName(meths[t], &nameofMeth, &nameofSig, NULL);
            std::string nameb = std::string(nameofMeth);
            TIenv->Deallocate((unsigned char*)nameofMeth);
            std::string names = std::string(nameofSig);
            TIenv->Deallocate((unsigned char*)nameofSig);
            StaticVec.push_back(nameb + names);
        }
        else
        {
            char* nameofMeth;
            char* nameofSig;
            TIenv->GetMethodName(meths[t], &nameofMeth, &nameofSig, NULL);
            std::string nameb = std::string(nameofMeth);
            TIenv->Deallocate((unsigned char*)nameofMeth);
            std::string names = std::string(nameofSig);
            TIenv->Deallocate((unsigned char*)nameofSig);
            NonStaticVec.push_back(nameb + names);
        }
    }

    StaticVec.insert(StaticVec.end(), NonStaticVec.begin(), NonStaticVec.end());
    GlobalVector = StaticVec;
    return GlobalVector;
}


std::vector<std::string> getClassFields(jvmtiEnv* TIenv, JNIEnv* jniEnv, std::string clazz) {
    std::vector<std::string> GlobalVector;
    jclass cladd = jniEnv->FindClass(clazz.c_str());
    if (cladd == nullptr) {
        return std::vector<std::string> {"Class", "Not Found"};
    }

    std::printf("Getting class fields of: ");
    std::printf(clazz.c_str());
    std::printf("\n");

    jint fcount = 0;
    jfieldID* feths;
    jvmtiError tterr = TIenv->GetClassFields(cladd, &fcount, &feths);
    std::printf("Error Code Of GetClassFields:\n");
    std::printf(GetJVMTIError(tterr));
    std::printf("\n");


    std::vector<std::string> StaticVec = {" ","STATIC:"," "};
    std::vector<std::string> NonStaticVec = {" ","NON STATIC:"," "};
    for (int t = 0; t < fcount; t++) {
        std::string modi;
        jint modifier;
        TIenv->GetFieldModifiers(cladd,feths[t], &modifier);
        if ((modifier & 0x0008) != 0) 
        {
            char* nameoffeth;
            char* fethsig;
            TIenv->GetFieldName(cladd, feths[t], &nameoffeth, &fethsig, NULL);
            std::string namef = std::string(nameoffeth);
            std::string namev = std::string(fethsig);
            TIenv->Deallocate((unsigned char*)nameoffeth);
            TIenv->Deallocate((unsigned char*)fethsig);
            StaticVec.push_back(namef + " : " + namev);
        }
        else 
        {
            char* nameoffeth;
            char* fethsig;
            TIenv->GetFieldName(cladd, feths[t], &nameoffeth, &fethsig, NULL);
            std::string namef = std::string(nameoffeth);
            std::string namev = std::string(fethsig);
            TIenv->Deallocate((unsigned char*)nameoffeth);
            TIenv->Deallocate((unsigned char*)fethsig);
            NonStaticVec.push_back(namef + " : " + namev);
        }
    }

    StaticVec.insert(StaticVec.end(), NonStaticVec.begin(), NonStaticVec.end());
    GlobalVector = StaticVec;
    return GlobalVector;
}

std::vector<std::string> getMethodBytecodes(jvmtiEnv* TIenv, JNIEnv* jniEnv, std::string clazz, std::string func, std::string sig, std::string statictag) {
    std::vector<std::string> ReturnVector;
    jclass cladd = jniEnv->FindClass(clazz.c_str());
    if (cladd == nullptr) {
        return std::vector<std::string> {"Class", "Not Found"};
    }
    jmethodID methodid;
    if (statictag == "true") {
        methodid = jniEnv->GetStaticMethodID(cladd, func.c_str(), sig.c_str());
    }
    else
    {
        methodid = jniEnv->GetMethodID(cladd, func.c_str(), sig.c_str());
    }
    if (methodid == nullptr) {
        return std::vector<std::string> {"Method", "Not Found"};
    }

    //Get bytecodes
    unsigned char* bytecodes;
    jint bytecode_length;
    jvmtiError error = TIenv->GetBytecodes(methodid, &bytecode_length, &bytecodes);
    if (error != JVMTI_ERROR_NONE) {
        std::cout << "Error Getting Bytecodes:" << error << "\n";
        return std::vector<std::string> { "ByteCodes", "Cant Get" };
    }

    std::cout << "Getting Bytecodes of: " << func;

    std::stringstream bytecodeString;
    for (int i = 0; i < bytecode_length; i++) {
        bytecodeString << std::hex << std::stoi(std::to_string(bytecodes[i]));
        //bytecodeString << " ";
    }
    

    TIenv->Deallocate((unsigned char*)bytecodes);

    return std::vector<std::string> {bytecodeString.str()};
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
    capa.can_get_bytecodes = 1;
    capa.can_retransform_classes = 1;
    capa.can_generate_all_class_hook_events = 1;

    jvmtiError erro;
    erro = TIenv->AddCapabilities(&capa);
    if (erro != JVMTI_ERROR_NONE)
    {
        std::printf("[!] Failed to get the jvmti capabilities error below\n");
        std::printf(GetJVMTIError(erro));
        std::printf("\n");
    }

    ClassFileManager::init();

    //For class file hooks
    // Set event callbacks
    jvmtiEventCallbacks callbacks;
    (void)memset(&callbacks, 0, sizeof(callbacks));
    callbacks.ClassFileLoadHook = &ClassFileLoadHook;
    jvmtiError error = TIenv->SetEventCallbacks(&callbacks, sizeof(callbacks));
    if (error != JVMTI_ERROR_NONE) {
        std::cerr << "Error setting event callbacks" << std::endl;
        return;
    }

    // Enable necessary events
    jvmtiError errort = TIenv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_CLASS_FILE_LOAD_HOOK, nullptr);
    if (errort != JVMTI_ERROR_NONE) {
        std::cerr << "Error enabling event notifications" << std::endl;
        return;
    }

    std::printf("Enabeld Class file load hooks\n");


    std::printf("Successfully connected to the Java VM\n");

    std::printf("Connecting to Client Pipe API...\n");

    PipeClientAPI::PrintPipes();

    std::printf("Starting Function Call Loop\n");

    while (true) {
        while (true) {            
            if (!PipeClientAPI::isFunctionPipeNone()) {
                std::printf("Function Call Detected!\n");
                break;
            }
            Sleep(10);
        }
        std::printf("Looping Func\n");
        std::string called = PipeClientAPI::ReadFunctionPipeAR();
        std::vector<std::string> args = PipeClientAPI::ReadFunctionArgPipeAR();
        if (called == "TESTFUNC") {
            PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string>{"Hello", "World"});
        }
        else if (called == "getLoadedClasses") {
            std::vector<std::string> sus = getLoadedClassesSignatures(TIenv, jniEnv);
            if (sus.size() * sizeof(std::string) > PipeClientAPI::ReturnPipe.GetInfo().Size) {
                PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string>{"ERROR: ", "DATA TOO LARGE - ADJUST IN PIPE MANAGER", "SIZE OF DATA,PIPE", std::to_string((int)(sus.size() * sizeof(std::string))), std::to_string(PipeClientAPI::ReturnPipe.GetInfo().Size)});
            }
            else
            {
                PipeClientAPI::ReturnPipe.WritePipe(sus);
            }

        }
        else if (called == "getClassMethods") {
            std::printf("Getting Class Methods\n");
            PipeClientAPI::ReturnPipe.WritePipe(getClassMethods(TIenv, jniEnv, args[0]));
        }
        else if (called == "getClassFields") {
            std::printf("Getting Class Fields\n");
            PipeClientAPI::ReturnPipe.WritePipe(getClassFields(TIenv, jniEnv, args[0]));
        }
        else if (called == "getClassInstances") {
            std::printf("Getting Class Instacnes\n");
            PipeClientAPI::ReturnPipe.WritePipe(getClassInstances(TIenv, jniEnv, args[0]));
            std::printf("Returned Class Instacnes\n");
        }
        else if (called == "getMethodBytecodes") {
            std::printf("Getting Class Fields\n");
            PipeClientAPI::ReturnPipe.WritePipe(getMethodBytecodes(TIenv, jniEnv, args[0], args[1], args[2], args[3]));
        }
        else if (called == "getClassBytecodes") {
            std::printf("Getting Class Bytecodes");
            ClassFile found = ClassFileManager::FindClassFile(args[0]);
            if (found.classname == "NOT FOUND") {
                PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string>{"Class File Not Found", "Class File Not Found"});
            }
            else
            {
                PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string> {found.classname, found.bytecodes});
            }
        }
        else if (called == "getUnknownClassFiles") {
            PipeClientAPI::ReturnPipe.WritePipe(ClassFileManager::GetUnknownClassFiles());
        }
        else if (called == "setUnknownClassFiles") {
            std::printf("Deleting Unknown Classes\n");
            ClassFileManager::DeleteUnknownClassFiles();
            std::printf("Setting Unresolved Bytecodes\n");
            for (size_t i = 0; i < args.size(); i += 2) {
                ClassFile sendof;
                sendof.classname = args[i];
                sendof.bytecodes = args[i + 1];
                std::cout << "Resolved Class: " << sendof.classname << std::endl;
                ClassFileManager::AddClassFile(sendof);
            }
            PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string>{"Sucsessfull!"});
        }
        else if (called == "getClassFileNames") {
            PipeClientAPI::ReturnPipe.WritePipe(ClassFileManager::GetClassFileNames());
        }
        else if (called == "setupScriptingEnviroment") {
            
            //Load the Scripting Core
            std::vector<unsigned char> jvec = readClassFile(args[0].c_str());            
            
            const jbyte* classdata = reinterpret_cast<const jbyte*>(jvec.data());

            if (jvec.empty()) {
                std::cout << "File Read Data is Empty!" << std::endl;
            }

            jclass ScriptingCore = jniEnv->DefineClass("com/jreverse/jreverse/Core/JReverseScriptingCore", NULL, classdata, jvec.size());

            CheckJNIError(jniEnv);

            if (ScriptingCore == nullptr) {
                PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string> {"Failed To Make Class"});
            }
            else
            {             
                //Run Main Method
                jmethodID mainMethod = jniEnv->GetStaticMethodID(ScriptingCore, "Main", "()I");

                jint result = jniEnv->CallStaticIntMethod(ScriptingCore, mainMethod);

                std::cout << "Result of Main Method: " << result << std::endl;

                PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string> {"Created Scripting Class and ran Main with 0"});
            }



        }
        else if (called == "runScript")
        {
            jclass ScriptingCore = jniEnv->FindClass("com/jreverse/jreverse/Core/JReverseScriptingCore");
            if (ScriptingCore == nullptr) {
                PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string> {"Scripting Core Class Not Initalized!"});
            }
            else
            {
                
                std::cout << "Running RunScript Now!" << std::endl;
                jmethodID RunScriptMethod = jniEnv->GetStaticMethodID(ScriptingCore, "RunScript", "(Ljava/lang/String;)Ljava/lang/String;");
                jstring ScriptPath = jniEnv->NewStringUTF(args[0].c_str());
                if (RunScriptMethod != nullptr) {
                    jstring ScriptRes = (jstring)jniEnv->CallStaticObjectMethod(ScriptingCore, RunScriptMethod, ScriptPath);
                    std::cout << "Executed Script!" << std::endl;

                    std::string res;
                    if (ScriptRes == nullptr) res = "Empty Script Return!";
                    else
                    {                        
                        res = jniEnv->GetStringUTFChars(ScriptRes, nullptr);                       
                    }                                          
                    PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string> {res});
                }
                else
                {
                    PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string> {"Major Error!"});
                }                                       
                
            }
            
        }
        else if (called == "uninjectCore") {
            std::cout << "uninjectingJReverse" << std::endl;
            break;
        }
        else {
            PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string>{"Function", "Non Existent"});
        }
    }
    

}

bool __stdcall DllMain(HINSTANCE hinstance, DWORD reason, LPVOID reserved)
{
    static FILE* p_file{ nullptr };
    static std::thread main_thread;

    if (reason == DLL_PROCESS_ATTACH)
    {
        AllocConsole();
        freopen_s(&p_file, "CONOUT$", "w", stdout);

        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MainThread, hinstance, 0, nullptr);
    }
    else if (reason == DLL_PROCESS_DETACH)
    {
        fclose(p_file);
        FreeConsole();
    }

    return true;
}

