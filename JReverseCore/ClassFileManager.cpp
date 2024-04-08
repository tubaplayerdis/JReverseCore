#include "ClassFileManager.h"
#include "ClassFile.h"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <windows.h>
#include <synchapi.h>

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
    Sleep(1);
    ClassFileManager::ClassFilesList.push_back(classfile);
}
