#pragma once
#include "JReversePipeClient.h"
#include <vector>
#include <string>

class PipeClientAPI
{
public:
    static std::vector<std::string> GetAllPipes();
    static bool CheckForPipe(std::string pipename);
    static std::string GetPipeType(std::string name);
    static void PrintPipes();

    static JReversePipeClient<std::string> FunctionPipe;
    static JReversePipeClient<std::vector<std::string>> PipeNamePipe;
    static JReversePipeClient<std::string> CommunicationPipe;
};
