#pragma once
#include <string>
#include "ClassFile.h"
class DynamicCollector
{
public:
	static void init(std::string path);
	static void ChangePath(std::string path);
	static void Collect(ClassFile classfile);
	static void Collect(std::string name, std::string bytecodes);
	static void Collect(std::string name, unsigned char* bytecodes, int bytecodeslen);
private:
	static std::string collectionpath;
	static void _checkpath();
	static bool _docollection;
};

