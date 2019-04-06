#pragma once

#include <string>

class SettingsLoader
{
public:
	static void loadAll(const std::string & path);
	static void unloadAll();

private:
	static void loadDirectoryRessources(const std::string & basePath, const std::string & subPath);
	static void loadFile(const std::string & path, const std::string & filename);

	static bool loadTileDefinition(const std::string & path, const std::string & filename);
	static bool loadCollisionDefinition(const std::string & path, const std::string & filename);
};