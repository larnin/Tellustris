#pragma once

#include "TileConnexionType.h"
#include "Utility/FixedMatrix.h"

#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Renderer/Texture.hpp>

#include <vector>
#include <array>
#include <random>

class TileDefinition;

using TileDefinitionRef = Nz::ObjectRef<TileDefinition>;
using TileDefinitionConstRef = Nz::ObjectRef<const TileDefinition>;

struct SingleTileDefinition
{
	size_t textureID = 0;
	size_t tileID = 0;
	float weight = 0;
};

struct TileMaterialLayers
{
	size_t min = 0;
	size_t max = 0;
};

struct TileMaterialDefinition
{
	std::array<std::vector<SingleTileDefinition>, static_cast<unsigned int>(TileConnexionType::Max) + 1> tiles;
	std::vector<TileMaterialLayers> allowedLayers;
};

class TileDefinition : public Nz::RefCounted
{
public:
	size_t addTexture(Nz::TextureRef texture);
	void removeTexture(Nz::TextureRef texture);
	void removeTexture(size_t index);
	void removeAllTexture();
	bool haveTexture(Nz::TextureRef texture) const;
	size_t textureIndex(Nz::TextureRef texture) const;
	Nz::TextureRef getTexture(size_t index) const;
	size_t textureCount() const;

	void addTile(size_t materialID, TileConnexionType connexion, const SingleTileDefinition & def);
	void addTile(size_t materialID, TileConnexionType connexion, size_t tileID, size_t textureID = 0, float weight = 1);
	const std::vector<SingleTileDefinition> & getTile(size_t materialID, TileConnexionType connexions) const;
	void addAllowedLayers(size_t materialID, size_t min, size_t max);
	
	template <typename Gen>
	SingleTileDefinition getRandomTile(size_t materialID, TileConnexionType connexions, Gen gen) const
	{
		auto & tiles = getTile(materialID, connexions);
		if (tiles.empty())
			return {};
		std::vector<float> weights;
		for (const auto & t : tiles)
			weights.push_back(t.weight);
		return tiles[std::discrete_distribution<size_t>(weights.begin(), weights.end())(gen)];
	}

	size_t materialCount() const;
	void clearMaterials();
	std::vector<size_t> texturesIndexsForMaterial(size_t materialID) const;
	bool isMaterialAllowedOnLayer(size_t materialID, size_t layer) const;

	template<typename... Args> static TileDefinitionRef New(Args&&... args)
	{
		auto object = std::make_unique<TileDefinition>(std::forward<Args>(args)...);
		object->SetPersistent(false);

		return object.release();
	}

private:
	std::vector<Nz::TextureRef> m_textures;
	std::vector<TileMaterialDefinition> m_materials;
};