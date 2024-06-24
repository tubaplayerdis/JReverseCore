#pragma once
#include <string>

class JReverseStartupSettings
{
public:
	static void InitSettings();
	static bool isClassFileLoadMessages;
	static bool isClassFileCollection;
	static bool isConsoleWindow;
	static int funcLoopTimeout;
	static int JNIEnvTiemout;
	static bool isDynamicClassFileCollection;
	static std::string DynamicClassFileCollectionPath;
};