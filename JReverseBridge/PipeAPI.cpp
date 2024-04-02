#include "PipeAPI.h"
#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/any/basic_any.hpp>
#include "Pipe.h"
#include "JReversePipeInfo.h"

JReversePipe<std::string> PipeAPI::FunctionPipe = JReversePipe<std::string>("CriticalCommunicationPipe", boost::interprocess::read_write, 1000);
JReversePipe<std::vector<std::string>> PipeAPI::PipeNamePipe = JReversePipe<std::vector<std::string>>("CriticalPipeNamePipe", boost::interprocess::read_write, 1000);
JReversePipe<std::string> PipeAPI::CommunicationPipe = JReversePipe<std::string>("CriticalFunctionPipe", boost::interprocess::read_write, 1000);


void PipeAPI::setCritPipes()
{
    std::vector<std::string> cur;
    cur.push_back(std::string("CriticalFunctionPipe"));
    cur.push_back(std::string("CriticalPipeNamePipe"));
    cur.push_back(std::string("CriticalCommunicationPipe"));
    PipeAPI::PipeNamePipe.WritePipe(cur);
}

std::vector<std::string> PipeAPI::GetAllPipeNames()
{
    return PipeAPI::PipeNamePipe.ReadPipe();
}

std::vector<JReversePipeInfo> PipeAPI::GetAllPipesInfo()
{
    return std::vector<JReversePipeInfo>();
}
