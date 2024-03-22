#include "SharedMemManager.h"
#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>

void SharedMemManager::Setup()
{
    using namespace boost::interprocess;
    //Create a native windows shared memory object.
    windows_shared_memory shmm(create_only, "StringSharedMemory", read_write, 1000);

    //Map the whole shared memory in this process
    mapped_region regionn(shmm, read_write);

    //Write all the memory to 1
    std::memset(regionn.get_address(), 1, regionn.get_size());

    shm = &shmm;
    region = &regionn;
}

std::string SharedMemManager::ReadString()
{
    return std::string(static_cast<const char*>(region->get_address()));
}

void SharedMemManager::WriteString(std::string message)
{
    std::strcpy(static_cast<char*>(region->get_address()), message.c_str());
}
