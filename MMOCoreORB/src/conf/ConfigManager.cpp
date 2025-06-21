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

// All ConfigManager methods restored below

void ConfigManager::clearConfigData() {
	Locker guard(&mutex);

	for (int i = 0; i < configData.size(); ++i) {
		auto entry = configData.getUnsafe(i).getValue();
		delete entry;
	}

	configData.removeAll();
	configData.setNoDuplicateInsertPlan();
	incrementConfigVersion();
}

bool ConfigManager::getBool(const String& name, bool defaultValue, unsigned int accountID) {
	ReadLocker guard(&mutex);
	ConfigDataItem* itm = findItem(name, accountID);
	if (itm == nullptr)
		return defaultValue;
	return itm->getBool();
}

int ConfigManager::getInt(const String& name, int defaultValue, unsigned int accountID) {
	ReadLocker guard(&mutex);
	ConfigDataItem* itm = findItem(name, accountID);
	if (itm == nullptr)
		return defaultValue;
	return itm->getInt();
}

float ConfigManager::getFloat(const String& name, float defaultValue, unsigned int accountID) {
	ReadLocker guard(&mutex);
	ConfigDataItem* itm = findItem(name, accountID);
	if (itm == nullptr)
		return defaultValue;
	return itm->getFloat();
}

const String& ConfigManager::getString(const String& name, const String& defaultValue, unsigned int accountID) {
	Locker guard(&mutex);
	ConfigDataItem* itm = findItem(name, accountID);
	if (itm == nullptr) {
		itm = new ConfigDataItem(defaultValue);
		if (itm == nullptr || !updateItem(name, itm))
			throw Exception("ConfigManager::getString(" + name + ") failed to set default value: [" + defaultValue + "]");
	}
	return itm->getString();
}

const Vector<String>& ConfigManager::getStringVector(const String& name, unsigned int accountID) {
	ReadLocker guard(&mutex);
	ConfigDataItem* itm = findItem(name, accountID);
	if (itm == nullptr)
		throw Exception("ConfigManager::getStringVector(" + name + ") not found");
	return itm->getStringVector();
}

const SortedVector<String>& ConfigManager::getSortedStringVector(const String& name, unsigned int accountID) {
	ReadLocker guard(&mutex);
	ConfigDataItem* itm = findItem(name, accountID);
	if (itm == nullptr)
		throw Exception("ConfigManager::getSortedStringVector(" + name + ") not found");
	return itm->getSortedStringVector();
}

int ConfigManager::getUsageCounter(const String& name) const {
	ConfigDataItem* itm = findItem(name);
	if (itm == nullptr)
		return -1;
	return itm->getUsageCounter();
}

bool ConfigManager::setBool(const String& name, bool newValue) {
	return updateItem(name, new ConfigDataItem(newValue));
}

bool ConfigManager::setInt(const String& name, int newValue) {
	return updateItem(name, new ConfigDataItem(newValue));
}

bool ConfigManager::setFloat(const String& name, float newValue) {
	return updateItem(name, new ConfigDataItem(newValue));
}

bool ConfigManager::setString(const String& name, const String& newValue) {
	return updateItem(name, new ConfigDataItem(newValue));
}

bool ConfigManager::setStringFromFile(const String& name, const String& fileName) {
	StringBuffer newValue;
	try {
		File file(fileName);
		FileReader reader(&file);
		String line;
		while (reader.readLine(line))
			newValue << line;
		reader.close();
		return setString(name, newValue.toString());
	} catch (const FileNotFoundException& e) {
		error("setStringFromFile(" + name + ", " + fileName +") File Not Found.");
	} catch (const Exception& e) {
		error("setStringFromFile(" + name + ", " + fileName +") Unexpected exception reading file.");
	}
	return false;
}

bool ConfigManager::updateItem(const String& name, ConfigDataItem* newItem) {
	Locker guard(&mutex);
	if (newItem == nullptr || name.isEmpty())
		return false;
	int pos = configData.find(name);
	if (pos != -1) {
		ConfigDataItem* oldItem = configData.get(pos);
		configData.drop(name);
		delete oldItem;
		oldItem = nullptr;
	}
	if (logChanges) {
		if (isSensitiveKey(name)) {
			info(true) << "Configuration updated: " << name;
		} else {
			info(true) << "Configuration update: " << name << " = [" << newItem->toString() << "]";
		}
	}
#ifdef DEBUG_CONFIGMANAGER
	info("updateItem: " + name + " = [" + newItem->toString() + "]", true);
	newItem->setDebugTag(name);
#endif // DEBUG_CONFIGMANAGER
	configData.put(std::move(name), std::move(newItem));
	incrementConfigVersion();
	return true;
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
