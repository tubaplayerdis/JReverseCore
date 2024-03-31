#pragma once
#include <string>
#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/any/basic_any.hpp>
#include "JReversePipeInfo.h"
#include "JReversePipeHelper.h"

template<typename T>
class JReversePipe
{
public:
    JReversePipe(std::string Name, boost::interprocess::mode_t Mode, int Size);
    void WritePipe(const T& data);
    T ReadPipe();
    JReversePipeInfo GetInfo();
private:
    boost::interprocess::windows_shared_memory shm;
    boost::interprocess::mapped_region region;
};

template<typename T>
inline JReversePipe<T>::JReversePipe(std::string Name, boost::interprocess::mode_t Mode, int Size)
{
    //This was stolen
    using namespace boost::interprocess;
    //Create a native windows shared memory object.
    windows_shared_memory ghm(create_only, Name.c_str(), Mode, 1000);

    //Map the whole shared memory in this process
    mapped_region regionn(ghm, Mode);

    //Write all the memory to 1
    std::memset(regionn.get_address(), 1, regionn.get_size());

    ghm.swap(shm);
    regionn.swap(region);

    //Inform the PipeNamePipe
    /*
    if (Name != "CriticalPipeNamePipe" || Name != "CriticalFunctionPipe" || Name != "CriticalCommunicationPipe") {
        std::vector<std::string> curinfo = PipeAPI::PipeNamePipe.ReadPipe();
        curinfo.push_back(Name);
        PipeAPI::PipeNamePipe.WritePipe(curinfo);
    }
    */

}

template<typename T>
inline void JReversePipe<T>::WritePipe(const T& data)
{
    std::memcpy(region.get_address(), &data, sizeof(data));
}

template<typename T>
inline T JReversePipe<T>::ReadPipe()
{
    T data;
    std::memcpy(&data, region.get_address(), sizeof(data));
    return data;
}

template<typename T>
inline JReversePipeInfo JReversePipe<T>::GetInfo()
{
    JReversePipeInfo returnable;
    returnable.Name = std::string(shm.get_name());
    returnable.Size = shm.get_size();
    returnable.Mode = shm.get_mode();
    return returnable;
}



