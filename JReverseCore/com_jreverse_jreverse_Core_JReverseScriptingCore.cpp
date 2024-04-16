#include "com_jreverse_jreverse_Core_JReverseScriptingCore.h"
#include "jvmti.h"
#include "jni.h"
#include "jvmticmlr.h"
#include "datastore.h"
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

    using jniGetCreatedJavaVMs_t = jint(*)(JavaVM** vmBuf, jsize bufLen, jsize* nVMs);

    const auto jvmHandle = GetModuleHandleW(L"jvm.dll");
    if (jvmHandle == nullptr)
    {
        std::printf("[!] Failed to retrieve handle for \"jvm.dll\"!\n");
    }

    const auto jniGetCreatedJavaVMs = reinterpret_cast<jniGetCreatedJavaVMs_t>(GetProcAddress(
        jvmHandle, "JNI_GetCreatedJavaVMs"));

    JavaVM* javaVm = nullptr;
    jniGetCreatedJavaVMs(&javaVm, 1, nullptr);
    if (javaVm == nullptr)
    {
        std::printf("[!] Failed to retrieve created Java VMs!\n");
    }
    
    jvmtiEnv* TIenv = nullptr;
    jint res = javaVm->GetEnv((void**)&TIenv, JVMTI_VERSION_1);
    if (res != JNI_OK)
    {
        std::printf("[!] Failed to retrive JVMTI interface on the Java VM.\n");
    }

    std::printf("Got TIenv Pointer\n");

    std::string clazz = jniEnv->GetStringUTFChars(ClassName, NULL);

    std::cout << "Checking Existance of: " << clazz << std::endl;

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
