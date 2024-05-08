#include "JReverseSettings.h"
#include "PipeClientAPI.h"
#include <iostream>

//Defualts by defualt
int JReverseStartupSettings::funcLoopTimeout = 10;
int JReverseStartupSettings::JNIEnvTiemout = 100;
bool JReverseStartupSettings::isClassFileLoadMessages = true;
bool JReverseStartupSettings::isConsoleWindow = true;
bool JReverseStartupSettings::isClassFileCollection = true;

void JReverseStartupSettings::InitSettings()
{
	std::vector<std::string> settings = PipeClientAPI::SettingsPipe.ReadPipe();
	if (settings.empty()) {
		std::cout << "Settings Empty. Assuming defualts" << std::endl;
		return;
	}
	//Serielize settings
}
