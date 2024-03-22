#include "PipeManager.h"
#include "pch.h"
#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <iostream>

bool PipeManager::isDebug;
boost::interprocess::windows_shared_memory *PipeManager::shm;
boost::interprocess::mapped_region *PipeManager::region;

int PipeManager::SetupClient()
{
    using namespace boost::interprocess;
    //Open already created shared memory object.
    windows_shared_memory shmm(open_only, "StringSharedMemory", read_write);

    //Map the whole shared memory in this process
    mapped_region regionn(shmm, read_only);

    //Check that memory was initialized to 1
    char* mem = static_cast<char*>(regionn.get_address());
    for (std::size_t i = 0; i < regionn.get_size(); ++i)
        if (*mem++ != 1)
            return 1;   //Error checking memory

    shm = &shmm;
    region = &regionn;
    isDebug = true;

    return 0;
}

std::string PipeManager::ReadString()
{
    if (!isDebug) {
        return std::string(static_cast<const char*>(region->get_address()));
    }
    else
    {
        std::string what = std::string(static_cast<const char*>(region->get_address()));
        std::cout << what << std::endl;
        return what;
    }
    
}

void PipeManager::WriteString(std::string data)
{
    std::strcpy(static_cast<char*>(region->get_address()), data.c_str());
}
