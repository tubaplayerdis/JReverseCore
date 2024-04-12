#include "JReverseLogger.h"
#include <string>
#include "jni.h"


JReverseLogger::JReverseLogger(JNIEnv* env)
{
	Env = env;
	LoggerClass = env->FindClass("com/jreverse/jreverse/Bridge/JReverseLogger");
	LoggingMethodID = env->GetStaticMethodID(LoggerClass, "Print", "(Ljava/lang/String;)V");
	RemLoggingID = env->GetStaticMethodID(LoggerClass, "RemoveAndPrint", "(Ljava/lang/String;)V");
	PipeCallBackLimitID = env->GetStaticFieldID(LoggerClass, "PipeCallBackLimit", "I");
}

void JReverseLogger::Log(std::string message)
{	
	Env->CallStaticVoidMethod(LoggerClass, LoggingMethodID, Env->NewStringUTF(message.c_str()));
}

void JReverseLogger::RemoveAndLog(std::string message)
{
	Env->CallStaticVoidMethod(LoggerClass, RemLoggingID, Env->NewStringUTF(message.c_str()));
}

int JReverseLogger::GetPipeCallBackLimit()
{
	return Env->GetStaticIntField(LoggerClass, PipeCallBackLimitID);
}
