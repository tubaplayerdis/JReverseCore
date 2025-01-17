#pragma once

struct StartupRule
{
	const char* ClassName;
	const char* ByteCodes;
	bool IsBypass;
};

inline StartupRule SerializeStartupRule(const char* name, const char* bytecodes, const char* isbypass) {
	StartupRule returnable;
	returnable.ClassName = name;
	returnable.ByteCodes = bytecodes;
	if (strcmp(isbypass, "true") == 0) returnable.IsBypass = true; else returnable.IsBypass = false;
	return returnable;
}