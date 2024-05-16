#include "ClassFileManager.h"
#include "ClassFile.h"
#include <jni.h>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <windows.h>
#include <synchapi.h>
#include <mutex> // For std::mutex

std::mutex mtx;

std::vector<ClassFile> ClassFileManager::ClassFilesList = std::vector<ClassFile>{};

std::vector<jclass> ClassFileManager::ClassRefrencesList = std::vector<jclass>{};

JNIEnv* jniEnv = nullptr;

ClassFile ClassFileManager::FindClassFile(std::string name)
{
    std::cout << "Searching for class file: " << name << std::endl;
    for (ClassFile clafil : ClassFileManager::ClassFilesList) 
    {
        if (clafil.classname == name) return clafil;
    }
    return ClassFile{ "NOT FOUND", "NOT FOUND" };
}

int ClassFileManager::FindClassFileIndex(std::string name)
{
    //More Efficent way
    for (int i = 0; i < ClassFileManager::ClassFilesList.size(); i++) 
    {
        if (ClassFileManager::ClassFilesList[i].classname == name) return i;
    }
    return -1;
}

void ClassFileManager::init()
{
    std::cout << "ClassFileManager::init() not needed\n";
}

void ClassFileManager::AddClassFile(ClassFile classfile)
{
    int index = ClassFileManager::FindClassFileIndex(classfile.classname);
    std::lock_guard<std::mutex> lock(mtx);
    if (index != -1) //Class File Exists, so overwrite 
    {
        ClassFileManager::ClassFilesList[index] = classfile;
        return;
    }
    ClassFileManager::ClassFilesList.push_back(classfile);
}

std::vector<std::string> ClassFileManager::GetUnknownClassFiles()
{
    std::vector<std::string> ReturnVec;
    for (ClassFile clafil : ClassFileManager::ClassFilesList) {
        if (clafil.classname == "unknown") ReturnVec.push_back(clafil.bytecodes);
    }
    if (ReturnVec.size() <= 0) return std::vector<std::string>{"NO CLASSES"};
    std::cout << "#of Unknown Class Files: " << ReturnVec.size() << std::endl;
    return ReturnVec;
}

void ClassFileManager::DeleteUnknownClassFiles()
{
    std::lock_guard<std::mutex> lock(mtx);
    for (int i = 0; i < ClassFileManager::ClassFilesList.size(); i++) {
        if (ClassFileManager::ClassFilesList[i].classname == "unknown") ClassFileManager::ClassFilesList.erase(ClassFileManager::ClassFilesList.begin() + i);
    }
}

std::vector<std::string> ClassFileManager::GetClassFileNames()
{
    std::vector<std::string> ReturnVec;
    for (ClassFile clafil : ClassFileManager::ClassFilesList) {
        ReturnVec.push_back(clafil.classname);
    }
    return ReturnVec;
}

void ClassFileManager::SetEnv(JNIEnv* pointer)
{
}

void ClassFileManager::addClassRef(jclass clazz)
{
    std::lock_guard<std::mutex> lock(mtx);
    ClassFileManager::ClassRefrencesList.push_back(clazz);
}

jclass ClassFileManager::findClassRef(const char* name)
{
    return nullptr;
}
