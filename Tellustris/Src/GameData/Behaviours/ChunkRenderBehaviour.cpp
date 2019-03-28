
#include "GameData/Behaviours/ChunkRenderBehaviour.h"
#include "Utility/Ressource.h"
#include "GameData/Behaviours/WorldRenderBehaviour2.h"
#include "Utility/StaticRandomGenerator.h"

#include <NDK/Components/GraphicsComponent.hpp>

#include <cassert>

ChunkRenderBehaviour::ChunkRenderBehaviour(Chunk & chunk, WorldMap & map, WorldRenderBehaviour2 & worldRender, int chunkX, int chunkY, TileDefinitionRef definition)
	: m_chunk(chunk)
	, m_map(map)
	, m_worldRender(worldRender)
	, m_chunkX(chunkX)
	, m_chunkY(chunkY)
	, m_definition(definition)
{
	m_layerChangedHolder = chunk.registerLayerChangedCallback([this](const auto & layerChanged) {onLayerChange(layerChanged.layer, layerChanged.state); });
}

BehaviourRef ChunkRenderBehaviour::clone() const
{
	auto render = std::make_unique<ChunkRenderBehaviour>(m_chunk, m_map, m_worldRender, m_chunkX, m_chunkY, m_definition);
	return std::move(render);
}

void ChunkRenderBehaviour::onBoderBlockUpdate(size_t x, size_t y, size_t layer)
{
	if (layer == 0)
		return;

	assert(x < Chunk::chunkSize && y < Chunk::chunkSize);
	if (layer >= m_chunk.layerCount())
		return;

	auto chunkPos = m_map.worldToLocalChunkPos(m_chunkX, m_chunkY);
	auto pos = m_map.tilePosToPos(Nz::Vector2ui(static_cast<unsigned int>(x), static_cast<unsigned int>(y)), chunkPos);
	auto mat = m_map.getTiles(pos.x - 1, pos.y - 1, 3, 3, layer);

	FixedMatrix<bool, 3, 3> tiles;	

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			tiles(i, j) = mat(i, j).id == mat(1, 1).id;

	auto id = m_definition->getRandomTile(mat(1, 1).id, localMatrixToTileConnexionType(tiles), StaticRandomGenerator<std::mt19937>());
	drawTile(m_tilemaps[layer-1], static_cast<unsigned int>(x), static_cast<unsigned int>(y), id.tileID, id.textureID);
}
	
void ChunkRenderBehaviour::onEnable()
{
	for (size_t i = 1; i < m_chunk.layerCount(); i++)
		onLayerAdd(i);
}

void ChunkRenderBehaviour::onDisable()
{
	if (!haveEntity())
		return;

	auto & graph = getEntity()->GetComponent<Ndk::GraphicsComponent>();

	for (const auto & m : m_tilemaps)
		graph.Detach(m.tilemap);
	m_tilemaps.clear();
}

void ChunkRenderBehaviour::onLayerChange(size_t layer, Chunk::LayerChanged::ChangeState state)
{
	switch (state)
	{
	case Chunk::LayerChanged::ChangeState::added:
		onLayerAdd(layer);
		break;
	case Chunk::LayerChanged::ChangeState::removed:
		onLayerRemove(layer);
	}
}

void ChunkRenderBehaviour::onMapChange(size_t layer, size_t x, size_t y)
{
	if (layer == 0)
		return;

	auto map = m_chunk.getMap(layer);

	if (x >= map->width() || y >= map->height())
		onFullMapChange(layer);
	else onTileChange(layer, x, y);
}

void ChunkRenderBehaviour::onLayerAdd(size_t layer)
{
	if (layer == 0)
		return;

	assert(layer == m_tilemaps.size() + 1);

	auto & graph = getEntity()->GetComponent<Ndk::GraphicsComponent>();
		
	std::vector<size_t> texturesIndexs;
	for (size_t i = 0; i < m_definition->materialCount(); i++)
	{
		if (!m_definition->isMaterialAllowedOnLayer(i+1, layer))
			continue;
		auto textures = m_definition->texturesIndexsForMaterial(i+1);
		for (auto t : textures)
			if (std::find(texturesIndexs.begin(), texturesIndexs.end(), t) == texturesIndexs.end())
				texturesIndexs.push_back(t);
	}

	auto tilemap = Nz::TileMap::New(Nz::Vector2ui(Chunk::chunkSize, Chunk::chunkSize), Nz::Vector2f(1, 1), texturesIndexs.size());
		
	for (size_t i = 0 ; i < texturesIndexs.size() ; i++)
	{
		auto mat = Nz::Material::New(*Ressource<Nz::Material>::get("default"));
		mat->SetDiffuseMap(m_definition->getTexture(texturesIndexs[i]));
		tilemap->SetMaterial(i, mat);
	}
	graph.Attach(tilemap, Nz::Matrix4f::Translate(Nz::Vector3f(0, 0, layer - 2.0f)));
	m_tilemaps.push_back(TilemapInfos{ tilemap, std::move(texturesIndexs) });

	m_mapModified.push_back(m_chunk.getMap(layer)->registerTilemapModifiedCallback([this, layer](const auto & e) {onTileChange(e.x, e.y, layer); }));

	onFullMapChange(layer);
}

