#include "com_jreverse_jreverse_PipeManager_PipeManager.h"
#include "PipeAPI.h"

JNIEXPORT jobjectArray JNICALL Java_com_jreverse_jreverse_PipeManager_PipeManager_GetLoadedPipes(JNIEnv* env, jclass)
{
    //
    // THIS WAS COPIED OF GOOGLE FOR EXAMPLE, FLESH OUT THE PIPE API THEN USE IT.
    // 
    // Find the class of java.lang.String
    jclass stringClass = env->FindClass("java/lang/String");

    // Create an empty array of strings
    jobjectArray stringArray = env->NewObjectArray(10, stringClass, NULL);

    // Add some strings to the array
    jstring string1 = env->NewStringUTF("Hello");
    jstring string2 = env->NewStringUTF("World");
    env->SetObjectArrayElement(stringArray, 0, string1);
    env->SetObjectArrayElement(stringArray, 1, string2);

    // Return the array
    return stringArray;
}
