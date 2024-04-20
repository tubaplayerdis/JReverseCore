#include "JythonInterpreter.h"
#include <iostream>

JythonInterpreter::JythonInterpreter(JNIEnv* ENV, jobject OutputStream, jclass PythonInterpreterCalss)
{
	env = ENV;
    jmethodID PYconstructor = env->GetMethodID(PythonInterpreterCalss, "<init>", "()V");
    jobject PYInterpreterOBJ = env->NewObject(PythonInterpreterCalss, PYconstructor);

    std::cout << "Constructed A Python Interpreter" << std::endl;

    //Link the String Writer to the PYInterpreter
    jmethodID PYInterpreterSetOut = env->GetMethodID(PythonInterpreterCalss, "setOut", "(Ljava/io/Writer;)V");
    jmethodID PYInterpreterSetErr = env->GetMethodID(PythonInterpreterCalss, "setErr", "(Ljava/io/Writer;)V");
    jmethodID PYInterpreterClose = env->GetMethodID(PythonInterpreterCalss, "close", "()V");
    env->CallVoidMethod(PYInterpreterOBJ, PYInterpreterSetOut, OutputStream);
    env->CallVoidMethod(PYInterpreterOBJ, PYInterpreterSetErr, OutputStream);

    std::cout << "Linked String Writer" << std::endl;

    runScript = env->GetMethodID(PythonInterpreterCalss, "execfile", "(Ljava/lang/String;)V");
    close = PYInterpreterClose;
    InterpreterOBJ = PYInterpreterOBJ;
    hasClosed = false;

    std::cout << "Created A Jython Interpreter!" << std::endl;
}

void JythonInterpreter::RunScript(const char* path)
{
    if (hasClosed) return;
    jstring ArgsString = env->NewStringUTF(path);
    env->CallVoidMethod(InterpreterOBJ, runScript, ArgsString);
}

void JythonInterpreter::Close()
{
    env->CallVoidMethod(InterpreterOBJ, close);
}

