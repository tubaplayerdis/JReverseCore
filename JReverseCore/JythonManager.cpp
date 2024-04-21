#include "JythonManager.h"
#include "JythonInterpreter.h"
#include "JythonReturns.h"
#include <vector>


std::vector<JythonInterpreter> JythonManager::Interpreters = std::vector<JythonInterpreter>{};

JythonReturns JythonManager::RunOnInterpreter(int ID, std::string path)
{
	for (int i = 0; i < JythonManager::Interpreters.size(); i++)
	{
		if (ID == JythonManager::Interpreters[i].ID)
		{
			return JythonManager::Interpreters[i].RunScript(path.c_str());
		}
	}
	return JythonReturns::INTERPRETER_NOT_FOUND;
}

int JythonManager::AddInterpreter(JythonInterpreter interpreter)
{
	JythonManager::Interpreters.push_back(interpreter);
	return interpreter.ID;
}

void JythonManager::RemoveByID(int ID)
{
	for (int i = 0; i < JythonManager::Interpreters.size(); i++)
	{
		if (ID == JythonManager::Interpreters[i].ID) {
			JythonManager::Interpreters[i].Close();
			JythonManager::Interpreters.erase(JythonManager::Interpreters.begin() + i);
			return;
		}
	}
}

std::vector<int> JythonManager::GetIDS()
{
	std::vector<int> ReturnList;
	for(int i = 0; i < JythonManager::Interpreters.size(); i++)
	{
		ReturnList.push_back(JythonManager::Interpreters[i].ID);
	}
	return ReturnList;
}
