#pragma once
#include <string>
#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>

class PipeManager
{
public:
	static void SetupClient();
	static std::string ReadString();
	static void WriteString(std::string data);
	static bool isDebug;
	static boost::interprocess::windows_shared_memory shm;
	static boost::interprocess::mapped_region region;
private:
};
