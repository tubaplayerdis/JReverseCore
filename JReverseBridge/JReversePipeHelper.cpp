#include "JReversePipeHelper.h"
#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>

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