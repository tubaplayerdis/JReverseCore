#include "PipeClientAPI.h"
#include "JReversePipeClient.h"
#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <string>
#include <vector>
#include <iostream>

const std::vector<std::string> PipeClientAPI::noneVec = std::vector<std::string>{ "NONE" };
const std::vector<std::string> PipeClientAPI::noneInstacneVec = std::vector<std::string>{ "NO INSTANCES" };
const std::string PipeClientAPI::noneStr = "NONE";

JReversePipeClient<std::string> PipeClientAPI::FunctionPipe = JReversePipeClient<std::string>("CriticalFunctionPipe", boost::interprocess::read_write);
JReversePipeClient<std::vector<std::string>> PipeClientAPI::FunctionArgPipe = JReversePipeClient<std::vector<std::string>>("CriticalFunctionArgPipe", boost::interprocess::read_write);
JReversePipeClient<std::vector<std::string>> PipeClientAPI::ReturnPipe = JReversePipeClient<std::vector<std::string>>("CriticalReturnPipe", boost::interprocess::read_write);
JReversePipeClient<std::vector<std::string>> PipeClientAPI::PipeNamePipe = JReversePipeClient<std::vector<std::string>>("CriticalPipeNamePipe", boost::interprocess::read_write);
JReversePipeClient<std::vector<std::string>> PipeClientAPI::StartupPipe = JReversePipeClient<std::vector<std::string>>("CriticalStartupPipe", boost::interprocess::read_write);
JReversePipeClient<std::string> PipeClientAPI::CommunicationPipe = JReversePipeClient<std::string>("CriticalCommunicationPipe", boost::interprocess::read_write);

std::vector<std::string> PipeClientAPI::GetAllPipes()
{
	return PipeNamePipe.ReadPipe();
}

bool PipeClientAPI::CheckForPipe(std::string pipename)
{
	std::vector<std::string> checklis = PipeNamePipe.ReadPipe();
	for (std::string pipnam : checklis)
	{
		pipnam = pipnam.substr(0, pipnam.find(":", 0));
		if (pipnam == pipename) return true;
	}
	return false;
}

std::string PipeClientAPI::GetPipeType(std::string name)
{
	std::vector<std::string> checklis = PipeNamePipe.ReadPipe();
	for (std::string pipnam : checklis)
	{
		std::string check = pipnam.substr(0, pipnam.find(":", 0));
		if (check == name) {
			return pipnam.substr(pipnam.find(":") + 1);
		}
	}
	return "PipeNotFound";
}

bool PipeClientAPI::isFunctionPipeNone()
{
	if (PipeClientAPI::FunctionPipe.ReadPipe() == "NONE") return true;
	return false;
}

bool PipeClientAPI::isFunctionArgPipeNone()
{
	auto vec = PipeClientAPI::FunctionArgPipe.ReadPipe();
	if (vec[0] == "NONE") {
		return true;
	}
	return false;
}

std::string PipeClientAPI::ReadFunctionPipeAR()
{
	std::string cur = PipeClientAPI::FunctionPipe.ReadPipe();
	PipeClientAPI::FunctionPipe.WritePipe(PipeClientAPI::noneStr);
	return cur;
}

std::vector<std::string> PipeClientAPI::ReadFunctionArgPipeAR()
{
	std::vector<std::string> cur = PipeClientAPI::FunctionArgPipe.ReadPipe();
	PipeClientAPI::FunctionArgPipe.WritePipe(PipeClientAPI::noneVec);
	return cur;
}

void PipeClientAPI::PrintPipes()
{
	std::cout << "ALL PIPES ACTIVE" << "\n";
	std::vector<std::string> checklis = PipeNamePipe.ReadPipe();
	for (std::string pipnam : checklis)
	{
		std::cout << pipnam << "\n";
	}
	
}
