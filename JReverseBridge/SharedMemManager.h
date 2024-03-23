#pragma
#include <string>
#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>

class SharedMemManager
{
public:
	static void Setup();
	static std::string ReadString();
	static void WriteString(std::string message);

	static boost::interprocess::windows_shared_memory shm;
	static boost::interprocess::mapped_region region;
};

