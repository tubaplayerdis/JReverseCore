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
