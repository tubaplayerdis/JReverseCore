#include "JReverseLogger.h"
#include <string>
#include "jni.h"


JReverseLogger::JReverseLogger(JNIEnv* env)
{
	Env = env;
	LoggerClass = env->FindClass("com/jreverse/jreverse/Bridge/JReverseLogger");
	LoggingMethodID = env->GetStaticMethodID(LoggerClass, "Print", "(Ljava/lang/String;)V");
}

void JReverseLogger::Log(std::string message)
{
	Env->CallStaticVoidMethod(LoggerClass, LoggingMethodID, Env->NewStringUTF(message.c_str()));
}
