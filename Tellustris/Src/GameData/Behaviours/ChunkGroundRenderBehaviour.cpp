
#include "GameData/Behaviours/ChunkGroundRenderBehaviour.h"
#include "Utility/Ressource.h"
#include "GameData/Behaviours/WorldRenderBehaviour2.h"
#include "Utility/StaticRandomGenerator.h"

#include <NDK/Components/GraphicsComponent.hpp>

#include <cassert>

ChunkGroundRenderBehaviour::ChunkGroundRenderBehaviour(Chunk & chunk, WorldMap & map, WorldRenderBehaviour2 & worldRender, int chunkX, int chunkY, TileDefinitionRef definition)
	: m_chunk(chunk)
	, m_map(map)
	, m_worldRender(worldRender)
	, m_chunkX(chunkX)
	, m_chunkY(chunkY)
	, m_definition(definition)
{
	m_layerChangedHolder = chunk.registerLayerChangedCallback([this](const auto & layerChanged) {onLayerChange(layerChanged.layer, layerChanged.state); });

	if(chunk.layerCount() > 0)
		onLayerAdd();
}

BehaviourRef ChunkGroundRenderBehaviour::clone() const
{
	auto render = std::make_unique<ChunkGroundRenderBehaviour>(m_chunk, m_map, m_worldRender, m_chunkX, m_chunkY, m_definition);
	return std::move(render);
}

void ChunkGroundRenderBehaviour::onBoderBlockUpdate(size_t x, size_t y, size_t layer)
{
	if (layer != 0 || layer > m_chunk.layerCount())
		return;

	assert(x < Chunk::chunkSize && y < Chunk::chunkSize);

	auto chunkPos = m_map.worldToLocalChunkPos(m_chunkX, m_chunkY);
	auto pos = m_map.tilePosToPos(Nz::Vector2ui(static_cast<unsigned int>(x), static_cast<unsigned int>(y)), chunkPos);
	auto mat = m_map.getTiles(pos.x - 1, pos.y - 1, 3, 3, layer);
	auto centerID = mat(1, 1).id;

	clearTile(static_cast<unsigned int>(x), static_cast<unsigned int>(y));

	std::vector<size_t> setMats;
	for(size_t k = 0 ; k < 3 ; k++)
		for (size_t l = 0; l < 3; l++)
		{
			auto currentID = mat(k, l).id;
			if (currentID > centerID || std::find(setMats.begin(), setMats.end(), currentID) != setMats.end())
				continue;
			setMats.push_back(currentID);

			FixedMatrix<bool, 3, 3> tiles;
			for (int i = 0; i < 3; i++)
				for (int j = 0; j < 3; j++)
					tiles(i, j) = mat(i, j).id <= mat(1, 1).id;

			auto id = m_definition->getRandomTile(mat(1, 1).id, localMatrixToTileConnexionType(tiles), StaticRandomGenerator<std::mt19937>());

			setTile(currentID, localMatrixToTileConnexionType(tiles), static_cast<unsigned int>(x), static_cast<unsigned int>(y));
		}
	
	cleanLayers();
}

void ChunkGroundRenderBehaviour::onEnable()
{
	if (m_chunk.layerCount() > 0)
		onLayerAdd();
}

void ChunkGroundRenderBehaviour::onDisable()
{
	onLayerRemove();
}

void ChunkGroundRenderBehaviour::onLayerChange(size_t layer, Chunk::LayerChanged::ChangeState state)
{
	if (layer != 0)
		return;

	switch (state)
	{
	case Chunk::LayerChanged::ChangeState::added:
		onLayerAdd();
		break;
	case Chunk::LayerChanged::ChangeState::removed:
		onLayerRemove();
	}
}

void ChunkGroundRenderBehaviour::onMapChange(size_t layer, size_t x, size_t y)
{
	if (layer != 0)
		return;

	auto map = m_chunk.getMap(layer);

	if (x >= map->width() || y >= map->height())
		onFullMapChange();
	else onTileChange(x, y);
}

void ChunkGroundRenderBehaviour::onLayerAdd()
{
	onFullMapChange();
}

void ChunkGroundRenderBehaviour::onLayerRemove()
{
	auto & renderer = getEntity()->GetComponent<Ndk::GraphicsComponent>();
	for (const auto & m : m_tilemaps)
		renderer.Detach(m.tilemap);
	m_tilemaps.clear();
	m_mapModified.disconnect();
}

