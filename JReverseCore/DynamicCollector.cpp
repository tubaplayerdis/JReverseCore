#include "DynamicCollector.h"
#include <string>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <fstream>
#include <mutex>
#include "JReverseSettings.h"


std::string DynamicCollector::collectionpath = "invalid";
bool DynamicCollector::_docollection = false;
std::vector<DynamicCollectionExclusion> DynamicCollector::exclusionlist = std::vector<DynamicCollectionExclusion>{};

void DynamicCollector::init(std::string path)
{
	//Check if System is enabeled
	if (!JReverseStartupSettings::isDynamicClassFileCollection) {
		std::cout << "Dynamic Collection Not Requested." << std::endl;
		_docollection = false;
		return;
	}
	//Validate Path
	if (!std::filesystem::exists(path)) {
		std::cout << "Dynamic Collection Path Does not exist! Dymanic Collection Initalization Stopped!" << std::endl;
		_docollection = false;
		return;
	}

	collectionpath = path;
	_docollection = true;

	std::cout << "Dynamic Collection System Initalized At: " << collectionpath << std::endl;
	//Collect Existing Classes 
}

void DynamicCollector::ChangePath(std::string path)
{
	if (!std::filesystem::exists(path)) {
		std::cout << "Dynamic Collection Path Does not exist! Dymanic Collection Path Not Changed!" << std::endl;
		return;
	}

	_docollection = true;
	collectionpath = path;
}

void DynamicCollector::Collect(ClassFile classfile)
{
	DynamicCollector::Collect(classfile.classname, classfile.bytecodes);
}

void DynamicCollector::Collect(std::string name, std::string bytecodes)
{
	if (!_docollection) return;

	//Check for exclusion
	for (DynamicCollectionExclusion exclusion : exclusionlist) {
		switch (exclusion.type)
		{
		case StartsWith:
			if (name._Starts_with(exclusion.name)) {
				if (JReverseStartupSettings::isClassFileLoadMessages) std::cout << "Dynamic Class File Collection Exclusion Detected!" << std::endl;
				return;
			}
			break;
		case Contains:
			if (name.find(exclusion.name) != std::string::npos) {
				if (JReverseStartupSettings::isClassFileLoadMessages) std::cout << "Dynamic Class File Collection Exclusion Detected!" << std::endl;
				return;
			}
			break;
		case Both:
			if (name._Starts_with(exclusion.name)) {
				if (JReverseStartupSettings::isClassFileLoadMessages) std::cout << "Dynamic Class File Collection Exclusion Detected!" << std::endl;
				return;
			}
			else if (name.find(exclusion.name) != std::string::npos) {
				if (JReverseStartupSettings::isClassFileLoadMessages) std::cout << "Dynamic Class File Collection Exclusion Detected!" << std::endl;
				return;
			}
			break;
		default:
			//Ignore the exclusion
			break;
		}
	}

	//Create folders and file based of of name
	std::size_t lastSlashPos = name.find_last_of('/');
	if (lastSlashPos == std::string::npos) {
		if (JReverseStartupSettings::isClassFileLoadMessages) std::cerr << "Invalid path format. No slashes found." << std::endl;
		return;
	}

	//Calcualte Hex Data
	std::basic_string<uint8_t> bytes;

	for (size_t i = 0; i < bytecodes.length(); i += 2)
	{
		uint16_t byte;
		std::string nextbyte = bytecodes.substr(i, 2);
		std::istringstream(nextbyte) >> std::hex >> byte;
		bytes.push_back(static_cast<uint8_t>(byte));
	}
	std::string result(begin(bytes), end(bytes));


	// Extract the directory path and file name
	std::string dirPath = name.substr(0, lastSlashPos);
	std::string fileName = name.substr(lastSlashPos + 1) + ".class";

	// Create the full directory path
	std::filesystem::path fullDirPath = std::filesystem::path(collectionpath) / dirPath;

	// Create directories if they do not exist
	if (!std::filesystem::exists(fullDirPath)) {
		std::filesystem::create_directories(fullDirPath);
		if(JReverseStartupSettings::isClassFileLoadMessages) std::cout << "Created directories: " << fullDirPath << std::endl;
	}
	else {
		if (JReverseStartupSettings::isClassFileLoadMessages) std::cout << "Directories already exist: " << fullDirPath << std::endl;
	}

	// Create the file within the created directory
	std::filesystem::path filePath = fullDirPath / fileName;
	std::ofstream outFile(filePath, std::ios::binary);
	if (outFile) {
		if (JReverseStartupSettings::isClassFileLoadMessages) std::cout << "Created or opened file: " << filePath << std::endl;
		outFile << result;
		outFile.close();
	}
	else {
		if (JReverseStartupSettings::isClassFileLoadMessages) std::cerr << "Failed to create file!: " << filePath << std::endl;
	}
}

void DynamicCollector::Collect(std::string name, unsigned char* bytecodes, int bytecodeslen)
{
	std::vector<unsigned char> bytecode(bytecodes, bytecodes + bytecodeslen);
	std::stringstream toadd;
	for (unsigned char byte : bytecode) {
		toadd << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
	}
	DynamicCollector::Collect(name, toadd.str());
}

size_t DynamicCollector::FindExclsuionIndex(std::string name)
{
	for (int i = 0; i < exclusionlist.size(); i++) {
		if (name == exclusionlist[i].name) return i;
	}
	return -1;
}


void DynamicCollector::ListExclusions()
{
	std::cout << "DYNAMIC CLASS FILE COLLECTION EXCLUSIONS" << std::endl;
	for (int i = 0; i < exclusionlist.size(); i++) {
		switch (exclusionlist[i].type)
		{
		case StartsWith:
			std::cout << "I: " << i << " NAME: " << exclusionlist[i].name << "   TYPE: " << "StartsWith" << std::endl;
			break;
		case Contains:
			std::cout << "I: " << i << "NAME: " << exclusionlist[i].name << "   TYPE: " << "Contains" << std::endl;
			break;
		case Both:
			std::cout << "I: " << i << "NAME: " << exclusionlist[i].name << "   TYPE: " << "Both" << std::endl;
			break;
		default:
			std::cout << "I: " << i << "NAME: " << exclusionlist[i].name << "   TYPE: " << "Undefined" << std::endl;
			break;
		}
	}
}

void DynamicCollector::_checkpath()
{
	if (!std::filesystem::exists(collectionpath)) {
		std::cout << "Dynamic Collection Path Does not exist! Dymanic Collection Stopped!" << std::endl;
		_docollection = false;
		return;
	}
}
