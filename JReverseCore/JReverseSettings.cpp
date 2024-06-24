#include "JReverseSettings.h"
#include <iostream>
#include <vector>
#include <string>
#include "PipeClientAPI.h"

//Defualts by defualt

int JReverseStartupSettings::funcLoopTimeout = 10;
int JReverseStartupSettings::JNIEnvTiemout = 100;
bool JReverseStartupSettings::isClassFileLoadMessages = true;
bool JReverseStartupSettings::isConsoleWindow = true;
bool JReverseStartupSettings::isClassFileCollection = true;
bool JReverseStartupSettings::isDynamicClassFileCollection = false;
std::string JReverseStartupSettings::DynamicClassFileCollectionPath = "None";


void JReverseStartupSettings::InitSettings()
{
	JReversePipeInfo info = PipeClientAPI::SettingsPipe.GetInfo();
	if (info.Size < 1) std::cout << "Problem with settings IPC" << std::endl;
	std::vector<std::string> settings = PipeClientAPI::SettingsPipe.ReadPipe();
	std::cout << "Read Settings Pipe!" << std::endl;
	
	if (settings.size() < 7) {
		std::cout << "Settings Invalid. Assuming defualts" << std::endl;
		std::cout << "Dumping Settings Input: " << std::endl;
		for (std::string setting : settings) {
			std::cout << setting << std::endl;
		}
		return;
	}
	//Serielize settings
	if (settings[0] == "1") isClassFileCollection = true; else isClassFileCollection = false;
	if (settings[1] == "1") isClassFileLoadMessages = true; else isClassFileLoadMessages = false;
	if (settings[2] == "1") isConsoleWindow = true; else isConsoleWindow = false;
	funcLoopTimeout = std::stoi(settings[3]);
	JNIEnvTiemout = std::stoi(settings[4]);
	isDynamicClassFileCollection = std::stoi(settings[5]);
	DynamicClassFileCollectionPath = settings[6];
}
