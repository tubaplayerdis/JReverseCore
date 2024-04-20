#pragma once
#include <jni.h>

class JythonInterpreter
{
public:
	JythonInterpreter(JNIEnv* ENV, jobject OutputStream, jclass PythonInterpreterCalss);
	void RunScript(const char* path);
	void Close();
private:
	bool hasClosed;
	JNIEnv* env;
	jobject InterpreterOBJ;
	jmethodID runScript;
	jmethodID close;
};