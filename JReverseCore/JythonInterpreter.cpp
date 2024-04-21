#include "JythonInterpreter.h"
#include <iostream>
#include <chrono>
#include <sstream>
#include "JythonManager.h"

const char* CheckJNIErrorBetter(JNIEnv* jniEnv) {
    jthrowable ex = nullptr;
    if (jniEnv->ExceptionCheck()) { std::cout << "A JVM EXEPTION WAS FOUND!" << std::endl; ex = jniEnv->ExceptionOccurred(); }
    if (ex == nullptr) return "No Error";

    jclass exceptionClass = jniEnv->GetObjectClass(ex);

    // Get the method ID of the getMessage() method of Throwable
    jmethodID getMessageMethod = jniEnv->GetMethodID(exceptionClass, "getMessage", "()Ljava/lang/String;");

    // Call getMessage() method
    jstring messageObj = (jstring)jniEnv->CallObjectMethod(ex, getMessageMethod);

    // Convert messageObj to C string
    const char* message = jniEnv->GetStringUTFChars(messageObj, nullptr);
    std::cout << "Exception message: " << message << std::endl;

    // Create a StringWriter to capture the stack trace
    jclass stringWriterClass = jniEnv->FindClass("java/io/StringWriter");
    jmethodID stringWriterConstructor = jniEnv->GetMethodID(stringWriterClass, "<init>", "()V");
    jobject stringWriterObject = jniEnv->NewObject(stringWriterClass, stringWriterConstructor);
    jmethodID stringWriterMethod = jniEnv->GetMethodID(stringWriterClass, "toString", "()Ljava/lang/String;");

    // Create a PrintWriter to write to the StringWriter
    jclass printWriterClass = jniEnv->FindClass("java/io/PrintWriter");
    jmethodID printWriterConstructor = jniEnv->GetMethodID(printWriterClass, "<init>", "(Ljava/io/Writer;)V");
    jobject printWriterObject = jniEnv->NewObject(printWriterClass, printWriterConstructor, stringWriterObject);

    // Print the stack trace to the PrintWriter
    jclass throwableClass = jniEnv->FindClass("java/lang/Throwable");
    jmethodID printStackTraceMethod = jniEnv->GetMethodID(throwableClass, "printStackTrace", "(Ljava/io/PrintWriter;)V");
    jniEnv->CallVoidMethod(ex, printStackTraceMethod, printWriterObject);

    // Get the string representation from the StringWriter
    jstring stackTraceString = (jstring)jniEnv->CallObjectMethod(stringWriterObject, stringWriterMethod);

    // Convert the Java string to C-style string
    const char* stackTraceCString = jniEnv->GetStringUTFChars(stackTraceString, NULL);

    // Print the stack trace string to std::cout
    std::cout << "Exception stack trace:\n" << stackTraceCString << std::endl;

    // Release the allocated memory
    jniEnv->ReleaseStringUTFChars(stackTraceString, stackTraceCString);

    // Release the message string
    jniEnv->ReleaseStringUTFChars(messageObj, message);

    jniEnv->ExceptionDescribe();
    jniEnv->ExceptionClear();
    return message;
}


JythonInterpreter::JythonInterpreter(JNIEnv* ENV, jobject outputStream, jclass PythonInterpreterCalss, jmethodID WriteWriterMethod)
{
	env = ENV;
    OutputStream = outputStream;
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
    writeStream = WriteWriterMethod;

    srand(time(NULL));
    ID = rand() % 90000 + 10000;

    std::cout << "Created A Jython Interpreter. ID: " << ID << std::endl;
}

JythonReturns JythonInterpreter::RunScript(const char* path)
{
    if (CheckJNIErrorBetter(env) != "No Error") return JythonReturns::FAILURE;

    if (hasClosed) return JythonReturns::INTERPRETER_NOT_FOUND;

    std::chrono::system_clock::time_point begtime = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(begtime);

    std::stringstream timestring;
    timestring << "Execution of script at: ";
    timestring << std::ctime(&now_time);
    timestring << "Interpreter ID: " << ID << "\n";

    jstring curtime = env->NewStringUTF(timestring.str().c_str());

    env->CallVoidMethod(OutputStream, writeStream, curtime);

    jstring ArgsString = env->NewStringUTF(path);
    env->CallVoidMethod(InterpreterOBJ, runScript, ArgsString);
   
    std::cout << "Executed script from: " << ID << " at: " << path << std::endl;

    if (CheckJNIErrorBetter(env) != "No Error") return JythonReturns::FAILURE;
    return JythonReturns::SUCCESS;
}

void JythonInterpreter::Close()
{
    env->CallVoidMethod(InterpreterOBJ, close);
}

