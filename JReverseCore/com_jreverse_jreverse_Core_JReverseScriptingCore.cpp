#include "com_jreverse_jreverse_Core_JReverseScriptingCore.h"
#include "jvmti.h"
#include "jni.h"
#include "jvmticmlr.h"
#include <string>
#include <windows.h>
#include <iostream>

static jvmtiIterationControl JNICALL
heap_callback(jlong class_tag, jlong size, jlong* tag_ptr, void* user_data) {
    *tag_ptr = 1;
    return JVMTI_ITERATION_CONTINUE;
}


JNIEXPORT jobjectArray JNICALL Java_com_jreverse_jreverse_Core_JReverseScriptingCore_GetClassInstances(JNIEnv* jniEnv, jclass, jstring ClassName)
{
    std::printf("Getting TIEnv Pointer!\n");

    JavaVM* javaVm = nullptr;
    jint atres = jniEnv->GetJavaVM(&javaVm);
    std::cout << atres << std::endl;

    if (javaVm == nullptr) std::printf("Extremley Large Problem!");

    javaVm->AttachCurrentThread(reinterpret_cast<void**>(&jniEnv), nullptr);
    
    jvmtiEnv* TIenv = nullptr;
    jint res = javaVm->GetEnv((void**)&TIenv, JVMTI_VERSION_1);
    if (res != JNI_OK)
    {
        std::printf("[!] Failed to retrive JVMTI interface on the Java VM.\n");
    }

    if (jniEnv->ExceptionCheck() == true) std::printf("JNI ERROR!");

    


    std::printf("Got TIenv and envPointer\n");

    if (jniEnv == nullptr) std::printf("Major Problem!");

    std::string clazz = jniEnv->GetStringUTFChars(ClassName, NULL);

    std::cout << "Checking Existance of: " << clazz.c_str() << std::endl;

    jclass cladd = jniEnv->FindClass(clazz.c_str()); //Does not find some loaded classes for some reason.
    jclass stringclass = jniEnv->FindClass("java/lang/String");
    if (cladd == nullptr) {
        std::printf("Class Does Not Exist!\n");
        jstring badstring = jniEnv->NewStringUTF("Class Not Found!");
        jobjectArray badresult = jniEnv->NewObjectArray(1, stringclass, NULL);
        jniEnv->SetObjectArrayElement(badresult, 0, badstring);
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

    jobjectArray result = jniEnv->NewObjectArray(count, cladd, NULL);

    for (int i = 0; i < count; i++) {
        jobject cur = objs[i];
        jniEnv->SetObjectArrayElement(result, i, cur);
    }

    return result;
}
