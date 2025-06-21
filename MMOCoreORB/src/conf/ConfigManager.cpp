/*
				Copyright <SWGEmu>
	See file COPYING for copying conditions.*/

#include "ConfigManager.h"
#include <regex>

using namespace sys::thread;

ConfigManager::ConfigManager() {
	setLoggingName("ConfigManager");
#ifdef DEBUG_CONFIGMANAGER
	setLogLevel(Logger::DEBUG);
#else // DEBUG_CONFIGMANAGER
	setLogLevel(Logger::INFO);
#endif // DEBUG_CONFIGMANAGER
}

ConfigManager::~ConfigManager() {
	clearConfigData();
}

bool ConfigManager::loadConfigData() {
	Locker guard(&mutex);

	logChanges = false;

	if (configStartTime.getStartTime() != 0)
		configStartTime.stop();

	configStartTime.start();

	// Optional config.lua load
	File fileConfig("../bin/custom_scripts/config.lua");
	if (fileConfig.setReadOnly()) {
		if (!lua.runFile("../bin/custom_scripts/config.lua")) {
			error("ConfigManager failed to parse custom_scripts/config.lua");
			return false;
		}
	} else {
		info("Did not find custom_scripts/config.lua", true);
	}

	// Optional config-local.lua load
	File fileLocal("../bin/custom_scripts/config-local.lua");
	if (fileLocal.setReadOnly()) {
		if (!lua.runFile("../bin/custom_scripts/config-local.lua")) {
			error("ConfigManager failed to parse custom_scripts/config-local.lua");
			return false;
		}
	} else {
		info("Did not find custom_scripts/config-local.lua", true);
	}

	// Load fallback config-local.lua in conf/
	File file("conf/config-local.lua");
	if (file.setReadOnly()) {
		if (!lua.runFile("conf/config-local.lua")) {
			error("ConfigManager failed to parse conf/config-local.lua");
			return false;
		}
	} else {
		info("Did not find conf/config-local.lua", true);
	}

#ifdef DEBUG_CONFIGMANAGER
	info("Loaded config file(s) in " + String::valueOf(getConfigDataAgeMs()) + "ms", true);
#endif // DEBUG_CONFIGMANAGER

	bool resultGlobal, resultCore3;

	clearConfigData();

	// Load new-style "Core3.value" settings
	LuaObject core3 = lua.getGlobalObject("Core3");

	resultCore3 = parseConfigData("Core3");

	if (!resultCore3)
		error("Failed to parse Core3 configuration table, falling back on old Globals style config.");

	core3.pop();

	// Load legacy "globals" style configuration
	lua_State* L = lua.getLuaState();

	lua_pushglobaltable(L);

	resultGlobal = parseConfigData("Core3", true);

	if (!resultGlobal)
		error("Failed to parse legacy configuration globals.");

	lua_pop(L, 1);

	// Load file based strings into config
	setStringFromFile("Core3.MOTD", "conf/motd.txt");
	setStringFromFile("Core3.Revision", "conf/rev.txt");

#ifdef DEBUG_CONFIGMANAGER
	info("Parsed config into memory in " + String::valueOf(getConfigDataAgeMs()) + "ms", true);
	setString("Core3.ConfigManagerDebug", "Test1");
	setString("Core3.ConfigManagerDebug", "Compiled with DEBUG_CONFIGMANAGER");
	dumpConfig();
#endif // DEBUG_CONFIGMANAGER

	logChanges = true;

	return resultGlobal || resultCore3;
}
