#pragma once
#include <string>
#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/any/basic_any.hpp>
#include "JReversePipeInfo.h"
#include "Pipe.h"


class PipeAPI
{
private:
    static void NoneReturnPipe();
public:
    static bool isReturnPipeNone();
    static const std::vector<std::string> noneVec;
    static const std::string noneStr;
    static void setCritPipes();
    static std::vector<std::string> GetAllPipeNames();//Read from PipeNamePipe
    static JReversePipeInfo GetPipeInfo(std::string name);
    static bool CheckForPipe(std::string name);
    static std::string GetPipeType(std::string name);
    static void AddPipeToList(std::string name);
    static std::vector<std::string> ReadReturnPipeAR();
    //Critical Pipes
    static JReversePipe<std::string> FunctionPipe;
    static JReversePipe<std::vector<std::string>> FunctionArgPipe;
    static JReversePipe<std::vector<std::string>> ReturnPipe;
    static JReversePipe<std::vector<std::string>> PipeNamePipe;
    static JReversePipe<std::vector<std::string>> StartupRulesPipe;
    static JReversePipe<std::vector<std::string>> SettingsPipe;
    static JReversePipe<std::string> CommunicationPipe;
    //ref list
};