void ChunkGroundRenderBehaviour::onTileChange(size_t x, size_t y)
{
	auto chunkPos = m_map.worldToLocalChunkPos(m_chunkX, m_chunkY);
	auto pos = m_map.tilePosToPos(Nz::Vector2ui(static_cast<unsigned int>(x), static_cast<unsigned int>(y)), chunkPos);
	auto mat = m_map.getTiles(pos.x - 2, pos.y - 2, 5, 5, 0);

	for (int i = static_cast<int>(x) - 1; i <= x + 1; i++)
		for (int j = static_cast<int>(y) - 1; j <= y + 1; j++)
		{
			if (i >= 0 && j >= 0 && i < Chunk::chunkSize && j < Chunk::chunkSize)
			{
				clearTile(static_cast<unsigned int>(i), static_cast<unsigned int>(j));

				Tile centerTile = mat(i - x + 2, j - y + 2);
				std::vector<size_t> setMats;
				for (size_t m = 0; m < 3; m++)
					for (size_t n = 0; n < 3; n++)
					{
						auto currentID = mat(m, n).id;
						if (currentID > centerTile.id || std::find(setMats.begin(), setMats.end(), currentID) != setMats.end())
							continue;
						setMats.push_back(currentID);
						FixedMatrix<bool, 3, 3> tiles;
						for (int k = 0; k < 3; k++)
							for (int l = 0; l < 3; l++)
								tiles(k, l) = mat(i - x + 1 + k, j - y + 1 + l).id <= currentID;

						setTile(currentID, localMatrixToTileConnexionType(tiles), i, j);
					}
			}

			int chunkX = m_chunkX - (i < 0) + (i >= Chunk::chunkSize);
			int chunkY = m_chunkY - (j < 0) + (j >= Chunk::chunkSize);

			size_t newX = i < 0 ? Chunk::chunkSize - 1 : i > Chunk::chunkSize ? 0 : i;
			size_t newY = j < 0 ? Chunk::chunkSize - 1 : j > Chunk::chunkSize ? 0 : j;
			assert(!(chunkX == m_chunkX && chunkY == m_chunkY));
			m_worldRender.onBoderBlockUpdate(chunkX, chunkY, newX, newY, 0);
		}
	cleanLayers();
}

void ChunkGroundRenderBehaviour::onFullMapChange()
{
	//clear
	onLayerRemove();

	auto chunkPos = m_map.worldToLocalChunkPos(m_chunkX, m_chunkY);
	auto pos = m_map.tilePosToPos(Nz::Vector2ui(0, 0), chunkPos);
	auto mat = m_map.getTiles(pos.x - 1, pos.y - 1, Chunk::chunkSize + 2, Chunk::chunkSize + 2, 0);

	for (int i = 0; i < Chunk::chunkSize; i++)
		for (int j = 0; j < Chunk::chunkSize; j++)
		{
			clearTile(static_cast<unsigned int>(i), static_cast<unsigned int>(j));

			Tile centerTile = mat(i + 1, j + 1);
			std::vector<size_t> setMats;
			for (size_t m = 0; m < 3; m++)
				for (size_t n = 0; n < 3; n++)
				{
					auto currentID = mat(m, n).id;
					if (currentID > centerTile.id || std::find(setMats.begin(), setMats.end(), currentID) != setMats.end())
						continue;
					setMats.push_back(currentID);

					FixedMatrix<bool, 3, 3> tiles;
					for (int k = 0; k < 3; k++)
						for (int l = 0; l < 3; l++)
							tiles(k, l) = mat(i + k, j + l).id == centerTile.id;

					setTile(currentID, localMatrixToTileConnexionType(tiles), i, j);
				}
		}

	//update corners
	m_worldRender.onBoderBlockUpdate(m_chunkX - 1, m_chunkY - 1, Chunk::chunkSize - 1, Chunk::chunkSize - 1, 0);
	m_worldRender.onBoderBlockUpdate(m_chunkX - 1, m_chunkY + 1, Chunk::chunkSize - 1, 0, 0);
	m_worldRender.onBoderBlockUpdate(m_chunkX + 1, m_chunkY - 1, 0, Chunk::chunkSize - 1, 0);
	m_worldRender.onBoderBlockUpdate(m_chunkX + 1, m_chunkY + 1, 0, 0, 0);

	//update sides
	for (size_t i = 0; i < Chunk::chunkSize; i++)
	{
		m_worldRender.onBoderBlockUpdate(m_chunkX - 1, m_chunkY, Chunk::chunkSize - 1, i, 0);
		m_worldRender.onBoderBlockUpdate(m_chunkX + 1, m_chunkY, 0, i, 0);
		m_worldRender.onBoderBlockUpdate(m_chunkX, m_chunkY - 1, i, Chunk::chunkSize - 1, 0);
		m_worldRender.onBoderBlockUpdate(m_chunkX, m_chunkY + 1, i, 0, 0);
	}
	cleanLayers();
}

