#pragma once

#include <string>
#include <vector>

class RessourceLoader
{
	struct AnimatorLink
	{
		std::string animator;
		std::string animatorState;
		std::string animation;
	};

public:
	static void loadAll(const std::string & path);
	static void unloadAll();

	static std::string readFile(const std::string & filename);

private:
	RessourceLoader() = delete;
	RessourceLoader(const RessourceLoader &) = delete;

	static void loadDirectoryRessources(const std::string & basePath, const std::string & subPath);
	static void loadFile(const std::string & path, const std::string & filename);
	static void FixLinksAfterLoad();

	static bool loadTexture(const std::string & path, const std::string & filename);
	static bool loadAnimation(const std::string & path, const std::string & filename);
	static bool loadAnimator(const std::string & path, const std::string & filename);
	static bool loadTilemap(const std::string & path, const std::string & filename);

	static void initializeMaterials();

	static std::vector<AnimatorLink> m_animatorLinks;
};
