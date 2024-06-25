#pragma once
#include "JReverseSettings.h"
#include "DynamicCollectionExclusion.h"
#include "DynamicCollector.h"
#include <iostream>
#include <string>
#include "PipeClientAPI.h"

inline void RunDebugConsole() {
    bool restore = JReverseStartupSettings::isClassFileLoadMessages;
    JReverseStartupSettings::isClassFileLoadMessages = false;
    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "JReverse Console. Enter a Command: ";
    std::string command;
    std::string pip;
    std::string input;
    std::cin >> input;

    if (input.find(" ") == std::string::npos) {
        command = input;
        pip = "NONE";
    }
    else {
        command = input.substr(0, input.find_first_of(" "));
        pip = input.substr(input.find_first_of(" ") + 1);
    }
    if (command == "HELP") {
        std::cout << "How to use commands: Type the command and then the pipe to use. Some commands dont require the input of a pipe." << "\nExample: INFO CriticalReturnPipe" << std::endl;
        std::cout << "\nCommmands and Description: \nHELP - Displays Help. NO PIPE\nINFO - Displays info about a pipe\nRECONNECT - Reconnects local refrence to shared memory\nDISCONNECT - Disconnects and destorys local refrence to shared memory\nBLOCK - Blocks outgoing read/writes to a pipe\nGROW - Increase the size of a pipe by a given amount\nGUI - Display JReverse Target Swing UI. NO PIPE" << std::endl;
    }
    else if (command == "INFO") {
        std::cout << "\nINFO: " << pip << std::endl;
        JReversePipeInfo info;
        if (pip == "CriticalFunctionPipe") {
            info = PipeClientAPI::FunctionPipe.GetInfo();
        }
        else if (pip == "CriticalFunctionArgPipe") {
            info = PipeClientAPI::FunctionArgPipe.GetInfo();
        }
        else if (pip == "CriticalReturnPipe") {
            info = PipeClientAPI::ReturnPipe.GetInfo();
        }
        else if (pip == "CriticalPipeNamePipe") {
            info = PipeClientAPI::PipeNamePipe.GetInfo();
        }
        else if (pip == "CriticalStartupPipe") {
            info = PipeClientAPI::StartupPipe.GetInfo();
        }
        else if (pip == "CriticalSettingsPipe") {
            info = PipeClientAPI::SettingsPipe.GetInfo();
        }
        std::cout << "NAME: " << info.Name << std::endl << "SIZE: " << info.Size << std::endl << "MODE: " << info.Mode << std::endl;
    }
    else if (command == "RECONNECT") {
        std::cout << "\nRECONNECTING: " << pip << std::endl;
        if (pip == "CriticalFunctionPipe") {
            PipeClientAPI::FunctionPipe.Reconnect();
        }
        else if (pip == "CriticalFunctionArgPipe") {
            PipeClientAPI::FunctionArgPipe.Reconnect();
        }
        else if (pip == "CriticalReturnPipe") {
            PipeClientAPI::ReturnPipe.Reconnect();
        }
        else if (pip == "CriticalPipeNamePipe") {
            PipeClientAPI::PipeNamePipe.Reconnect();
        }
        else if (pip == "CriticalStartupPipe") {
            PipeClientAPI::StartupPipe.Reconnect();
        }
        else if (pip == "CriticalSettingsPipe") {
            PipeClientAPI::SettingsPipe.Reconnect();
        }
    }
    else if (command == "DISCONNECT") {
        if (pip == "CriticalFunctionPipe") {
            std::cout << "\nDISCONNECTING: " << pip << std::endl;
            PipeClientAPI::FunctionPipe.Disconnect();
        }
        else if (pip == "CriticalFunctionArgPipe") {
            std::cout << "\nDISCONNECTING: " << pip << std::endl;
            PipeClientAPI::FunctionArgPipe.Disconnect();
        }
        else if (pip == "CriticalReturnPipe") {
            std::cout << "\nDISCONNECTING: " << pip << std::endl;
            PipeClientAPI::ReturnPipe.Disconnect();
        }
        else if (pip == "CriticalPipeNamePipe") {
            std::cout << "\nDISCONNECTING: " << pip << std::endl;
            PipeClientAPI::PipeNamePipe.Disconnect();
        }
        else if (pip == "CriticalStartupPipe") {
            std::cout << "\nDISCONNECTING: " << pip << std::endl;
            PipeClientAPI::StartupPipe.Disconnect();
        }
        else if (pip == "CriticalSettingsPipe") {
            std::cout << "\nDISCONNECTING: " << pip << std::endl;
            PipeClientAPI::SettingsPipe.Disconnect();
        }
    }
    else if (command == "BLOCK") {
        std::cout << "\nBLOCK: " << pip << std::endl;
        if (pip == "CriticalFunctionPipe") {
            PipeClientAPI::FunctionPipe.SetBlock(true);
        }
        else if (pip == "CriticalFunctionArgPipe") {
            PipeClientAPI::FunctionArgPipe.SetBlock(true);
        }
        else if (pip == "CriticalReturnPipe") {
            PipeClientAPI::ReturnPipe.SetBlock(true);
        }
        else if (pip == "CriticalPipeNamePipe") {
            PipeClientAPI::PipeNamePipe.SetBlock(true);
        }
        else if (pip == "CriticalStartupPipe") {
            PipeClientAPI::StartupPipe.SetBlock(true);
        }
        else if (pip == "CriticalSettingsPipe") {
            PipeClientAPI::SettingsPipe.SetBlock(true);
        }
    }
    else if (command == "UNBLOCK") {
        std::cout << "\nUNBLOCK: " << pip << std::endl;
        if (pip == "CriticalFunctionPipe") {
            PipeClientAPI::FunctionPipe.SetBlock(false);
        }
        else if (pip == "CriticalFunctionArgPipe") {
            PipeClientAPI::FunctionArgPipe.SetBlock(false);
        }
        else if (pip == "CriticalReturnPipe") {
            PipeClientAPI::ReturnPipe.SetBlock(false);
        }
        else if (pip == "CriticalPipeNamePipe") {
            PipeClientAPI::PipeNamePipe.SetBlock(false);
        }
        else if (pip == "CriticalStartupPipe") {
            PipeClientAPI::StartupPipe.SetBlock(false);
        }
        else if (pip == "CriticalSettingsPipe") {
            PipeClientAPI::SettingsPipe.SetBlock(false);
        }
    }
    else if (command == "GROW") {
        std::cout << "\nBLOCK: " << pip << std::endl;
        if (pip == "CriticalFunctionPipe") {
            PipeClientAPI::FunctionPipe.SetBlock(false);
        }
        else if (pip == "CriticalFunctionArgPipe") {
            PipeClientAPI::FunctionArgPipe.SetBlock(false);
        }
        else if (pip == "CriticalReturnPipe") {
            PipeClientAPI::ReturnPipe.SetBlock(false);
        }
        else if (pip == "CriticalPipeNamePipe") {
            PipeClientAPI::PipeNamePipe.SetBlock(false);
        }
        else if (pip == "CriticalStartupPipe") {
            PipeClientAPI::StartupPipe.SetBlock(false);
        }
        else if (pip == "CriticalSettingsPipe") {
            PipeClientAPI::SettingsPipe.SetBlock(false);
        }
    }
    else if (command == "FREE") {
        std::cout << "FREE Can only be used from JReverse" << std::endl;
    }
    else if (command == "GUI") {
        std::cout << "GUI is still in development!" << std::endl;
    }
    else if (command == "EXCLUSIONADD") {
        std::cout << "NAME? ";
        std::string name;
        std::cin >> name;
        std::cout << "\nTYPE? ";
        int ty;
        std::cin >> ty;
        DynamicCollectionExclusionType typeexc = getExclusionType(ty);
        DynamicCollectionExclusion exclusion;
        exclusion.name = name;
        exclusion.type = typeexc;
        DynamicCollector::exclusionlist.push_back(exclusion);
        std::cout << "\nCREATED EXCLUSION. NAME: " << name << " TYPE: " << typeexc << std::endl;
    }
    else if (command == "EXCLUSIONREMOVE") {
        //Pip is treated as the exlcusion name
        size_t index = DynamicCollector::FindExclsuionIndex(pip);
        DynamicCollector::exclusionlist.erase(DynamicCollector::exclusionlist.begin() + index);
    }
    else if (command == "EXCLUSIONMODIFY") {
        //Pip is treated as the exclusion to modify name
    }
    else if (command == "EXCLUSIONLIST") {
        DynamicCollector::ListExclusions();
    }

    JReverseStartupSettings::isClassFileLoadMessages = restore;

    std::cout << "Exiting JReverse Console..." << std::endl;
}