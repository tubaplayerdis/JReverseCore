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
    cur.push_back(std::string("CriticalFunctionPipe:std::string"));
    cur.push_back(std::string("CriticalPipeNamePipe:std::vector<std::string>"));
    cur.push_back(std::string("CriticalCommunicationPipe:std::string"));
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

bool PipeAPI::CheckForPipe(std::string name)
{
    std::vector<std::string> checklis = PipeNamePipe.ReadPipe();
    for (std::string pipnam : checklis)
    {
        pipnam = pipnam.substr(0, pipnam.find(":", 0));
        if (pipnam == name) return true;
    }
    return false;
}

std::string PipeAPI::GetPipeType(std::string name)
{
    std::vector<std::string> checklis = PipeNamePipe.ReadPipe();
    for (std::string pipnam : checklis)
    {
        std::string check = pipnam.substr(0, pipnam.find(":", 0));
        if (check == name) {
            return pipnam.substr(pipnam.find(":") + 1);
        }
    }
    return "PipeNotFound";
}

void PipeAPI::AddPipeToList(std::string name)
{
    std::vector<std::string> cur = PipeAPI::PipeNamePipe.ReadPipe();
    cur.push_back(name);
    PipeAPI::PipeNamePipe.WritePipe(cur);
}
