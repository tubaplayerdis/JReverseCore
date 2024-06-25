#pragma once
#include <string>
#include <vector>
#include "ClassFile.h"
#include "DynamicCollectionExclusion.h"
class DynamicCollector
{
public:
	static void init(std::string path);
	static void ChangePath(std::string path);
	static void Collect(ClassFile classfile);
	static void Collect(std::string name, std::string bytecodes);
	static void Collect(std::string name, unsigned char* bytecodes, int bytecodeslen);
	static size_t FindExclsuionIndex(std::string name);
	static void ListExclusions();
	static std::vector<DynamicCollectionExclusion> exclusionlist;
private:
	static std::string collectionpath;
	static void _checkpath();
	static bool _docollection;
};

