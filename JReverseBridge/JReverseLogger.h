#pragma once
#include "jni.h"
#include <string>

class JReverseLogger
{
public:
	JReverseLogger(JNIEnv* env);
	void Log(std::string message);
	void RemoveAndLog(std::string message);
	int GetPipeCallBackLimit();

private:
	JNIEnv* Env;
	jclass LoggerClass;
	jmethodID LoggingMethodID;
	jmethodID RemLoggingID;
	jfieldID PipeCallBackLimitID;
};



