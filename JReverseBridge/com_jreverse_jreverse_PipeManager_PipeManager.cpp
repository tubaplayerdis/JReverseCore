#include "com_jreverse_jreverse_PipeManager_PipeManager.h"
#include "PipeAPI.h"
#include "JReversePipeInfo.h"

JNIEXPORT void JNICALL Java_com_jreverse_jreverse_PipeManager_PipeManager_InitAPI(JNIEnv*, jclass)
{
    //PipeAPI::addCritPipes();
}

JNIEXPORT jobjectArray JNICALL Java_com_jreverse_jreverse_PipeManager_PipeManager_GetLoadedPipes(JNIEnv* env, jclass)
{
    JReversePipeInfo info = PipeAPI::PipeNamePipe.GetInfo();
    //
    // THIS WAS COPIED OF GOOGLE FOR EXAMPLE, FINISH THE PIPE API THEN USE IT.
    // 
    // Find the class of java.lang.String
    jclass stringClass = env->FindClass("java/lang/String");

    // Create an empty array of strings
    jobjectArray stringArray = env->NewObjectArray(10, stringClass, NULL);

    // Add some strings to the array
    jstring string1 = env->NewStringUTF(info.Name.c_str());
    jstring string2 = env->NewStringUTF(std::to_string(info.Size).c_str());
    env->SetObjectArrayElement(stringArray, 0, string1);
    env->SetObjectArrayElement(stringArray, 1, string2);

    /*
    for (int i = 0; i < info.size(); i++) {
        jstring string1 = env->NewStringUTF(info[i].c_str());
        env->SetObjectArrayElement(stringArray, i, string1);
    }
    */

    // Return the array
    return stringArray;
}
