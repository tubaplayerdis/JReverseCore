#include "PipeAPI.h"
#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/any/basic_any.hpp>

JReversePipe<std::string> FunctionPipe = JReversePipe<std::string>("CriticalFunctionPipe", 1, 1000);
JReversePipe<std::vector<std::string>> PipeNamePipe = JReversePipe<std::vector<std::string>>("CriticalPipeNamePipe", 1, 1000);
static JReversePipe<std::string> CommunicationPipe = JReversePipe<std::string>("CriticalCommunicationPipe", 1, 1000);;


boost::interprocess::mode_t JReversePipeHelpers::getMode(int arg)
{
    switch (arg)
    {
    case 0:
        return boost::interprocess::mode_t::read_only;
    case 1:
        return boost::interprocess::mode_t::read_write;
    default:
        return boost::interprocess::mode_t::read_only;
    }
}
