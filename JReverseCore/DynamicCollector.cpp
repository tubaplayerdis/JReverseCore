#include "DynamicCollector.h"
#include <string>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <fstream>
#include "JReverseSettings.h"

std::string DynamicCollector::collectionpath = "invalid";
bool DynamicCollector::_docollection = false;

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

	//Create folders and file based of of name
	std::size_t lastSlashPos = name.find_last_of('/');
	if (lastSlashPos == std::string::npos) {
		std::cerr << "Invalid path format. No slashes found." << std::endl;
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
		std::cout << "Created directories: " << fullDirPath << std::endl;
	}
	else {
		std::cout << "Directories already exist: " << fullDirPath << std::endl;
	}

	// Create the file within the created directory
	std::filesystem::path filePath = fullDirPath / fileName;
	std::ofstream outFile(filePath, std::ios::binary);
	if (outFile) {
		std::cout << "Created file: " << filePath << std::endl;
		outFile << result;
		outFile.close();
	}
	else {
		std::cerr << "Failed to create file!: " << filePath << std::endl;
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

void DynamicCollector::_checkpath()
{
	if (!std::filesystem::exists(collectionpath)) {
		std::cout << "Dynamic Collection Path Does not exist! Dymanic Collection Stopped!" << std::endl;
		_docollection = false;
		return;
	}
}
