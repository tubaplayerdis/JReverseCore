#include "SharedMemManager.h"
#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>

boost::interprocess::windows_shared_memory SharedMemManager::shm;
boost::interprocess::mapped_region SharedMemManager::region;

void SharedMemManager::Setup()
{
    using namespace boost::interprocess;
    //Create a native windows shared memory object.
    windows_shared_memory ghm(create_only, "StringSharedMemory", read_write, 1000);

    //Map the whole shared memory in this process
    mapped_region regionn(ghm, read_write);

    //Write all the memory to 1
    std::memset(regionn.get_address(), 1, regionn.get_size());

    ghm.swap(shm);
    regionn.swap(region);
}

std::string SharedMemManager::ReadString()
{
    return std::string(static_cast<const char*>(region.get_address()));
}

void SharedMemManager::WriteString(std::string message)
{
    std::strcpy(static_cast<char*>(region.get_address()), message.c_str());
}
