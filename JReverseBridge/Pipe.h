#pragma once
#include <algorithm>
#include <string>
#include <memory.h>
#include <type_traits>
#include <iterator>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/managed_mapped_file.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <cassert>
#include <boost/any/basic_any.hpp>
#include "JReversePipeInfo.h"
#include "JReversePipeHelper.h"

// Alias for the allocator for strings in shared memory
typedef boost::interprocess::allocator<char, boost::interprocess::managed_shared_memory::segment_manager> CharAllocator;
typedef boost::interprocess::basic_string<char, std::char_traits<char>, CharAllocator> ShmString;

// Alias for the allocator for vector of strings in shared memory
typedef boost::interprocess::allocator<ShmString, boost::interprocess::managed_shared_memory::segment_manager> StringAllocator;
typedef boost::interprocess::vector<ShmString, StringAllocator> ShmStringVector;


template<typename T>
class JReversePipe
{
public:
    JReversePipe(std::string Name, boost::interprocess::mode_t Mode, int Size);
    void WritePipe(const T& data);
    T ReadPipe();
    JReversePipeInfo GetInfo();
    void Grow(long long int size);
    void Reload(long long int size);
    void Free();
private:
    boost::interprocess::managed_shared_memory shm;
    void* address;
    size_t writtenSize;
    std::string name;
    boost::interprocess::mode_t mode;
};

template<typename T>
inline JReversePipe<T>::JReversePipe(std::string Name, boost::interprocess::mode_t Mode, int Size)
{
    //Store for resize data
    name = Name;
    mode = Mode;
    using namespace boost::interprocess;

    //Create a native windows shared memory object.
    managed_shared_memory ghm(open_or_create, Name.c_str(), Size);

    ghm.swap(shm);
    address = shm.get_address();

    writtenSize = sizeof(T);

}

template<typename T>
inline void JReversePipe<T>::WritePipe(const T& data)
{
    if (sizeof(data) > shm.get_size()) {
        throw std::exception("The data is to big for the region!");
    }

    boost::interprocess::interprocess_mutex* mutex = shm.find_or_construct<boost::interprocess::interprocess_mutex>("MUTEX")();

    //writtenSize = sizeof(data);

    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
        std::vector<std::string> vec = data;

        // Calculate the total size needed for the strings
        std::size_t totalSize = 0;
        for (const std::string& str : vec) {
            totalSize += str.size() + 1; // Include null terminator
        }

        auto* segment_manager = shm.get_segment_manager();


        StringAllocator string_allocator(segment_manager);
        ShmStringVector* shm_vector = shm.find_or_construct<ShmStringVector>("MAIN")(string_allocator);

        mutex->lock();

        shm_vector->clear();  

        int d = vec.size();
        int i = 0;
        try {
            for (const std::string& str : vec) {
                ShmString shm_str(str.c_str(), string_allocator);
                shm_vector->push_back(shm_str);
                i++;
            }
        }
        catch (boost::interprocess::bad_alloc e) {
            
        }

        mutex->unlock();

        return;
    }
    else if constexpr (std::is_same_v<T, std::string>) {
        std::string input = data;
        auto* seg_man = shm.get_segment_manager();
        CharAllocator char_allocator(seg_man);
        ShmString* shm_string = shm.find_or_construct<ShmString>("MAIN")(char_allocator);
        mutex->lock();
        *shm_string = ShmString(input.c_str(), char_allocator);
        mutex->unlock();

        return;
    }
    //Generic Copy
    mutex->lock();
    std::memcpy(address, &data, sizeof(T));
    mutex->unlock();
}

template<typename T>
inline T JReversePipe<T>::ReadPipe()
{
    if (shm.get_address() == nullptr) {
        throw std::exception("address was null");
    }
    T data;

    boost::interprocess::interprocess_mutex* mutex = shm.find_or_construct<boost::interprocess::interprocess_mutex>("MUTEX")();

    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
        // Determine the size of the mapped data
        std::size_t size = shm.get_size();
        std::vector<std::string> result;
        try
        {
            // Check if the region is empty
            if (size == 0) {
               boost::interprocess::interprocess_exception("Region is empty.");
            }

            // Find the vector in the shared memory
            ShmStringVector* shm_vector = shm.find<ShmStringVector>("MAIN").first;

            mutex->lock();

            if (shm_vector) {
                for (const auto& shm_str : *shm_vector) {
                    result.emplace_back(shm_str.c_str());
                }
            }
            else
            {
                result.push_back("NONE");
            }
        }
        catch (boost::interprocess::interprocess_exception e)
        {
            result.push_back(std::string(e.what()));
        }
        data = result;
        
    }
    else if constexpr (std::is_same_v <T, std::string>) {
        ShmString* shm_string = shm.find<ShmString>("MAIN").first;
        mutex->lock();

        if (shm_string) {
            data = std::string(shm_string->c_str());
        }
        else {
            data = "NONE";
        }
    }
    else
    {   
        std::memcpy(&data, shm.get_address(), sizeof(T));
    }

    mutex->unlock();
    return data;
}

template<typename T>
inline JReversePipeInfo JReversePipe<T>::GetInfo()
{
    JReversePipeInfo returnable;
    returnable.Name = std::string(name);
    returnable.Size = (long long int)(shm.get_size());
    returnable.FreeMemory = (long long int)(shm.get_free_memory());
    return returnable;
}

template<typename T>
inline void JReversePipe<T>::Grow(long long int size)
{
    boost::interprocess::managed_shared_memory::grow(name.c_str(), size);
}

/*
* SEND A DISCONNECT COMMAND AND THEN RECONNECT COMMAND BEFORE USING
*/
template<typename T>
inline void JReversePipe<T>::Reload(long long int size)
{
    T currentdata = ReadPipe();
    boost::interprocess::shared_memory_object::remove(name.c_str());
    using namespace boost::interprocess;
    managed_shared_memory ghm(open_or_create, name.c_str(), size);
    ghm.swap(shm);
    address = shm.get_address();
    WritePipe(currentdata);
}

template<typename T>
inline void JReversePipe<T>::Free()
{
    boost::interprocess::shared_memory_object::remove(name.c_str());
}



