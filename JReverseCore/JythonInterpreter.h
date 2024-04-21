#pragma once
#include <jni.h>
#include "JythonReturns.h"

class JythonInterpreter
{
public:
	JythonInterpreter(JNIEnv* ENV, jobject OutputStream, jclass PythonInterpreterCalss, jmethodID WriteWriterMethod);
	JythonReturns RunScript(const char* path);
	void Close();
	int ID;
private:
	bool hasClosed;
	JNIEnv* env;
	jobject InterpreterOBJ;
	jobject OutputStream;
	jmethodID runScript;
	jmethodID close;
	jmethodID writeStream;
};