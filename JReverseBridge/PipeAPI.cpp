#include "PipeAPI.h"
#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/any/basic_any.hpp>
#include "Pipe.h"
#include "JReversePipeInfo.h"
#include "JReverseLogger.h"

const std::vector<std::string> PipeAPI::noneVec = std::vector<std::string>{ "NONE" };
const std::string PipeAPI::noneStr = "NONE";

JReversePipe<std::string> PipeAPI::FunctionPipe = JReversePipe<std::string>("CriticalFunctionPipe", boost::interprocess::read_write, 1000);
JReversePipe<std::vector<std::string>> PipeAPI::FunctionArgPipe = JReversePipe<std::vector<std::string>>("CriticalFunctionArgPipe", boost::interprocess::read_write, 500000);
JReversePipe<std::vector<std::string>> PipeAPI::ReturnPipe = JReversePipe<std::vector<std::string>>("CriticalReturnPipe", boost::interprocess::read_write, 5000000);
JReversePipe<std::vector<std::string>> PipeAPI::PipeNamePipe = JReversePipe<std::vector<std::string>>("CriticalPipeNamePipe", boost::interprocess::read_write, 1000);
JReversePipe<std::vector<std::string>> PipeAPI::StartupRulesPipe = JReversePipe<std::vector<std::string>>("CriticalStartupPipe", boost::interprocess::read_write, 50000);
JReversePipe<std::vector<std::string>> PipeAPI::SettingsPipe = JReversePipe<std::vector<std::string>>("CriticalSettingsPipe", boost::interprocess::read_write, 50000);
JReversePipe<std::string> PipeAPI::CommunicationPipe = JReversePipe<std::string>("CriticalCommunicationPipe", boost::interprocess::read_write, 1000);


void PipeAPI::NoneReturnPipe()
{
    PipeAPI::ReturnPipe.WritePipe(PipeAPI::noneVec);
}

bool PipeAPI::isReturnPipeNone()
{

    auto vec = PipeAPI::ReturnPipe.ReadPipe();
    if (vec[0] == "NONE") {
        return true;
    }
    return false;
}

void PipeAPI::setCritPipes()
{
    std::vector<std::string> cur;
    cur.push_back(std::string("CriticalFunctionPipe:std::string"));
    cur.push_back(std::string("CriticalFunctionArgPipe:std::vector<std::string>"));
    cur.push_back(std::string("CriticalReturnPipe:std::vector<std::string>"));
    cur.push_back(std::string("CriticalPipeNamePipe:std::vector<std::string>"));
    cur.push_back(std::string("CriticalCommunicationPipe:std::string"));
    PipeAPI::PipeNamePipe.WritePipe(cur);
    PipeAPI::NoneReturnPipe();
    PipeAPI::FunctionPipe.WritePipe(PipeAPI::noneStr);
}

std::vector<std::string> PipeAPI::GetAllPipeNames()
{
    return PipeAPI::PipeNamePipe.ReadPipe();
}

JReversePipeInfo PipeAPI::GetPipeInfo(std::string name)
{
    JReversePipeInfo defsend;
    defsend.Mode = boost::interprocess::invalid_mode;
    defsend.Name = "null";
    defsend.Size = 0;
    if (name == "CriticalFunctionPipe") return PipeAPI::FunctionPipe.GetInfo();
    else if (name == "CriticalFunctionArgPipe") return PipeAPI::FunctionPipe.GetInfo();
    else if (name == "CriticalReturnPipe") return PipeAPI::FunctionPipe.GetInfo();
    else if (name == "CriticalPipeNamePipe") return PipeAPI::FunctionPipe.GetInfo();
    else if (name == "CriticalCommunicationPipe") return PipeAPI::FunctionPipe.GetInfo();
    else return defsend;
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

void PipeAPI::ResizePipe(std::string name, int size, JNIEnv* env)
{
    JReverseLogger logger = JReverseLogger(env);
    logger.Log("Resizing: " + name);
    std::string cb;
    if (name == "CriticalFunctionPipe") {
        PipeAPI::FunctionPipe.Grow(size);
    }
    else if (name == "CriticalFunctionArgPipe") {
        PipeAPI::FunctionArgPipe.Grow(size);
    }
    else if (name == "CriticalReturnPipe") {
        PipeAPI::ReturnPipe.Grow(size);
    }
    else if (name == "CriticalPipeNamePipe") {
        PipeAPI::PipeNamePipe.Grow(size);
    }
    else if (name == "CriticalStartupPipe") {
        PipeAPI::StartupRulesPipe.Grow(size);
    }
    else if (name == "CriticalSettingsPipe") {
        PipeAPI::SettingsPipe.Grow(size);
    }
    logger.Log("Reized Pipe Callback: "+cb);
}

std::vector<std::string> PipeAPI::ReadReturnPipeAR()
{
    std::vector<std::string> ret = PipeAPI::ReturnPipe.ReadPipe();
    PipeAPI::NoneReturnPipe();
    return ret;
}
