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
#include <chrono>
#include "PipeManager.h"
#include <boost/interprocess/windows_shared_memory.hpp>
#include "PipeClientAPI.h"
#include "ClassFile.h"
#include "ClassFileManager.h"
#include "datastore.h"
#include "com_jreverse_jreverse_Core_JReverseScriptingCore.h"



void printBool(bool Bool) {
    if (Bool) {
        std::printf("true");
    }
    else
    {
        std::printf("false");
    }
}

const char* CheckJNIError(JNIEnv* jniEnv) {
    jthrowable ex = nullptr;
    if (jniEnv->ExceptionCheck()) { std::cout << "A JVM EXEPTION WAS FOUND!" << std::endl; ex = jniEnv->ExceptionOccurred(); }
    if (ex == nullptr) return "No Error";
    
    jclass exceptionClass = jniEnv->GetObjectClass(ex);

    // Get the method ID of the getMessage() method of Throwable
    jmethodID getMessageMethod = jniEnv->GetMethodID(exceptionClass, "getMessage", "()Ljava/lang/String;");

    // Call getMessage() method
    jstring messageObj = (jstring)jniEnv->CallObjectMethod(ex, getMessageMethod);

    // Convert messageObj to C string
    const char* message = jniEnv->GetStringUTFChars(messageObj, nullptr);
    std::cout << "Exception message: " << message << std::endl;

    // Create a StringWriter to capture the stack trace
    jclass stringWriterClass = jniEnv->FindClass("java/io/StringWriter");
    jmethodID stringWriterConstructor = jniEnv->GetMethodID(stringWriterClass, "<init>", "()V");
    jobject stringWriterObject = jniEnv->NewObject(stringWriterClass, stringWriterConstructor);
    jmethodID stringWriterMethod = jniEnv->GetMethodID(stringWriterClass, "toString", "()Ljava/lang/String;");

    // Create a PrintWriter to write to the StringWriter
    jclass printWriterClass = jniEnv->FindClass("java/io/PrintWriter");
    jmethodID printWriterConstructor = jniEnv->GetMethodID(printWriterClass, "<init>", "(Ljava/io/Writer;)V");
    jobject printWriterObject = jniEnv->NewObject(printWriterClass, printWriterConstructor, stringWriterObject);

    // Print the stack trace to the PrintWriter
    jclass throwableClass = jniEnv->FindClass("java/lang/Throwable");
    jmethodID printStackTraceMethod = jniEnv->GetMethodID(throwableClass, "printStackTrace", "(Ljava/io/PrintWriter;)V");
    jniEnv->CallVoidMethod(ex, printStackTraceMethod, printWriterObject);

    // Get the string representation from the StringWriter
    jstring stackTraceString = (jstring)jniEnv->CallObjectMethod(stringWriterObject, stringWriterMethod);

    // Convert the Java string to C-style string
    const char* stackTraceCString = jniEnv->GetStringUTFChars(stackTraceString, NULL);

    // Print the stack trace string to std::cout
    std::cout << "Exception stack trace:\n" << stackTraceCString << std::endl;

    // Release the allocated memory
    jniEnv->ReleaseStringUTFChars(stackTraceString, stackTraceCString);

    // Release the message string
    jniEnv->ReleaseStringUTFChars(messageObj, message);

    jniEnv->ExceptionDescribe();
    jniEnv->ExceptionClear();
    return message;
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

void add_path(JNIEnv* env, const std::string& path)
{
    const std::string urlPath = "file:/"+path;
    jclass classLoaderCls = env->FindClass("java/lang/ClassLoader");
    jmethodID getSystemClassLoaderMethod = env->GetStaticMethodID(classLoaderCls, "getSystemClassLoader", "()Ljava/lang/ClassLoader;");
    jobject classLoaderInstance = env->CallStaticObjectMethod(classLoaderCls, getSystemClassLoaderMethod);
    jclass urlClassLoaderCls = env->FindClass("java/net/URLClassLoader");
    jmethodID addUrlMethod = env->GetMethodID(urlClassLoaderCls, "addURL", "(Ljava/net/URL;)V");
    jclass urlCls = env->FindClass("java/net/URL");
    jmethodID urlConstructor = env->GetMethodID(urlCls, "<init>", "(Ljava/lang/String;)V");
    jobject urlInstance = env->NewObject(urlCls, urlConstructor, env->NewStringUTF(urlPath.c_str()));
    env->CallVoidMethod(classLoaderInstance, addUrlMethod, urlInstance);
    std::cout << "Added " << urlPath << " to the classpath." << std::endl;
}


void MainThread(HMODULE instance)
{
    std::printf("JReverse Has Been Injected Successfully!\n");
    std::cerr << "Testing STD:ERR stream" << std::endl;
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

    JReverseStore::jvmtienv = TIenv;


    std::printf("Successfully connected to the Java VM\n");

    std::printf("Connecting to Client Pipe API...\n");

    PipeClientAPI::PrintPipes();

    std::printf("Starting Function Call Loop\n");

    //Create Scripting StringWriter!
    jclass stringWriterClass = jniEnv->FindClass("java/io/StringWriter");
    jmethodID StringWriterconstructor = jniEnv->GetMethodID(stringWriterClass, "<init>", "()V");
    jmethodID toStringMethod = jniEnv->GetMethodID(stringWriterClass, "toString", "()Ljava/lang/String;");
    jmethodID closeWriterMethod = jniEnv->GetMethodID(stringWriterClass, "close", "()V");
    jmethodID flushWriterMethod = jniEnv->GetMethodID(stringWriterClass, "flush", "()V");
    jmethodID writeWriterMethod = jniEnv->GetMethodID(stringWriterClass, "write", "(Ljava/lang/String;)V");
    jobject stringWriterObj = jniEnv->NewObject(stringWriterClass, StringWriterconstructor);


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
            //Seutp python
            std::cout << "Seting up python!" << std::endl;

            if (!std::filesystem::exists(args[1].c_str())) std::cout << "The Jar file does not exist!" << std::endl;

            //This works. Thanks Stack Overflow!
            add_path(jniEnv,args[1]);

            std::cout << "Checking Errors" << std::endl;
            CheckJNIError(jniEnv);


            jclass pythonInterpreterClass = jniEnv->FindClass("org/python/util/PythonInterpreter");
            if (pythonInterpreterClass == nullptr) {
                PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string> {"1", "The Python Interter class was not found. Aborting Setup..."});
            }
            else {
                //Load the Scripting Core
                std::vector<unsigned char> jvec = readClassFile(args[0].c_str());

                const jbyte* classdata = reinterpret_cast<const jbyte*>(jvec.data());

                if (jvec.empty()) {
                    std::cout << "File Read Data is Empty!" << std::endl;
                }

                jclass ScriptingCore = jniEnv->DefineClass("com/jreverse/jreverse/Core/JReverseScriptingCore", NULL, classdata, jvec.size());

                const char* er = CheckJNIError(jniEnv);

                if (ScriptingCore == nullptr) std::cout << "Scripting Core was NULL" << std::endl;


                std::printf("Getting Thread!\n");
                std::thread::id this_id = std::this_thread::get_id();
                std::cout << "DLL main thread: " << this_id << std::endl;
                std::cout << "Starting Native Registration!" << std::endl;


                setGlobalEnv(jniEnv, TIenv);

                char MethodName[18] = "GetClassInstances";
                char MethodSig[40] = "(Ljava/lang/String;)[Ljava/lang/Object;";
                JNINativeMethod methods[] = {
                    { MethodName, MethodSig, (void*)&Java_com_jreverse_jreverse_Core_JReverseScriptingCore_GetClassInstances }
                };

                // Register the native methods with the class
                if (jniEnv->RegisterNatives(ScriptingCore, methods, 1) != 0) {
                    std::cout << "Register Error!" << std::endl;
                }

                std::cout << "Registered Method!" << std::endl;


                if (er != "No Error") {
                    PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string> {"1", er});
                }
                else
                {
                    if (ScriptingCore == nullptr) {
                        PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string> {"Failed To Make Class", "Failed To Make Class"});
                    }
                    else
                    {
                        //Run Main Method
                        jmethodID mainMethod = jniEnv->GetStaticMethodID(ScriptingCore, "Main", "()I");

                        jint result = jniEnv->CallStaticIntMethod(ScriptingCore, mainMethod);

                        std::cout << "Result of Main Method: " << result << std::endl;

                        const char* err = CheckJNIError(jniEnv);

                        std::cout << "Testing Script Capability!" << std::endl;

                        jclass PythonInterpreterCalss = jniEnv->FindClass("org/python/util/PythonInterpreter");
                        jmethodID PYconstructor = jniEnv->GetMethodID(PythonInterpreterCalss, "<init>", "()V");
                        jobject PYInterpreterOBJ = jniEnv->NewObject(PythonInterpreterCalss, PYconstructor);

                        jmethodID PYInterpreterSetOut = jniEnv->GetMethodID(PythonInterpreterCalss, "setOut", "(Ljava/io/Writer;)V");
                        jmethodID PYInterpreterSetErr = jniEnv->GetMethodID(PythonInterpreterCalss, "setErr", "(Ljava/io/Writer;)V");
                        jniEnv->CallVoidMethod(PYInterpreterOBJ, PYInterpreterSetOut, stringWriterObj);
                        jniEnv->CallVoidMethod(PYInterpreterOBJ, PYInterpreterSetErr, stringWriterObj);

                        jmethodID PYInterpreterExec = jniEnv->GetMethodID(PythonInterpreterCalss, "exec", "(Ljava/lang/String;)V");
                        if (PYInterpreterExec == nullptr) {
                            std::cout << "execfile signature incorrect!" << std::endl;
                        }
                        jstring ArgsString = jniEnv->NewStringUTF("print(\"The scripting system was setup correctly!\")");
                        jniEnv->CallVoidMethod(PYInterpreterOBJ, PYInterpreterExec, ArgsString);

                        CheckJNIError(jniEnv);


                        if (result == 1) {
                            PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string> {"1", err});
                        }
                        else
                        {
                            PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string> {"Setup Python and JReverse Scripting System on target process!"});
                        }

                        std::cout << "Checking for Interpreter class" << std::endl;
                        jclass PYInterpreterClass = jniEnv->FindClass("org/python/util/PythonInterpreter");
                        if (PYInterpreterClass == nullptr) {
                            std::cout << "Failed to find Python Interperter Class!" << std::endl;
                        }
                    }
                }
            }
        }
        else if (called == "addClass") {
            jclass ScriptingCore = jniEnv->FindClass("com/jreverse/jreverse/Core/JReverseScriptingCore");
            if (ScriptingCore == nullptr) {
                PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string> {"Scripting Core Class Not Initalized!"});
            }
            else
            {                

                std::cout << "Adding Loaded Class Now!" << std::endl;
                jmethodID addMethod = jniEnv->GetStaticMethodID(ScriptingCore, "AddClass", "(Ljava/lang/String;)Ljava/lang/String;");
                if (addMethod == nullptr) {
                    std::cout << "You Stupid!" << std::endl;
                }
                else
                {
                    //Convert std::vector<std::string> to jboject array
                    jstring arg = jniEnv->NewStringUTF(args[0].c_str());

                    std::cout << "Getting Result of AddClass!" << std::endl;
                    jstring resultofadd = (jstring)jniEnv->CallStaticObjectMethod(ScriptingCore, addMethod, arg);
                    std::cout << "Called the Method!" << std::endl;
                    std::string resultString = jniEnv->GetStringUTFChars(resultofadd, NULL);
                    std::cout << "Result of AddClasses: " << resultString << std::endl;

                    PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string>{ resultString });
                }
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
               
                std::cout << "Checking for Interpreter class" << std::endl;
                jclass PythonInterpreterCalss = jniEnv->FindClass("org/python/util/PythonInterpreter");
                if (PythonInterpreterCalss == nullptr) {
                    std::cout << "Failed to find Python Interperter Class!" << std::endl;
                }
                else
                {
                    
                    std::cout << "Running Script Process" << std::endl;
                    //Get constructor and create the Interpreter Object.
                    jmethodID PYconstructor = jniEnv->GetMethodID(PythonInterpreterCalss, "<init>", "()V");
                    jobject PYInterpreterOBJ = jniEnv->NewObject(PythonInterpreterCalss, PYconstructor);

                    std::cout << "Constructed The Python Interpreter" << std::endl;

                    //Link the String Writer to the PYInterpreter
                    jmethodID PYInterpreterSetOut = jniEnv->GetMethodID(PythonInterpreterCalss, "setOut", "(Ljava/io/Writer;)V");
                    jmethodID PYInterpreterSetErr = jniEnv->GetMethodID(PythonInterpreterCalss, "setErr", "(Ljava/io/Writer;)V");
                    jmethodID PYInterpreterClose = jniEnv->GetMethodID(PythonInterpreterCalss, "close", "()V");
                    jniEnv->CallVoidMethod(PYInterpreterOBJ, PYInterpreterSetOut, stringWriterObj);
                    jniEnv->CallVoidMethod(PYInterpreterOBJ, PYInterpreterSetErr, stringWriterObj);

                    CheckJNIError(jniEnv);

                    std::cout << "Linked String Writer" << std::endl;

                    std::chrono::system_clock::time_point begtime = std::chrono::system_clock::now();
                    std::time_t now_time = std::chrono::system_clock::to_time_t(begtime);

                    std::stringstream timestring;
                    timestring << "Execution of script at: ";
                    timestring << std::ctime(&now_time);
                    timestring << "-------------------------------------------------------\n";
                    
                    jstring curtime = jniEnv->NewStringUTF(timestring.str().c_str());

                    jniEnv->CallVoidMethod(stringWriterObj, writeWriterMethod, curtime);

                    std::cout << "Changing Jython Modifier Respectfullness" << std::endl;
                    jclass JythonOptions = jniEnv->FindClass("org/python/core/Options");
                    if (JythonOptions != nullptr) {
                        jfieldID RespectModifiers = jniEnv->GetStaticFieldID(JythonOptions, "respectJavaAccessibility", "Z");
                        if (RespectModifiers != nullptr) {
                            jboolean flasebool = (jboolean)false;
                            jniEnv->SetStaticBooleanField(JythonOptions, RespectModifiers, flasebool);
                        }
                        else std::cout << "Registry Class Field was not found" << std::endl;
                    }
                    else std::cout << "Registry Class was not found" << std::endl;
                    


                    //Call ExecFile
                    jmethodID PYInterpreterExecFile = jniEnv->GetMethodID(PythonInterpreterCalss, "execfile", "(Ljava/lang/String;)V");
                    if (PYInterpreterExecFile == nullptr) {
                        std::cout << "execfile signature incorrect!" << std::endl;
                    }
                    jstring ArgsString = jniEnv->NewStringUTF(args[0].c_str());
                    jniEnv->CallVoidMethod(PYInterpreterOBJ, PYInterpreterExecFile, ArgsString);

                    CheckJNIError(jniEnv);

                    std::cout << "Executed execfile" << std::endl;

                    PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string> {"Executed Script!"});

                    std::cout << "Returned Values" << std::endl;
                }
                
                
            }
            
        }
        else if (called == "getStringWriterData") {
            jstring returnable = (jstring)jniEnv->CallObjectMethod(stringWriterObj, toStringMethod);
            std::string toret = jniEnv->GetStringUTFChars(returnable, NULL);
            PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string> {toret});
        }
        else if (called == "clearStringWriterData") {
            jniEnv->CallVoidMethod(stringWriterObj, flushWriterMethod);
            PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string> {"Flushed the stringwriter!"});
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
        freopen_s(&p_file, "CONOUT$", "w", stderr);

        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MainThread, hinstance, 0, nullptr);
    }
    else if (reason == DLL_PROCESS_DETACH)
    {
        fclose(p_file);
        FreeConsole();
    }

    return true;
}

