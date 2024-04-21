#pragma once
#include "JythonInterpreter.h"
#include <vector>
#include <string>
#include "JythonReturns.h"

class JythonManager
{
public:
	static JythonReturns RunOnInterpreter(int ID, std::string path);
	static int AddInterpreter(JythonInterpreter interpreter);
	static void RemoveByID(int ID);
	static std::vector<int> GetIDS();
private:
	static std::vector<JythonInterpreter> Interpreters;
};