void ChunkGroundRenderBehaviour::setTile(size_t mat, TileConnexionType type, unsigned int x, unsigned int y)
{
	auto & renderer = getEntity()->GetComponent<Ndk::GraphicsComponent>();

	if (mat == 0)
		return;

	auto it = std::find_if(m_tilemaps.begin(), m_tilemaps.end(), [mat](const auto & map) {return map.materialIndex == mat; });
	if (it == m_tilemaps.end())
	{
		assert(m_definition->isMaterialAllowedOnLayer(mat, 0));
		auto textures = m_definition->texturesIndexsForMaterial(mat);

		auto tilemap = Nz::TileMap::New(Nz::Vector2ui(Chunk::chunkSize, Chunk::chunkSize), Nz::Vector2f(1, 1), textures.size(), Nz::Vector2f(0, 1));

		for (size_t i = 0; i < textures.size(); i++)
		{
			auto mat = Nz::Material::New(*Ressource<Nz::Material>::get("default"));
			mat->SetDiffuseMap(m_definition->getTexture(textures[i]));
			tilemap->SetMaterial(i, mat);
		}
		renderer.Attach(tilemap, Nz::Matrix4f::Translate(Nz::Vector3f(0, 0, - 2.0f)));
		m_tilemaps.push_back(TilemapInfos{ tilemap, std::move(textures), mat, 0 });

		it = m_tilemaps.end() - 1;
	}

	auto id = m_definition->getRandomTile(mat, type, StaticRandomGenerator<std::mt19937>());
	drawTile(*it, x, y, id.tileID, id.textureID);
}

void ChunkGroundRenderBehaviour::clearTile(unsigned int x, unsigned int y)
{
	for (auto & m : m_tilemaps)
		drawTile(m, x, y, 0, 0);
}

void ChunkGroundRenderBehaviour::cleanLayers()
{
	m_tilemaps.erase(std::remove_if(m_tilemaps.begin(), m_tilemaps.end(), [](const auto & m) {return m.setTilesCount == 0; }), m_tilemaps.end());
	updateMaterialsHeights();
}

void ChunkGroundRenderBehaviour::drawTile(ChunkGroundRenderBehaviour::TilemapInfos & map, unsigned int x, unsigned int y, size_t id, size_t textureIndex)
{
	if (id == 0)
	{
		map.tilemap->DisableTile(Nz::Vector2ui(x, y));
		return;
	}
	auto it = std::find(map.texturesIndexs.begin(), map.texturesIndexs.end(), textureIndex);
	if (it == map.texturesIndexs.end())
	{
		map.tilemap->DisableTile(Nz::Vector2ui(x, y));
		return;
	}
	size_t texID = std::distance(map.texturesIndexs.begin(), it);
	auto mat = map.tilemap->GetMaterial(texID);
	assert(mat.IsValid());
	assert(mat->HasDiffuseMap());
	auto tex = mat->GetDiffuseMap();
	auto texSize = tex->GetSize();
	auto nbWidth = (texSize.x + Chunk::tileSize) / (Chunk::tileSize + Chunk::tileDelta);
	assert(nbWidth > 0);
	float tX = (id - 1) % nbWidth * (Chunk::tileSize + Chunk::tileDelta) / static_cast<float>(texSize.x);
	float tY = (id - 1) / nbWidth * (Chunk::tileSize + Chunk::tileDelta) / static_cast<float>(texSize.y);
	map.tilemap->EnableTile(Nz::Vector2ui(x, y), Nz::Rectf(tX, tY, Chunk::tileSize / static_cast<float>(texSize.x), Chunk::tileSize / static_cast<float>(texSize.y)));
}

void ChunkGroundRenderBehaviour::updateMaterialsHeights()
{
	auto & renderer = getEntity()->GetComponent<Ndk::GraphicsComponent>();

	std::vector<size_t> orderedMats;
	for (const auto & m : m_tilemaps)
		orderedMats.push_back(m.materialIndex);
	std::sort(orderedMats.begin(), orderedMats.end());

	for (size_t i = 0; i < orderedMats.size(); i++)
	{
		auto it = std::find_if(m_tilemaps.begin(), m_tilemaps.end(), [mat = orderedMats[i]](const auto & map){return map.materialIndex == mat; });
		assert(it != m_tilemaps.end());

		renderer.UpdateLocalMatrix(it->tilemap, Nz::Matrix4f::Translate(Nz::Vector3f(0, 0, static_cast<float>(i) - orderedMats.size() - 1.0f)));
	}
}