#include "com_jreverse_jreverse_Core_JReverseScriptingCore.h"
#include "jvmti.h"
#include "jni.h"
#include "jvmticmlr.h"
#include "datastore.h"
#include <string>

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

static jvmtiIterationControl JNICALL
heap_callback(jlong class_tag, jlong size, jlong* tag_ptr, void* user_data) {
    *tag_ptr = 1;
    return JVMTI_ITERATION_CONTINUE;
}


JNIEXPORT jobjectArray JNICALL Java_com_jreverse_jreverse_Core_JReverseScriptingCore_GetClassInstances(JNIEnv* jniEnv, jclass, jstring ClassName)
{
    jvmtiEnv* TIenv = JReverseStore::jvmtienv;
    std::string clazz = jniEnv->GetStringUTFChars(ClassName, NULL);
    jclass cladd = jniEnv->FindClass(clazz.c_str());
    jclass stringclass = jniEnv->FindClass("java/lang/String");
    if (cladd == nullptr) {
        jstring badstring = jniEnv->NewStringUTF("Class Not Found!");
        jobjectArray badresult = jniEnv->NewObjectArray(1, stringclass, badstring);
        return badresult;
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
    std::printf(GetJVMTIError(err));
    std::printf("\n");

    std::printf("Class Requested OBJ Count:\n");
    std::printf(std::to_string((int)count).c_str());
    std::printf("\n");

    jobjectArray result = jniEnv->NewObjectArray(count, cladd, NULL);

    for (int i = 0; i < count; i++) {
        jobject cur = objs[i];
        jniEnv->SetObjectArrayElement(result, i, cur);
    }

    return result;
}
