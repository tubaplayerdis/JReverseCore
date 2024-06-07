#pragma once
#include <string>
#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>

struct JReversePipeInfo
{
    std::string Name;
    long long int Size;
    boost::interprocess::mode_t Mode;
};
