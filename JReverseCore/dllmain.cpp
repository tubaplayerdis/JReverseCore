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
#include <conio.h> 
#include "PipeManager.h"
#include <boost/interprocess/windows_shared_memory.hpp>
#include "PipeClientAPI.h"
#include "ClassFile.h"
#include "ClassFileManager.h"
#include "datastore.h"
#include "com_jreverse_jreverse_Core_JReverseScriptingCore.h"
#include "JythonManager.h"
#include "JythonInterpreter.h"
#include "StartupRule.h"
#include "JReverseSettings.h"



void printBool(bool Bool) {
    if (Bool) {
        std::printf("true");
    }
    else
    {
        std::printf("false");
    }
}

const char* btoi(bool Bool) {
    if (Bool) return "true"; else return "false";
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

std::string GetAccessFlagAsString(int flag) {
    std::stringstream stream;
    if (flag & 0x0001) {
        stream << "|ACC_PUBLIC|";
    }
    if (flag & 0x0010) {
        stream << "|ACC_FINAL|";
    }
    if (flag & 0x0020) {
        stream << "|ACC_SUPER|";
    }
    if (flag & 0x0200) {
        stream << "|ACC_INTERFACE|";
    }
    if (flag & 0x0400) {
        stream << "|ACC_ABSTRACT|";
    }
    if (flag & 0x1000) {
        stream << "|ACC_SYNTHETIC|";
    }
    if (flag & 0x2000) {
        stream << "|ACC_ANNOTATION|";
    }
    if (flag & 0x4000) {
        stream << "|ACC_ENUM|";
    }
    return stream.str();
}

std::string GetStatusIntAsString(int status) {
    std::stringstream stream;
    if (status & JVMTI_CLASS_STATUS_VERIFIED) {
        stream << "|CLASS_STATUS_VERIFIED|";
    }
    if (status & JVMTI_CLASS_STATUS_PREPARED) {
        stream << "|CLASS_STATUS_PREPARED|";
    }
    if (status & JVMTI_CLASS_STATUS_INITIALIZED) {
        stream << "|CLASS_STATUS_INITIALIZED|";
    }
    if (status & JVMTI_CLASS_STATUS_ERROR) {
        stream << "|CLASS_STATUS_ERROR|";
    }
    if (status & JVMTI_CLASS_STATUS_ARRAY) {
        stream << "|CLASS_STATUS_ARRAY|";
    }
    if (status & JVMTI_CLASS_STATUS_PRIMITIVE) {
        stream << "|CLASS_STATUS_PRIMITIVE|";
    }
    return stream.str();
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
    //Check if neccacary.
    if (!JReverseStartupSettings::isClassFileCollection) return;

    //Verify Data
    if (class_data_len <= 0 || class_data == nullptr) {
        // Invalid arguments, log an error and return
        if(JReverseStartupSettings::isClassFileLoadMessages) std::cout << "Invalid Class, Abandoning" << std::endl;
        return;
    }

    if (class_being_redefined == nullptr) {
        if (JReverseStartupSettings::isClassFileLoadMessages) std::cout << "New Class Load" << std::endl;
    }
    else {
        if (JReverseStartupSettings::isClassFileLoadMessages) std::cout << "Class Redefenition" << std::endl;
    }

    if (name == nullptr) {
        if (JReverseStartupSettings::isClassFileLoadMessages) std::cout << "Name is NULL. Sending to Unresolved" << std::endl;
        name = "unknown";
    }

    // Perform actions when a class file is loaded
    if (JReverseStartupSettings::isClassFileLoadMessages) std::cout << "Loading Class: " << name << std::endl;

    for (int i = 0; i < JReverseStore::ruleslist.size(); i++)
    {
        if (strcmp(JReverseStore::ruleslist[i].ClassName, name)) {
            if (!JReverseStore::ruleslist[i].IsBypass) {

                std::string pre = JReverseStore::ruleslist[i].ByteCodes;
                std::vector<unsigned char> bytecode(pre.begin(), pre.end());

                const size_t result_size = bytecode.size() / 2;
                unsigned char* result = new unsigned char[result_size];

                for (size_t i = 0; i < result_size; ++i) {
                    std::string byte_string = pre.substr(i * 2, 2);
                    result[i] = static_cast<unsigned char>(std::stoi(byte_string, nullptr, 16));
                }

                *new_class_data_len = result_size;
                *new_class_data = new unsigned char[result_size];
                memcpy(*new_class_data, result, result_size);

                if (JReverseStartupSettings::isClassFileLoadMessages) std::cout << "Modified class by rule: " << name << std::endl;
                ClassFile curfile;
                curfile.bytecodes = pre;
                curfile.classname = name;
                ClassFileManager::AddClassFile(curfile);

                JReverseStore::ruleslist.erase(JReverseStore::ruleslist.begin() + i);//Remove the rule list
                return;
            }
            JReverseStore::bypassRules.push_back(JReverseStore::ruleslist[i]);//Add to bypass rules
            if (JReverseStartupSettings::isClassFileLoadMessages) std::cout << "Bypass Rule Detected!" << std::endl;
            JReverseStore::ruleslist.erase(JReverseStore::ruleslist.begin() + i);//Remove the rule list
        } 
    }

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


    /*
    PipeManager::SetupClient();
    std::printf(PipeManager::ReadString().c_str());
    std::printf("\n");

    PipeManager::WriteString("Testing Shared Memory Pipeline! - From Injected Application");
    */

    HMODULE jvmHandle = nullptr;
    while (true) {
        jvmHandle = GetModuleHandleW(L"jvm.dll");
        if (jvmHandle == nullptr)
        {
            std::printf("[!] Failed to retrieve handle for \"jvm.dll\"!\n");
        }
        else {
            std::printf("Got jvm.dll Handle!\n");
            break;
        }
    }


    const auto jniGetCreatedJavaVMs = reinterpret_cast<jniGetCreatedJavaVMs_t>(GetProcAddress(
        jvmHandle, "JNI_GetCreatedJavaVMs"));

    JavaVM* javaVm = nullptr;
    jniGetCreatedJavaVMs(&javaVm, 1, nullptr);
    while (true) {
        jniGetCreatedJavaVMs(&javaVm, 1, nullptr);
        if (javaVm == nullptr)
        {
            std::printf("[!] Failed to retrieve created Java VMs!\n");
        }
        else
        {
            std::printf("Got JavaVM!\n");
            break;
        }
    }

    //Do settings here
    JReverseStartupSettings::InitSettings();
    if (!JReverseStartupSettings::isConsoleWindow) FreeConsole();

    std::cout << "Got Settings!" << std::endl;


    JNIEnv* jniEnv = nullptr;
    Sleep(JReverseStartupSettings::JNIEnvTiemout);
    while (true)
    {
        if (javaVm->AttachCurrentThread(reinterpret_cast<void**>(&jniEnv), nullptr) != JNI_OK) {
            std::cout << "Not Okay!" << std::endl;
        }
        if (jniEnv == nullptr)
        {
            std::printf("[!] Failed to attach to the Java VM.\n");
        }
        else {
            std::printf("Got JNI ENV\n");
            break;
        }
    }


    jvmtiEnv* TIenv = nullptr;
    while (true) {
        jint res = javaVm->GetEnv((void**)&TIenv, JVMTI_VERSION_1);
        if (TIenv == nullptr) {
            std::printf("[!] Failed to Find JVMTI interface with the Java VM.\n");
        }
        else {
            std::printf("Got the TIENV\n");
            if (res != JNI_OK)
            {
                std::printf("[!] Error With JVMTIENV\n");
            }
            break;
        }
    }

    std::printf("Got the JVMTI Interface!\n");


    static jvmtiCapabilities capa;
    (void)memset(&capa, 0, sizeof(jvmtiCapabilities));
    capa.can_tag_objects = 1;
    capa.can_get_source_file_name = 1;
    capa.can_get_bytecodes = 1;
    capa.can_retransform_classes = 1;
    capa.can_retransform_any_class = 1;
    capa.can_redefine_classes = 1;
    capa.can_redefine_any_class = 1;
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

    //Serealize all Startup Rules
    std::vector<std::string> toiter = PipeClientAPI::StartupPipe.ReadPipe();

    if (toiter.size() % 3 != 0 || toiter.size() < 3) {
        std::cout << "Invalid Startup Rules!" << std::endl;
        std::cout << "Rule Dump: " << std::endl;
        for (std::string str : toiter) {
            std::cout << str << std::endl;
        }
    }
    else
    {
        std::cout << "Valid Rules!" << std::endl;
        for (int i = 0; i < toiter.size(); i += 3)
        {
            StartupRule rule = SerializeStartupRule(toiter[i].c_str(), toiter[i + 1].c_str(), toiter[i + 2].c_str());
            JReverseStore::ruleslist.push_back(rule);
        }
    }

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

    //PipeClientAPI::PrintPipes();
    std::cout << "TO init Function Call Loop: " << std::endl;
    Sleep(JReverseStartupSettings::funcLoopTimeout);

    

    //Create Scripting StringWriter!
    jclass stringWriterClass = nullptr;
    jmethodID StringWriterconstructor = nullptr;
    jmethodID toStringMethod = nullptr;
    jmethodID closeWriterMethod = nullptr;
    jmethodID flushWriterMethod = nullptr;
    jmethodID writeWriterMethod = nullptr;
    jobject stringWriterObj = nullptr;


    std::printf("Starting Function Call Loop\n");

    system("cls");

    //This dosent work.
    std::cout << "     ██╗██████╗ ███████╗██╗   ██╗███████╗██████╗ ███████╗███████╗    ██╗   ██╗ ██╗    ██████╗ " << std::endl;
    std::cout << "     ██║██╔══██╗██╔════╝██║   ██║██╔════╝██╔══██╗██╔════╝██╔════╝    ██║   ██║███║   ██╔═████╗" << std::endl;
    std::cout << "     ██║██████╔╝█████╗  ██║   ██║█████╗  ██████╔╝███████╗█████╗      ██║   ██║╚██║   ██║██╔██║" << std::endl;
    std::cout << "██   ██║██╔══██╗██╔══╝  ╚██╗ ██╔╝██╔══╝  ██╔══██╗╚════██║██╔══╝      ╚██╗ ██╔╝ ██║   ████╔╝██║" << std::endl;
    std::cout << "╚█████╔╝██║  ██║███████╗ ╚████╔╝ ███████╗██║  ██║███████║███████╗     ╚████╔╝  ██║██╗╚██████╔╝" << std::endl;
    std::cout << " ╚════╝ ╚═╝  ╚═╝╚══════╝  ╚═══╝  ╚══════╝╚═╝  ╚═╝╚══════╝╚══════╝      ╚═══╝   ╚═╝╚═╝ ╚═════╝ " << std::endl;

    //https://patorjk.com/software/taag/#p=display&f=ANSI%20Shadow&t=JReverse%20V1.0
    //FONT: ANSI Shadow, everything else defualt.

    while (true) {

        while (true) {
            if (GetAsyncKeyState(VK_F12) & 0x01) {
                bool restore = JReverseStartupSettings::isClassFileLoadMessages;
                JReverseStartupSettings::isClassFileLoadMessages = false;
                std::cout << "----------------------------------------------" << std::endl;
                std::cout << "JReverse Console. Enter a Command: ";
                std::string command;
                std::cin >> command;
                if (command == "RECONNECT") {
                    std::cout << "\nWhich Pipe? ";
                    std::string pip;
                    std::cin >> pip;
                    std::cout << "\n\nRECONNECTING: " << pip << std::endl;
                    if (pip == "CriticalFunctionPipe") {
                        PipeClientAPI::FunctionPipe.Reconnect();
                    }
                    else if (pip == "CriticalFunctionArgPipe") {
                        PipeClientAPI::FunctionArgPipe.Reconnect();
                    }
                    else if (pip == "CriticalReturnPipe") {
                        PipeClientAPI::ReturnPipe.Reconnect();
                    }
                    else if (pip == "CriticalPipeNamePipe") {
                        PipeClientAPI::PipeNamePipe.Reconnect();
                    }
                    else if (pip == "CriticalStartupPipe") {
                        PipeClientAPI::StartupPipe.Reconnect();
                    }
                    else if (pip == "CriticalSettingsPipe") {
                        PipeClientAPI::SettingsPipe.Reconnect();
                    }
                }

                JReverseStartupSettings::isClassFileLoadMessages = restore;
            }


            if (PipeClientAPI::CommunicationPipe.ReadPipe().find("RECONNECT") != std::string::npos) {
                std::string command = PipeClientAPI::CommunicationPipe.ReadPipe();
                std::string pip = command.substr(10);
                std::cout << "Reconnecting: " << pip << std::endl;
                if (pip == "CriticalFunctionPipe") {
                    PipeClientAPI::FunctionPipe.Reconnect();
                }
                else if (pip == "CriticalFunctionArgPipe") {
                    PipeClientAPI::FunctionArgPipe.Reconnect();
                }
                else if (pip == "CriticalReturnPipe") {
                    PipeClientAPI::ReturnPipe.Reconnect();
                }
                else if (pip == "CriticalPipeNamePipe") {
                    PipeClientAPI::PipeNamePipe.Reconnect();
                }
                else if (pip == "CriticalStartupPipe") {
                    PipeClientAPI::StartupPipe.Reconnect();
                }
                else if (pip == "CriticalSettingsPipe") {
                    PipeClientAPI::SettingsPipe.Reconnect();
                }
                PipeClientAPI::CommunicationPipe.WritePipe("NONE");
            }

            if (!PipeClientAPI::isFunctionPipeNone()) {
                std::printf("Function Call Detected!\n");
                break;
            }
#pragma region BypassRuleUglyStuff
            if (JReverseStore::bypassRules.size() > 0) {
                std::cout << "Bypass Detected!" << std::endl;
                for (int i = 0; i < JReverseStore::bypassRules.size(); i++) {
                    std::cout << "Bypass Rule: " << JReverseStore::bypassRules[i].ClassName << "is being redefined" << std::endl;
                    std::vector<std::string> args = std::vector<std::string>{ JReverseStore::bypassRules[i].ClassName, JReverseStore::bypassRules[i].ByteCodes };
                    if (args[0].empty() || args[1].empty()) { std::printf("Invalid Bypass Rule\n"); }
                    else {
                        //0 = jclass, 1=bytecodes
                        jclass respectiveclass = jniEnv->FindClass(args[0].c_str());

                        std::cout << "Got Respective Class!" << std::endl;

                        if (respectiveclass == nullptr) std::printf("Respective Class NULL!");

                        std::string input_string = args[1];
                        std::vector<unsigned char> bytecode(input_string.begin(), input_string.end());
                        const size_t result_size = bytecode.size() / 2;
                        unsigned char* result = new unsigned char[result_size];

                        for (size_t i = 0; i < result_size; ++i) {
                            std::string byte_string = input_string.substr(i * 2, 2);
                            result[i] = static_cast<unsigned char>(std::stoi(byte_string, nullptr, 16));
                        }

                        std::cout << "Implemented Bytecodes" << std::endl;

                        jvmtiClassDefinition def;
                        def.class_bytes = result;
                        def.class_byte_count = result_size;
                        def.klass = respectiveclass;
                        jvmtiError res = TIenv->RedefineClasses(1, &def);
                        std::string callback = GetJVMTIError(res);
                        std::cout << "Refed bypass error: " << callback << std::endl;

                        delete[] result;//Prevent Memory Leaks
                    }
                    JReverseStore::bypassRules.erase(JReverseStore::bypassRules.begin() + i);
                }
            }
#pragma endregion
            Sleep(5);
            //Part of main loop    
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
            std::printf("Getting Class Bytecodes\n");
            ClassFile found = ClassFileManager::FindClassFile(args[0]);
            if (found.classname == "NOT FOUND") {
                PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string>{"Class File Not Found", "Class File Not Found"});
            }
            else
            {
                PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string> {found.classname, found.bytecodes});
            }
        }
        else if (called == "getClassExtraData") {
            std::cout << "Gettings Class Data on: " << args[0] << std::endl;

            if (args[0].empty()) {
                PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string>{"Args Empty"});
                continue;
            }
            jclass clazz = jniEnv->FindClass(args[0].c_str());
            if (clazz == nullptr) {
                PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string>{"Class Not Found"});
                continue;
            }
            jvmtiError curer;

            //IsInterface
            jboolean isinterbool;
            curer = TIenv->IsInterface(clazz, &isinterbool);
            std::cout << "GetEx0: " << GetJVMTIError(curer) << std::endl;

            //Version
            jint major;
            jint minor;
            TIenv->GetClassVersionNumbers(clazz, &major, &minor);
            std::cout << "GetEx1: " << GetJVMTIError(curer) << std::endl;

            //IsModifyable
            jboolean ismodbool;
            curer = TIenv->IsModifiableClass(clazz, &ismodbool);
            std::cout << "GetEx2: " << GetJVMTIError(curer) << std::endl;

            //IsArray
            jboolean isarrbool;
            curer = TIenv->IsArrayClass(clazz, &isarrbool);
            std::cout << "GetEx3: " << GetJVMTIError(curer) << std::endl;

            //Acsess Flags
            jint flagsint;
            curer = TIenv->GetClassModifiers(clazz, &flagsint);
            std::cout << "GetEx4: " << GetJVMTIError(curer) << std::endl;

            //Status
            jint statusint;
            curer = TIenv->GetClassStatus(clazz, &statusint);
            std::cout << "GetEx5: " << GetJVMTIError(curer) << std::endl;

            std::vector<std::string> retrunable = { btoi(isinterbool), std::to_string(major) + "." + std::to_string(minor), btoi(ismodbool), btoi(isarrbool), GetAccessFlagAsString(flagsint), GetStatusIntAsString(statusint)};


            /*
            * IsInterface
            * Version
            * IsModifyable
            * IsArrayClass
            * Access Flags - https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html
            * Status Int - https://docs.oracle.com/javase/8/docs/platform/jvmti/jvmti.html#GetClassStatus
            */
            PipeClientAPI::ReturnPipe.WritePipe(retrunable);
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
            //Init writers and stuff etc
            stringWriterClass = jniEnv->FindClass("java/io/StringWriter");
            StringWriterconstructor = jniEnv->GetMethodID(stringWriterClass, "<init>", "()V");
            toStringMethod = jniEnv->GetMethodID(stringWriterClass, "toString", "()Ljava/lang/String;");
            closeWriterMethod = jniEnv->GetMethodID(stringWriterClass, "close", "()V");
            flushWriterMethod = jniEnv->GetMethodID(stringWriterClass, "flush", "()V");
            writeWriterMethod = jniEnv->GetMethodID(stringWriterClass, "write", "(Ljava/lang/String;)V");
            stringWriterObj = jniEnv->NewObject(stringWriterClass, StringWriterconstructor);

            //Seutp python
            std::cout << "Seting up Jython!" << std::endl;

            if (!std::filesystem::exists(args[1].c_str())) std::cout << "The Jar file does not exist!" << std::endl;

            //This works. Thanks Stack Overflow!
            add_path(jniEnv, args[1]);

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
        else if (called == "getInterpreterIDS") {
            std::vector<std::string> ReturnList;
            std::vector<int> MainList = JythonManager::GetIDS();
            for (int i = 0; i < MainList.size(); i++) {
                ReturnList.push_back(std::to_string(MainList[i]));
            }
            PipeClientAPI::ReturnPipe.WritePipe(ReturnList);
        }
        else if (called == "deleteInterpreter") {
            int id = std::stoi(args[0]);
            JythonManager::RemoveByID(id);
        }
        else if (called == "createInterpreter") {
            jclass PythonInterpreterCalss = jniEnv->FindClass("org/python/util/PythonInterpreter");
            if (PythonInterpreterCalss == nullptr) {
                std::cout << "Failed to find Python Interperter Class When Making New Interpreter!" << std::endl;
            }
            JythonInterpreter what(jniEnv, stringWriterObj, PythonInterpreterCalss, writeWriterMethod);
            std::cout << "Adding Interpreter!" << std::endl;
            JythonManager::AddInterpreter(what);
            PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string> {std::to_string(what.ID)});
        }
        else if (called == "addClass") {
            //Add ClassFile to classpath
#pragma region OldIMPL
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
#pragma endregion
        }
        else if (called == "runScript")
        {
            jclass ScriptingCore = jniEnv->FindClass("com/jreverse/jreverse/Core/JReverseScriptingCore");
            if (ScriptingCore == nullptr) {
                PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string> {"Scripting Core Class Not Initalized!"});
            }
            else
            {
                //args are ID, path
                std::cout << "Checking for Interpreter class" << std::endl;
                jclass PythonInterpreterCalss = jniEnv->FindClass("org/python/util/PythonInterpreter");
                if (PythonInterpreterCalss == nullptr) {
                    std::cout << "Failed to find Python Interperter Class!" << std::endl;
                }
                else
                {
                    int ID = std::stoi(args[1]);
                    JythonReturns ret = JythonManager::RunOnInterpreter(ID, args[0]);
                    switch (ret)
                    {
                    case SUCCESS:
                        PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string> {"Executed Script!"});
                        break;
                    case INTERPRETER_NOT_FOUND:
                        PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string> {"Interpreter was not found"});
                        break;
                    case FAILURE:
                        PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string> {"Failed Execution"});
                        break;
                    default:
                        PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string> {"Something went wrong"});
                        break;
                    }
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
        else if (called == "redefineClass") {
            if (args[0].empty() || args[1].empty()) { std::printf("ARGS NULL"); PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string>{"Args NULL"}); }
            else {
                //0 = jclass, 1=bytecodes
                jclass respectiveclass = jniEnv->FindClass(args[0].c_str());

                std::cout << "Got Respective Class!" << std::endl;

                if (respectiveclass == nullptr) std::printf("Respective Class NULL!");

                std::string input_string = args[1];
                std::vector<unsigned char> bytecode(input_string.begin(), input_string.end());
                const size_t result_size = bytecode.size() / 2;
                unsigned char* result = new unsigned char[result_size];

                for (size_t i = 0; i < result_size; ++i) {
                    std::string byte_string = input_string.substr(i * 2, 2);
                    result[i] = static_cast<unsigned char>(std::stoi(byte_string, nullptr, 16));
                }

                std::cout << "Implemented Bytecodes" << std::endl;

                jvmtiClassDefinition def;
                def.class_bytes = result;
                def.class_byte_count = result_size;
                def.klass = respectiveclass;
                jvmtiError res = TIenv->RedefineClasses(1, &def);
                std::string callback = GetJVMTIError(res);
                PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string>{callback});

                delete[] result;//Prevent Memory Leaks
            }

        }
        else if (called == "retransformClass") {
            if (args[0].empty()) {
                std::cout << "Args Empty on retransform" << std::endl;
            }
            else
            {
                jclass toredef = jniEnv->FindClass(args[0].c_str());
                const jclass* pointers = { &toredef };
                if (toredef != nullptr) {
                    //IsModifyable
                    jboolean ismodbool;
                    TIenv->IsModifiableClass(toredef, &ismodbool);
                    if (ismodbool) {
                        std::string returnable = GetJVMTIError(TIenv->RetransformClasses(1, pointers));
                        PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string>{"Retransform Classes Callback: ", returnable});
                    }
                    else
                    {
                        PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string>{"Class Cannot be modded!", "Class Cannot be modded!"});
                    }
                    
                }
                else
                {
                    PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string>{"Class ref null", "Class ref null"});
                }
                jniEnv->DeleteLocalRef(toredef);
            }
        }
        else if (called == "retransformClasses") {
            /*
            Add modifyable check
            */
            if (args[0].empty()) {
                std::cout << "Args Empty on retransform" << std::endl;
                PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string>{"Args Empty!"});
                continue;
            }
            std::vector<jclass> classrefs;
            classrefs.reserve(args.size());
            for (int i = 0; i < args.size(); i++) 
            {
                jclass adder = jniEnv->FindClass(args[i].c_str());
                if (adder != nullptr) classrefs.emplace_back(adder);
            }
            std::cout << "Got all JClass refs" << std::endl;
            const jclass* pointers = classrefs.data();
            const char* returnable = GetJVMTIError(TIenv->RetransformClasses(classrefs.size(), pointers));
            PipeClientAPI::ReturnPipe.WritePipe(std::vector<std::string>{"Retransform Classes Callback: ", returnable});
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
        freopen_s(&p_file, "CONIN$", "r", stdin);
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

