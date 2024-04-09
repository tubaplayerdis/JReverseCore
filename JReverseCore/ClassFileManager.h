#pragma once
#include <vector>
#include <string>
#include "ClassFile.h"

class ClassFileManager
{
public:
	static std::vector<ClassFile> ClassFilesList; 
	static ClassFile FindClassFile(std::string name);
	static void init();
	static void AddClassFile(ClassFile classfile);
	static std::vector<std::string> GetUnknownClassFiles();
	static void DeleteUnknownClassFiles();
};