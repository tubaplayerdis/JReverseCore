#pragma once
#include <string>
#include <memory.h>
#include <type_traits>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
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
class JReversePipeClient
{
public:
    JReversePipeClient(std::string Name, boost::interprocess::mode_t Mode);
    void WritePipe(const T& data);
    T ReadPipe();
    JReversePipeInfo GetInfo();
    void SetBlock(bool status);
    void Reconnect();
    void Disconnect();
    void Grow(int size);
    void Free();
private:
    bool blocked;
    boost::interprocess::managed_shared_memory shm;
    void* address;
    size_t writtenSize;
    std::string name;
    boost::interprocess::mode_t mode;
};

template<typename T>
inline JReversePipeClient<T>::JReversePipeClient(std::string Name, boost::interprocess::mode_t Mode)
{
    //Store for reconnection data
    name = Name;
    mode = Mode;
    using namespace boost::interprocess;
    //Create a native windows shared memory object.
    managed_shared_memory ghm(open_only, Name.c_str());

    ghm.swap(shm);

    address = shm.get_address();

    writtenSize = sizeof(T);
    blocked = false;
}

template<typename T>
inline void JReversePipeClient<T>::WritePipe(const T& data)
{
    if (blocked) {
        std::cout << "Cannot Write! Pipe is blocked!" << std::endl;
        return;
    }

    if (sizeof(data) > shm.get_size()) {
        throw std::exception("The data is to big for the region!");
    }

    boost::interprocess::interprocess_mutex* mutex = shm.find_or_construct<boost::interprocess::interprocess_mutex>("MUTEX")();

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
            std::cout << "Not Enough Memory Avialable! Resize: " << name << " in IPC Manager!" << std::endl;
            std::cout << "Stopped at: " << i << "/" << d << " Strings" << std::endl;
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
inline T JReversePipeClient<T>::ReadPipe()
{
    if (shm.get_address() == nullptr) {
        throw std::exception("address was null");
    }

    boost::interprocess::interprocess_mutex* mutex = shm.find_or_construct<boost::interprocess::interprocess_mutex>("MUTEX")();

    T data;
    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
        if (blocked) {
            std::cout << "Cannot Read! Pipe is blocked!" << std::endl;
            return std::vector<std::string>{"NONE"};
        }
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
        if (blocked) {
            std::cout << "Cannot Read! Pipe is blocked!" << std::endl;
            return "NONE";
        }
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
inline JReversePipeInfo JReversePipeClient<T>::GetInfo()
{
    JReversePipeInfo returnable;
    returnable.Name = std::string(name);
    returnable.Size = shm.get_size();
    returnable.Mode = mode;
    return returnable;
}

template<typename T>
inline void JReversePipeClient<T>::SetBlock(bool status)
{
    blocked = status;
}

template<typename T>
inline void JReversePipeClient<T>::Reconnect()
{
    using namespace boost::interprocess;
    //Create a native windows shared memory object.
    managed_shared_memory ghm(open_only, name.c_str());
    
    std::cout << "New Connected Size: " << ghm.get_size() << std::endl;
    std::cout << "New Mapping Handle: " << ghm.get_address() << std::endl;

    ghm.swap(shm);
    address = shm.get_address();

    std::cout << "New Address: " << shm.get_address() << std::endl;
    std::cout << "New Size: " << shm.get_size() << std::endl;
}

template<typename T>
inline void JReversePipeClient<T>::Disconnect()
{
    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
        shm.destroy<ShmStringVector>("MAIN");
    }
    else if constexpr (std::is_same_v<T, std::string>) {
        shm.destroy<ShmString>("MAIN");
    }
    shm.~basic_managed_shared_memory();
}

template<typename T>
inline void JReversePipeClient<T>::Grow(int size)
{
    blocked = true;
    boost::interprocess::managed_shared_memory::grow(name.c_str(), size);
    blocked = false;
}

template<typename T>
inline void JReversePipeClient<T>::Free()
{
    blocked = true;
    boost::interprocess::managed_shared_memory::destroy(name.c_str());
}




