#include "ClassFileManager.h"
#include "ClassFile.h"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <windows.h>
#include <synchapi.h>
#include <mutex> // For std::mutex

std::mutex mtx;

std::vector<ClassFile> ClassFileManager::ClassFilesList = std::vector<ClassFile>{};

ClassFile ClassFileManager::FindClassFile(std::string name)
{
    for (ClassFile clafil : ClassFileManager::ClassFilesList) {
        if (clafil.classname == name) return clafil;
    }
    return ClassFile{ "NOT FOUND", "NOT FOUND" };
}

void ClassFileManager::init()
{
    std::cout << "ClassFileManager::init() not needed\n";
}

void ClassFileManager::AddClassFile(ClassFile classfile)
{
    std::lock_guard<std::mutex> lock(mtx);
    ClassFileManager::ClassFilesList.push_back(classfile);
}

std::vector<std::string> ClassFileManager::GetUnknownClassFiles()
{
    std::vector<std::string> ReturnVec;
    for (ClassFile clafil : ClassFileManager::ClassFilesList) {
        if (clafil.classname == "unknown") ReturnVec.push_back(clafil.bytecodes);
    }
    if (ReturnVec.size() <= 0) return std::vector<std::string>{"NO CLASSES"};
    return ReturnVec;
}

void ClassFileManager::DeleteUnknownClassFiles()
{
    std::lock_guard<std::mutex> lock(mtx);
    for (int i = 0; i < ClassFileManager::ClassFilesList.size(); i++) {
        if (ClassFileManager::ClassFilesList[i].classname == "unknown") ClassFileManager::ClassFilesList.erase(ClassFileManager::ClassFilesList.begin() + i);
    }
}
