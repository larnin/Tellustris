
#include "GameData/LoadSettings.h"
#include "Utility/Settings.h"
#include "Utility/Ressource.h"
#include "Utility/Json.h"
#include "GameData/TileDefinition.h"
#include "GameData/CollisionDefinition.h"
#include "GameData/LoadRessources.h"

#include <filesystem>
#include <iostream>
#include <cassert>

namespace fs = std::filesystem;
using Json = nlohmann::json;

void SettingsLoader::loadAll(const std::string & path)
{
	loadDirectoryRessources(path, "");
}

void SettingsLoader::unloadAll()
{
	Settings<TileDefinition>::reset();
	Settings<CollisionDefinition>::reset();
}

void SettingsLoader::loadDirectoryRessources(const std::string & basePath, const std::string & subPath)
{
	fs::path fullPath = basePath;
	fullPath /= subPath;
	if (!fs::exists(fullPath))
		return;
	for (auto & p : fs::directory_iterator(fullPath))
	{
		std::string str = p.path().u8string();
		str.erase(0, basePath.size());
		std::replace(str.begin(), str.end(), '\\', '/');

		if (p.is_directory())
			loadDirectoryRessources(basePath, str);
		else if (p.is_regular_file())
			loadFile(p.path().u8string(), str);
	}
}

void SettingsLoader::loadFile(const std::string & path, const std::string & filename)
{
	auto index = filename.find_last_of('.');
	std::string extension = "";
	if (index != std::string::npos && index < filename.size() - 1)
		extension = filename.substr(index + 1);

	bool loaded = false;

	if (extension == "tdef")
		loaded = loadTileDefinition(path, filename);
	if (extension == "cdef")
		loaded = loadCollisionDefinition(path, filename);

	if (loaded)
		std::cout << "Ressource " << filename << " loaded !" << std::endl;
	else std::cout << "ERROR: Can't load " << filename << std::endl;

}


bool SettingsLoader::loadTileDefinition(const std::string & path, const std::string & filename)
{
	if (Settings<TileDefinition>::value().IsValid())
		return false;

	Json j(RessourceLoader::readFile(path));

	assert(j.is_object());

	auto def = TileDefinition::New();

	const auto & textures = j["tex"];
	assert(textures.is_array());

	for (const auto & t : textures)
	{
		auto tex = Ressource<Nz::Texture>::get(t.get<std::string>());
		assert(tex.IsValid());
		def->addTexture(tex);
	}

	const auto & mats = j["mats"];
	assert(mats.is_array());

	for (size_t i = 0; i < mats.size(); i++)
	{
		auto layer = i + 1;

		const auto & m = mats[i];

		assert(m.is_object());

		const auto & allowed = m["allowed"];
		assert(allowed.is_array());
		for (const auto & a : allowed)
		{
			assert(a.is_object());
			auto min = a.find("min");
			auto max = a.find("max");
			if (min == a.end())
				continue;
			if (max == a.end())
				def->addAllowedLayers(layer, min->get<size_t>());
			else def->addAllowedLayers(layer, min->get<size_t>(), max->get<size_t>());
		}

		const auto & tiles = m["tiles"];
		assert(tiles.is_array());
		assert(tiles.size() == static_cast<unsigned int>(TileConnexionType::Max) + 1);
		for (size_t j = 0; j < tiles.size(); j++)
		{
			const auto & t = tiles[j];
			assert(t.is_array());
			for (const auto & tCon : t)
			{
				assert(tCon.is_object());
				SingleTileDefinition singleTile;
				singleTile.textureID = tCon["tex"].get<size_t>();
				singleTile.tileID = tCon["id"].get<size_t>();
				singleTile.weight = tCon["weight"].get<float>();
				def->addTile(layer, static_cast<TileConnexionType>(j), singleTile);
			}
		}
	}

	Settings<TileDefinition>::set(filename, def);

	return true;
}

bool SettingsLoader::loadCollisionDefinition(const std::string & path, const std::string & filename)
{
	if (Settings<CollisionDefinition>::value().IsValid())
		return false;

	Json j(RessourceLoader::readFile(path));

	assert(j.is_object());

	auto def = CollisionDefinition::New();

	const auto & layers = j["layers"];

	assert(layers.is_array());
	for (const auto & l : layers)
		def->addLayer(l.get<std::string>());

	auto layersSize = layers.size();

	const auto & contacts = j["contacts"];
	assert(contacts.is_array());
	assert(contacts.size() == layersSize * layersSize);
	for (size_t i = 0 ; i < layersSize; i++)
	{
		for (size_t j = 0; j < layersSize; j++)
		{
			auto contact = static_cast<ContactType>(contacts[i + j * layersSize].get<unsigned int>());
			def->setCollision(i, j, contact);
		}
	}

	Settings<CollisionDefinition>::set(filename, def);

	return true;
}