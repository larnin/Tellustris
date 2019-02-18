
#include "GameData/LoadRessources.h"
#include "Utility/Ressource.h"
#include "Utility/Json.h"
#include "Animator/Animation.h"
#include "Animator/Animator.h"
#include "Tilemap/Tilemap.h"

#include <Nazara/Renderer/Texture.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <cassert>

namespace fs = std::filesystem;
using Json = nlohmann::json;

std::vector<RessourceLoader::AnimatorLink> RessourceLoader::m_animatorLinks;

void RessourceLoader::loadAll(const std::string & path)
{
	loadDirectoryRessources(path, "");
	FixLinksAfterLoad();
}

void RessourceLoader::unloadAll()
{
	Ressource<Nz::Texture>::clear();
	Ressource<Animator>::clear();
	Ressource<Animation>::clear();
	Ressource<Tilemap>::clear();
}

std::string RessourceLoader::readFile(const std::string & filename)
{
	std::ifstream stream(filename);
	std::string str;

	stream.seekg(0, std::ios::end);
	str.reserve(stream.tellg());
	stream.seekg(0, std::ios::beg);

	str.assign((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());

	return str;
}


void RessourceLoader::loadDirectoryRessources(const std::string & basePath, const std::string & subPath)
{
	fs::path fullPath = basePath;
	fullPath /= subPath;
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

void RessourceLoader::loadFile(const std::string & path, const std::string & filename)
{
	auto index = filename.find_last_of('.');
	std::string extension = "";
	if (index != std::string::npos && index < filename.size() - 1)
		extension = filename.substr(index + 1);

	bool loaded = false;

	if (extension == "png")
		loaded = loadTexture(path, filename);
	else if (extension == "anim")
		loaded = loadAnimation(path, filename);
	else if (extension == "ator")
		loaded = loadAnimator(path, filename);
	else if (extension == "tmap")
		loaded = loadTilemap(path, filename);
			
	if (loaded)
		std::cout << "Ressource " << filename << " loaded !" << std::endl;
	else std::cout << "ERROR: Can't load " << filename << std::endl;
}

void RessourceLoader::FixLinksAfterLoad()
{
	for (const auto & l : m_animatorLinks)
	{
		auto ator = Ressource<Animator>::get(l.animator);
		assert(ator.IsValid());
		assert(ator->animationExist(l.animatorState));
		auto anim = Ressource<Animation>::get(l.animation);
		if (!anim.IsValid())
		{
			std::cout << "ERROR: Can't find the animation " + l.animation + " on animator " + l.animator + " state " + l.animatorState;
			continue;
		}
		ator->setAnimation(l.animatorState, anim);
	}

	m_animatorLinks.clear();
}

bool RessourceLoader::loadTexture(const std::string & path, const std::string & filename)
{
	auto texture = Nz::Texture::LoadFromFile(path);
	if (!texture.IsValid())
		return false;

	Ressource<Nz::Texture>::add(filename, texture);
	return true;
}

bool RessourceLoader::loadAnimation(const std::string & path, const std::string & filename)
{
	Json j(readFile(path));

	assert(j.is_object());

	auto anim = Animation::New();
	anim->loop = j["loop"].get<bool>();

	for (const auto jFrame : j["frames"])
	{
		Frame f;
		f.time = jFrame["time"].get<float>();
		f.offset.x = jFrame["x"].get<int>();
		f.offset.y = jFrame["y"].get<int>();
		f.rect.x = jFrame["left"].get<int>();
		f.rect.y = jFrame["top"].get<int>();
		f.rect.width = jFrame["width"].get<int>();
		f.rect.height = jFrame["height"].get<int>();
		anim->push_back(f);
	}

	Ressource<Animation>::add(filename, anim);

	return true;
}

bool RessourceLoader::loadAnimator(const std::string & path, const std::string & filename)
{
	Json j(readFile(path));

	assert(j.is_object());

	auto ator = Animator::New();

	auto jStates = j["states"];
	assert(jStates.is_array());
	auto jTransitions = j["transitions"];
	assert(jTransitions.is_array());

	for (const auto & jState : jStates)
	{
		auto name = jState["name"].get<std::string>();
		auto animName = jState["animation"].get<std::string>();
		auto xFlipped = jState["xFlipped"].get<bool>();
		auto yFlipped = jState["yFlipped"].get<bool>();

		m_animatorLinks.push_back(AnimatorLink{filename, name, animName});
		ator->addAnimation(name, {}, xFlipped, yFlipped);
	}

	for (const auto & jTransition : jTransitions)
	{
		auto previous = jTransition["previous"].get<unsigned int>();
		auto next = jTransition["next"].get<unsigned int>();
		auto condition = jTransition["condition"].get<std::string>();

		ator->addTransition(previous, next, condition);
	}

	ator->setDefaultAnimation(j["start"].get<unsigned int>());

	Ressource<Animator>::add(filename, ator);

	return true;
}

bool RessourceLoader::loadTilemap(const std::string & path, const std::string & filename)
{
	Json j(readFile(path));

	assert(j.is_object());

	auto sizeX = j["sizeX"].get<unsigned int>();
	auto sizeY = j["sizeY"].get<unsigned int>();
	auto tileSize = j["tile"].get<unsigned int>();
	auto tileDelta = j["delta"].get<unsigned int>();

	auto tilemap = Tilemap::New(sizeX, sizeY, tileSize, tileDelta);

	auto tiles = j["tiles"];
	assert(tiles.is_array() && tiles.size() == sizeX * sizeY);

	for (unsigned int i(0); i < sizeX; i++)
		for (unsigned int j(0); j < sizeY; j++)
		{
			auto t = tiles[i + j * sizeX];

			tilemap->setTile(i, j, Tile{ t["id"].get<unsigned int>(), t["c"].get<unsigned int>() });
		}

	return false;
}