#pragma once
#include "jni.h"
#include <string>

class JReverseLogger
{
public:
	JReverseLogger(JNIEnv* env);
	void Log(std::string message);
	

private:
	JNIEnv* Env;
	jclass LoggerClass;
	jmethodID LoggingMethodID;
};



