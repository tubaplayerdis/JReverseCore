#pragma once
#include <string>
#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/any/basic_any.hpp>
#include "JReversePipeInfo.h"
#include "Pipe.h"


class PipeAPI
{
public:
    static void setCritPipes();
    static std::vector<std::string> GetAllPipeNames();//Read from PipeNamePipe
    static std::vector<JReversePipeInfo> GetAllPipesInfo();
    //Critical Pipes
    static JReversePipe<std::string> FunctionPipe;
    static JReversePipe<std::vector<std::string>> PipeNamePipe;
    static JReversePipe<std::string> CommunicationPipe;
};

