#pragma once
#include <jvmti.h>
#include <vector>
#include "StartupRule.h"

class JReverseStore
{
public:
	static jvmtiEnv* jvmtienv;
	static std::vector<StartupRule> ruleslist;
	static std::vector<StartupRule> bypassRules;
};
