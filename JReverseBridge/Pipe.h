#pragma once
#include <string>
#include <memory.h>
#include <type_traits>
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
    size_t writtenSize;
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

    writtenSize = sizeof(T);

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
    if (sizeof(data) > region.get_size()) {
        throw std::exception("The data is to big for the region!");
    }
    writtenSize = sizeof(data);
    std::memcpy(region.get_address(), &data, sizeof(T));
}

template<typename T>
inline T JReversePipe<T>::ReadPipe()
{
    if (region.get_address() == nullptr) {
        throw std::exception("address was null");
    }
    T data;
    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
        std::vector<std::string> ndata;
        std::memcpy(&ndata, region.get_address(), ndata.size() * sizeof(std::string));
        data = ndata;
    }
    else
    {
        std::memcpy(&data, region.get_address(), sizeof(T));
    }
    return data;
}

template<typename T>
inline JReversePipeInfo JReversePipe<T>::GetInfo()
{
    JReversePipeInfo returnable;
    returnable.Name = std::string(shm.get_name());
    returnable.Size = region.get_size();
    returnable.Mode = shm.get_mode();
    return returnable;
}