void ChunkRenderBehaviour::onLayerRemove(size_t layer)
{
	if (layer == 0)
		return;

	assert(layer == m_tilemaps.size());

	auto & graph = getEntity()->GetComponent<Ndk::GraphicsComponent>();

	graph.Detach(m_tilemaps.back().tilemap);
	m_tilemaps.pop_back();
	m_mapModified.pop_back();
}

void ChunkRenderBehaviour::onTileChange(size_t x, size_t y, size_t layer)
{
	if (layer == 0)
		return;

	assert(layer < m_tilemaps.size());

	auto chunkPos = m_map.worldToLocalChunkPos(m_chunkX, m_chunkY);
	auto pos = m_map.tilePosToPos(Nz::Vector2ui(static_cast<unsigned int>(x), static_cast<unsigned int>(y)), chunkPos);
	auto mat = m_map.getTiles(pos.x - 2, pos.y - 2, 5, 5, layer);

	for (int i = static_cast<int>(x) - 1; i <= x + 1; i++)
		for (int j = static_cast<int>(y) - 1; j <= y + 1; j++)
		{
			if (i >= 0 && j >= 0 && i < Chunk::chunkSize && j < Chunk::chunkSize)
			{
				Tile centerTile = mat(i - x + 2, j - y + 2);
				FixedMatrix<bool, 3, 3> tiles;
				for(int k = 0 ; k < 3 ; k++)
					for (int l = 0; l < 3; l++)
						tiles(k, l) = mat(i - x + 1 + k, j - y + 1 + l).id == centerTile.id;

				auto id = m_definition->getRandomTile(centerTile.id, localMatrixToTileConnexionType(tiles), StaticRandomGenerator<std::mt19937>());
				drawTile(m_tilemaps[layer-1], i, j, id.tileID, id.textureID);
			}

			int chunkX = m_chunkX - (i < 0) + (i >= Chunk::chunkSize);
			int chunkY = m_chunkY - (j < 0) + (j >= Chunk::chunkSize);

			size_t newX = i < 0 ? Chunk::chunkSize - 1 : i > Chunk::chunkSize ? 0 : i;
			size_t newY = j < 0 ? Chunk::chunkSize - 1 : j > Chunk::chunkSize ? 0 : j;
			assert(!(chunkX == m_chunkX && chunkY == m_chunkY));
			m_worldRender.onBoderBlockUpdate(chunkX, chunkY, newX, newY, layer);
		}
}

void ChunkRenderBehaviour::onFullMapChange(size_t layer)
{
	auto chunkPos = m_map.worldToLocalChunkPos(m_chunkX, m_chunkY);
	auto pos = m_map.tilePosToPos(Nz::Vector2ui(0, 0), chunkPos);
	auto mat = m_map.getTiles(pos.x - 1, pos.y - 1, Chunk::chunkSize + 2, Chunk::chunkSize + 2, layer);

	for (int i = 0; i < Chunk::chunkSize; i++)
		for (int j = 0; j < Chunk::chunkSize; j++)
		{
			Tile centerTile = mat(i + 1, j + 1);
			FixedMatrix<bool, 3, 3> tiles;
			for (int k = 0; k < 3; k++)
				for (int l = 0; l < 3; l++)
					tiles(k, l) = mat(i + k, j + l).id == centerTile.id;

			auto id = m_definition->getRandomTile(centerTile.id, localMatrixToTileConnexionType(tiles), StaticRandomGenerator<std::mt19937>());
			drawTile(m_tilemaps[layer-1], i, j, id.tileID, id.textureID);
		}

	//update corners
	m_worldRender.onBoderBlockUpdate(m_chunkX - 1, m_chunkY - 1, Chunk::chunkSize - 1, Chunk::chunkSize - 1, layer);
	m_worldRender.onBoderBlockUpdate(m_chunkX - 1, m_chunkY + 1, Chunk::chunkSize - 1, 0, layer);
	m_worldRender.onBoderBlockUpdate(m_chunkX + 1, m_chunkY - 1, 0, Chunk::chunkSize - 1, layer);
	m_worldRender.onBoderBlockUpdate(m_chunkX + 1, m_chunkY + 1, 0, 0, layer);

	//update sides
	for (size_t i = 0; i < Chunk::chunkSize; i++)
	{
		m_worldRender.onBoderBlockUpdate(m_chunkX - 1, m_chunkY, Chunk::chunkSize - 1, i, layer);
		m_worldRender.onBoderBlockUpdate(m_chunkX + 1, m_chunkY, 0, i, layer);
		m_worldRender.onBoderBlockUpdate(m_chunkX, m_chunkY - 1, i, Chunk::chunkSize - 1, layer);
		m_worldRender.onBoderBlockUpdate(m_chunkX, m_chunkY + 1, i, 0, layer);
	}
}

void ChunkRenderBehaviour::drawTile(ChunkRenderBehaviour::TilemapInfos & map, unsigned int x, unsigned int y, size_t id, size_t textureIndex)
{
	if (id == 0)
	{
		map.tilemap->DisableTile(Nz::Vector2ui(x, y));
		return;
	}
	auto it = std::find(map.texturesIndexs.begin(), map.texturesIndexs.end(), textureIndex);
	if(it == map.texturesIndexs.end())
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