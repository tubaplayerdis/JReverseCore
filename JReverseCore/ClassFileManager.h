#pragma once
#include <vector>
#include <string>
#include <jni.h>
#include "ClassFile.h"

class ClassFileManager
{
public:
	static std::vector<ClassFile> ClassFilesList; 
	static ClassFile FindClassFile(std::string name);
	static int FindClassFileIndex(std::string name);
	static void init();
	static void AddClassFile(ClassFile classfile);
	static std::vector<std::string> GetUnknownClassFiles();
	static void DeleteUnknownClassFiles();
	static std::vector<std::string>GetClassFileNames();

	//JClass Refs
	static void SetEnv(JNIEnv* pointer);
	static void addClassRef(jclass clazz);
	static jclass findClassRef(const char* name);
	static std::vector<jclass> ClassRefrencesList;
};