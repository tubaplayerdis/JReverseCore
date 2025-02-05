#include "com_jreverse_jreverse_Core_JReverseScriptingCore.h"
#include "jvmti.h"
#include "jni.h"
#include "jvmticmlr.h"
#include <string>
#include <sstream>
#include <windows.h>
#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>

JNIEnv* globalenv = nullptr;
jvmtiEnv* TIenv = nullptr;

void removeChars(std::string& str, const std::string& charsToRemove) {
    for (char c : charsToRemove) {
        str.erase(std::remove(str.begin(), str.end(), c), str.end());
    }
}

// Function to remove the first occurrence of a substring from a string
void removeFirstOccurrence(std::string& str, const std::string& substr) {
    size_t pos = str.find(substr);
    if (pos != std::string::npos) {
        str.erase(pos, substr.length());
    }
}



jclass getLoadedClassRefrence(jvmtiEnv* TIenv, JNIEnv* jniEnv, const char* classname) {
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
            }
            else
            {
                std::string namebuild = std::string(name);

                if (namebuild == classname) {
                    std::cout << "Loaded Class Ref: " << namebuild << " Was matched to: " << classname << std::endl;
                    return clazz;
                }               

                TIenv->Deallocate((unsigned char*)name);
                
            }



        }
    }
    return nullptr;
}


void setGlobalEnv(JNIEnv* pointer, jvmtiEnv* pointerr) {
    globalenv = pointer;
    TIenv = pointerr;
}

static jvmtiIterationControl JNICALL
heap_callback(jlong class_tag, jlong size, jlong* tag_ptr, void* user_data) {
    *tag_ptr = 1;
    return JVMTI_ITERATION_CONTINUE;
}

const char* CheckJNIErrorCooler(JNIEnv* jniEnv) {
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


JNIEXPORT jobjectArray JNICALL Java_com_jreverse_jreverse_Core_JReverseScriptingCore_GetClassInstances(JNIEnv* jniEnv, jclass, jstring ClassName)
{
    std::printf("Getting Thread!\n");
    std::thread::id this_id = std::this_thread::get_id();
    std::cout << "GetClassInstance thread: " << this_id << std::endl;
    std::cout << "Starting Native Registration!" << std::endl;

    std::printf("Getting TIEnv Pointer!\n");

    JavaVM* javaVm = nullptr;
    jint atres = globalenv->GetJavaVM(&javaVm);
    std::cout << atres << std::endl;

    if (javaVm == nullptr) std::printf("Extremley Large Problem!");

    if (globalenv->ExceptionCheck() == true) std::printf("JNI ERROR!");

    std::cout << "Addres of ENV: " << &globalenv << std::endl;


    std::printf("Got TIenv and envPointer\n");

    if (globalenv == nullptr) std::printf("Major Problem!");

    std::string clazz = globalenv->GetStringUTFChars(ClassName, NULL);

    std::cout << "Checking Existance of: " << clazz.c_str() << std::endl;

    jclass cladd = getLoadedClassRefrence(TIenv, globalenv, clazz.c_str()); //Does not find some loaded classes for some reason.
    CheckJNIErrorCooler(globalenv);
    jclass stringclass = globalenv->FindClass("java/lang/String");
    if (stringclass == nullptr) { std:printf("Class Finding Does not work!"); }
    if (cladd == nullptr) {
        std::printf("Class Does Not Exist!\n");
        jstring badstring = globalenv->NewStringUTF("Class Not Found!");
        jobjectArray badresult = globalenv->NewObjectArray(1, stringclass, NULL);
        globalenv->SetObjectArrayElement(badresult, 0, badstring);
        std::printf("Sending off String Array!\n");
        return badresult;
    }


    std::printf("Checked Class Existance!\n");

    jvmtiError iterr = TIenv->IterateOverInstancesOfClass(cladd, JVMTI_HEAP_OBJECT_EITHER, heap_callback, NULL);
    std::printf("Error Code Of Iterate:\n");
    std::printf("\n");

    jint count = 0;
    jobject* objs;
    jlong tag = 1;
    jvmtiError err = TIenv->GetObjectsWithTags(1, &tag, &count, &objs, NULL);
    std::printf("Error Code Of GetOBJ:\n");
    std::printf("\n");

    std::printf("Class Requested OBJ Count:\n");
    std::printf(std::to_string((int)count).c_str());
    std::printf("\n");

    std::printf("Got Class Instacnes!\n");

    char* ExSignatiureName = nullptr;
    TIenv->GetClassSignature(cladd, &ExSignatiureName, NULL);
    std::cout << "Expecting: \n" << ExSignatiureName << std::endl;

    jobjectArray result = globalenv->NewObjectArray(count, cladd, NULL);

    for (int i = 0; i < count; i++) {
        std::cout << "Looping through: " << i << std::endl;
        if (globalenv->IsInstanceOf(objs[i], cladd)) {
            jclass ResultedClass = jniEnv->GetObjectClass(objs[i]);
            char* SignatiureName = nullptr;
            TIenv->GetClassSignature(ResultedClass, &SignatiureName, NULL);
            if (SignatiureName == nullptr) std::cout << "Signatue was NULL!" << std::endl;
            std::cout << "Sending off: " << SignatiureName << std::endl;
            globalenv->SetObjectArrayElement(result, i, objs[i]);
            TIenv->Deallocate((unsigned char*)SignatiureName);
        }
        else
        {
            jclass ResultedClass = jniEnv->GetObjectClass(objs[i]);
            char* SignatiureName = nullptr;
            TIenv->GetClassSignature(ResultedClass, &SignatiureName, NULL);
            if (ResultedClass == cladd) {
                std::cout << "Recomp is Sucsessfull, Sending off: " << SignatiureName << std::endl;
            }
            else {
                std::cout << "Class Mismatch! Expected: " << clazz << " Provided: " << SignatiureName << std::endl;
            }
            TIenv->Deallocate((unsigned char*)SignatiureName);
        }
    }

    TIenv->Deallocate((unsigned char*)objs);
    TIenv->Deallocate((unsigned char*)ExSignatiureName);

    //CheckJNIErrorCooler(globalenv);

    return result;
}
