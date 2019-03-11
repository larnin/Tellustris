
#include "GameData/TileDefinition.h"

size_t TileDefinition::addTexture(Nz::TextureRef texture)
{
	auto it = std::find(m_textures.begin(), m_textures.end(), texture);
	if (it != m_textures.end())
		return it - m_textures.begin();
	m_textures.push_back(texture);
	return m_textures.size() - 1;
}

void TileDefinition::removeTexture(Nz::TextureRef texture)
{
	removeTexture(textureIndex(texture));
}

void TileDefinition::removeTexture(size_t index)
{
	assert(index < m_textures.size());

	m_textures.erase(m_textures.begin() + index);
}

void TileDefinition::removeAllTexture()
{
	m_textures.clear();
}

bool TileDefinition::haveTexture(Nz::TextureRef texture) const
{
	return std::find(m_textures.begin(), m_textures.end(), texture) != m_textures.end();
}

size_t TileDefinition::textureIndex(Nz::TextureRef texture) const
{
	assert(haveTexture(texture));
	return std::find(m_textures.begin(), m_textures.end(), texture) - m_textures.begin();
}

Nz::TextureRef TileDefinition::getTexture(size_t index) const
{
	assert(index < m_textures.size());
	return m_textures[index];
}

size_t TileDefinition::textureCount() const
{
	return m_textures.size();
}

void TileDefinition::addTile(size_t materialID, TileConnexionType connexion, const SingleTileDefinition & def)
{
	while (materialID >= m_materials.size())
		m_materials.push_back({});

	auto & tile = m_materials[materialID].tiles[static_cast<size_t>(connexion)];
	auto it = std::find_if(tile.begin(), tile.end(), [def](const auto & t) {return t.textureID == def.textureID && t.tileID == def.tileID; });

	if (it != tile.end())
		it->weight = def.weight;
	else tile.push_back(def);
}

void TileDefinition::addTile(size_t materialID, TileConnexionType connexion, size_t tileID, size_t textureID, float weight)
{
	addTile(materialID, connexion, SingleTileDefinition{ textureID, tileID, weight });
}

const std::vector<SingleTileDefinition> & TileDefinition::getTile(size_t materialID, TileConnexionType connexions) const
{
	if (materialID >= m_materials.size())
		return {};
	return m_materials[materialID].tiles[static_cast<size_t>(connexions)];
}

void TileDefinition::addAllowedLayers(size_t materialID, size_t min, size_t max)
{
	while (materialID >= m_materials.size())
		m_materials.push_back({});

	m_materials[materialID].allowedLayers.push_back(TileMaterialLayers{ min, max });
}

size_t TileDefinition::materialCount() const
{
	return m_materials.size();
}

void TileDefinition::clearMaterials()
{
	m_materials.clear();
}

std::vector<size_t> TileDefinition::texturesIndexsForMaterial(size_t materialID) const
{
	if (materialID >= m_materials.size())
		return {};

	std::vector<size_t> indexs;

	for (const auto & tiles : m_materials[materialID].tiles)
		for (const auto & t : tiles)
		{
			if (t.tileID == 0)
				continue;
			if (std::find(indexs.begin(), indexs.end(), t.textureID) == indexs.end())
				indexs.push_back(t.textureID);
		}
	return indexs;
}

bool TileDefinition::isMaterialAllowedOnLayer(size_t materialID, size_t layer) const
{
	if (materialID >= m_materials.size())
		return false;

	const auto & mat = m_materials[materialID];
	for (const auto & allow : mat.allowedLayers)
		if (allow.min <= layer && allow.max >= layer)
			return true;
	return false;
}