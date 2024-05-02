#include "datastore.h"

jvmtiEnv* JReverseStore::jvmtienv = nullptr;
std::vector<StartupRule> JReverseStore::ruleslist = std::vector<StartupRule>{};
std::vector<StartupRule> JReverseStore::bypassRules = std::vector<StartupRule>{};