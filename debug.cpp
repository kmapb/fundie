#include <string>
#include <unordered_set>
#include "debug.h"

DBGModule::DBGModule() {
#define X(module) module = false;
    DBG_MODULES
#undef X
    auto cstr = ::getenv("DBG");
    std::string env (cstr ? cstr : "");
    if (env.empty()) {
        return;
    }
    std::unordered_set<std::string> modules;
    size_t pos = 0;
    while ((pos = env.find(':')) != std::string::npos) {
        auto module = env.substr(0, pos);
        modules.insert(module);
        env.erase(0, pos + 1);
    }
    modules.insert(env);

#define X(module) module = modules.find(#module) != modules.end();
    DBG_MODULES
}

DBGModule DBG;