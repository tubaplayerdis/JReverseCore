#pragma once
#include <string>
#include <memory.h>
#include <type_traits>
#include <algorithm>
#include <iterator>
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

    //ADD BACK
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

    //writtenSize = sizeof(data);

    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
        std::vector<std::string> vec = data;
        void* address = region.get_address();

        // Calculate the total size needed for the strings
        std::size_t totalSize = 0;
        for (const std::string& str : vec) {
            totalSize += str.size() + 1; // Include null terminator
        }

        // Write the strings to the mapped region
        char* ptr = static_cast<char*>(address);
        for (const std::string& str : vec) {
            std::size_t size = str.size();
            std::memcpy(ptr, str.c_str(), size); // Copy the string data
            ptr += size;
            *ptr++ = '\0'; // Null terminate the string
        }
        *ptr = '\0';
        return;
    }
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
        void* address = region.get_address();

        // Determine the size of the mapped data
        std::size_t size = region.get_size();
        std::vector<std::string> myStringVector;
        try
        {
            // Check if the mapped region is empty
            if (size == 0) {
               std::runtime_error("Mapped region is empty.");
            }

            // Read and deserialize the strings from the mapped region
            std::vector<std::string> vec;
            char* ptr = static_cast<char*>(address);
            while (*ptr != '\0') { // Check for null terminator at the beginning of each string
                // Find the null terminator
                char* endPtr = std::strchr(ptr, '\0');
                if (endPtr == nullptr) {
                    // Null terminator not found within the mapped region
                    std::runtime_error("Null terminator not found within the mapped region.");
                }

                // Read the string and add it to the vector
                std::string str(ptr, endPtr - ptr);
                vec.push_back(str);
                ptr = endPtr + 1; // Move to the next string (skip null terminator)
            }
            myStringVector = vec;
        }
        catch (std::runtime_error e)
        {
            myStringVector.push_back(std::string(e.what()));
        }
        data = myStringVector;
        
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



