#include "com_jreverse_jreverse_PipeManager_PipeManager.h"
#include "PipeAPI.h"
#include "JReversePipeInfo.h"
#include <string>
#include <sstream>

JNIEXPORT void JNICALL Java_com_jreverse_jreverse_PipeManager_PipeManager_InitAPI(JNIEnv*, jclass)
{
    PipeAPI::setCritPipes();
}

JNIEXPORT jobjectArray JNICALL Java_com_jreverse_jreverse_PipeManager_PipeManager_GetLoadedPipes(JNIEnv* env, jclass)
{   
    std::vector<std::string> exdata;
    exdata.push_back("Testing");
    exdata.push_back("Testing");
    exdata.push_back("Testing");
    exdata.push_back("Testing");
    exdata.push_back("Testing");
    exdata.push_back("Testing");

    
    //PipeAPI::PipeNamePipe.WritePipe(exdata);
   
    /*
    JReversePipe<int> mypipe = JReversePipe<int>("MyIntPipe", boost::interprocess::read_write, 1000);
    PipeAPI::AddPipeToList("MyIntPipe:int");
    mypipe.WritePipe(345);
    int noway = mypipe.ReadPipe();
    */
    
   
    
    //
    // THIS WAS COPIED OF GOOGLE FOR EXAMPLE, FINISH THE PIPE API THEN USE IT.
    // 
    // Find the class of java.lang.String
    jclass stringClass = env->FindClass("java/lang/String");

    // Create an empty array of strings
    jobjectArray stringArray = env->NewObjectArray(10, stringClass, NULL);

    // Add some strings to the array
    /*
    jstring string1 = env->NewStringUTF("errr.c_str()");
    jstring string2 = env->NewStringUTF(std::to_string(noway).c_str());
    env->SetObjectArrayElement(stringArray, 0, string1);
    env->SetObjectArrayElement(stringArray, 1, string2);
    */
    std::string errr = "No Error";
    std::vector<std::string> tes = PipeAPI::PipeNamePipe.ReadPipe();
    
    jstring string1 = env->NewStringUTF(std::to_string(tes.size()).c_str());
    env->SetObjectArrayElement(stringArray, 0, string1);
    
    for (int i = 0; i < tes.size(); i++) {
        jstring string1 = env->NewStringUTF(tes[i].c_str());
        env->SetObjectArrayElement(stringArray, i, string1);
    }
    

    // Return the array
    return stringArray;
}

JNIEXPORT jobjectArray JNICALL Java_com_jreverse_jreverse_PipeManager_PipeManager_GetPipeInfo(JNIEnv* env, jclass, jstring name)
{
    std::string pipname = env->GetStringUTFChars(name, NULL);
    JReversePipeInfo pipinfo = PipeAPI::GetPipeInfo(pipname);
    jclass stringClass = env->FindClass("java/lang/String");

    // Create an empty array of strings
    jobjectArray stringArray = env->NewObjectArray(3, stringClass, NULL);

    jstring string1 = env->NewStringUTF("read/write");
    env->SetObjectArrayElement(stringArray, 0, string1);

    jstring string2 = env->NewStringUTF(pipinfo.Name.c_str());
    env->SetObjectArrayElement(stringArray, 1, string2);

    jstring string3 = env->NewStringUTF(std::to_string(pipinfo.Size).c_str());
    env->SetObjectArrayElement(stringArray, 2, string3);

    return stringArray;
}

JNIEXPORT void JNICALL Java_com_jreverse_jreverse_PipeManager_PipeManager_AddPipe(JNIEnv* env, jclass, jstring name, jint size, jstring type)
{
    std::string pipname = env->GetStringUTFChars(name, NULL);
    std::string a = env->GetStringUTFChars(type, NULL);
    if (a == "int") {
        JReversePipe<int> addpip = JReversePipe<int>(pipname, boost::interprocess::read_write, (int)size);
        //TODO: add refrence
    }
    else if (a == "bool") {

    }
    else if (a == "float") {

    }
    else if (a == "double") {

    }
    else if (a == "std::string") {

    }
    else if (a == "std::vector<std::string>") {

    }
}

JNIEXPORT void JNICALL Java_com_jreverse_jreverse_PipeManager_PipeManager_RemovePipe(JNIEnv*, jclass, jstring name)
{
    return;
}

JNIEXPORT void JNICALL Java_com_jreverse_jreverse_PipeManager_PipeManager_ResizeAndReconnectPipe(JNIEnv* env, jclass, jstring name, jint size)
{
    std::stringstream recname;
    recname << "RECONNECT ";
    recname << env->GetStringUTFChars(name, NULL);
    std::string command = recname.str();
    const char* pip = env->GetStringUTFChars(name, NULL);
    std::string discommand = "DISCONNECT ";
    discommand.append(pip);
    PipeAPI::CommunicationPipe.WritePipe(discommand);
    Sleep(100);
    PipeAPI::ResizePipe(pip, size, env);
    PipeAPI::CommunicationPipe.WritePipe(command);
}
