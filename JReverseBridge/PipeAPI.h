#pragma
#include <string>
#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/any/basic_any.hpp>

class JReversePipeHelpers
{
public:
    static boost::interprocess::mode_t getMode(int arg);
};


class JReversePipeInfo
{
public:
    std::string Name;
    int Size;
    boost::interprocess::mode_t Mode;
};

template<typename T>
class JReversePipe
{
private:
	boost::interprocess::windows_shared_memory shm;
	boost::interprocess::mapped_region region;
public:
	JReversePipe(std::string Name, int Mode, int Size);
	void WritePipe(const T& data);
	T ReadPipe();
    JReversePipeInfo GetInfo();
};

template<typename T>
inline JReversePipe<T>::JReversePipe(std::string Name, int Mode, int Size)
{
    //This was stolen
    using namespace boost::interprocess;
    //Create a native windows shared memory object.
    windows_shared_memory ghm(create_only, Name, JReversePipeHelpers::getMode(Mode), 1000);

    //Map the whole shared memory in this process
    mapped_region regionn(ghm, JReversePipeHelpers::getMode(Mode));

    //Write all the memory to 1
    std::memset(regionn.get_address(), 1, regionn.get_size());

    ghm.swap(shm);
    regionn.swap(region);
}

template<typename T>
inline void JReversePipe<T>::WritePipe(const T& data)
{
    std::strcpy(static_cast<T>(region.get_address()), data);
}

template<typename T>
inline T JReversePipe<T>::ReadPipe()
{
    return T(static_cast<T>(region.get_address()));
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

class PipeAPI
{
public:
    static std::vector<JReversePipeInfo> GetAllPipesInfo();//Read from PipeNamePipe
    //Critical Pipes
    static JReversePipe<std::string> FunctionPipe;
    static JReversePipe<std::vector<std::string>> PipeNamePipe;
    static JReversePipe<std::string> CommunicationPipe;
};

