#pragma once
#include "JReversePipeClient.h"
#include <vector>
#include <string>

class PipeClientAPI
{
public:
    static bool isFunctionPipeNone();
    static bool isFunctionArgPipeNone();
    static const std::string noneStr;
    static const std::vector<std::string> noneVec;
    static const std::vector<std::string> noneInstacneVec;
    static std::vector<std::string> GetAllPipes();
    static bool CheckForPipe(std::string pipename);
    static std::string GetPipeType(std::string name);
    static std::string ReadFunctionPipeAR();
    static std::vector<std::string> ReadFunctionArgPipeAR();
    static void PrintPipes();

    static JReversePipeClient<std::string> FunctionPipe;
    static JReversePipeClient<std::vector<std::string>> FunctionArgPipe;
    static JReversePipeClient<std::vector<std::string>> ReturnPipe;
    static JReversePipeClient<std::vector<std::string>> PipeNamePipe;
    static JReversePipeClient<std::string> CommunicationPipe;
};
