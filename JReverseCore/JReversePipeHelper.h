#pragma once
#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>

class JReversePipeHelpers
{
public:
    static boost::interprocess::mode_t getMode(int arg);
};
