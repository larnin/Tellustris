
#include "GameData/Behaviours/WorldRenderBehaviour.h"
#include "Components/TilemapComponent.h"
#include "Components/TilemapColliderComponent.h"
#include "Components/TilemapAnimationComponent.h"
#include "Utility/Ressource.h"

#include <NDK/World.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Components/GraphicsComponent.hpp>

#include <algorithm>

WorldRenderBehaviour::WorldRenderBehaviour(WorldMap & map, float viewSize)
	: m_map(map)
	, m_viewSize(viewSize)
{
	m_CenterViewUpdateHolder = StaticEvent<CenterViewUpdate>::connect([this](const auto & e) {onCenterViewUpdate(e.x, e.y); });
}

BehaviourRef WorldRenderBehaviour::clone() const
{
	auto render = std::make_unique<WorldRenderBehaviour>(m_map, m_viewSize);
	render->setDefaultTileDef(m_defaultTileDef);
	for (size_t i = 0; i < m_tilesDef.size(); i++)
		render->setTileDef(m_tilesDef[i], i);
	return std::move(render);
}

void WorldRenderBehaviour::setDefaultTileDef(TileRenderDefinitionRef def)
{
	m_defaultTileDef = def;
}

void WorldRenderBehaviour::setTileDef(TileRenderDefinitionRef def, size_t layer)
{
	if (!def && layer >= m_tilesDef.size())
		return;

	for (size_t i = m_tilesDef.size(); i <= layer; i++)
		m_tilesDef.push_back({});
	m_tilesDef[layer] = def;
}

void WorldRenderBehaviour::onEnable()
{
	for(auto & c : m_chunkInfos)
		for (auto & l : c.layers)
			l.entity->Enable();
}

void WorldRenderBehaviour::onDisable()
{
	for (auto & c : m_chunkInfos)
		for (auto & l : c.layers)
			l.entity->Disable();
}

void WorldRenderBehaviour::onCenterViewUpdate(float x, float y)
{
	auto viewChunks = getViewChunks(x, y);
	assert(!viewChunks.empty());

	for (const auto & c : viewChunks)
	{
		auto it = std::find_if(m_chunkInfos.begin(), m_chunkInfos.end(), [c](const auto & chunk) {return c.x == chunk.x && c.y == chunk.y; });
		if (it == m_chunkInfos.end())
			addNewChunk(c.x, c.y);
	}

	for (size_t i = 0 ; i < m_chunkInfos.size() ; i++)
	{
		const auto & chunk = m_chunkInfos[i];

		auto it = std::find_if(viewChunks.begin(), viewChunks.end(), [&chunk](const auto & c) {return c.x == chunk.x && c.y == chunk.y; });
		if (it == viewChunks.end())
		{
			removeChunk(i);
			i--;
		}
	}
}

void WorldRenderBehaviour::onLayerUpdate(int chunkX, int chunkY, size_t layer, Chunk::LayerChanged::ChangeState state)
{
	auto it = std::find_if(m_chunkInfos.begin(), m_chunkInfos.end(), [chunkX, chunkY](const auto & c) {return c.x == chunkX && c.y == chunkY; });
	if (it == m_chunkInfos.end())
		return;

	switch (state)
	{
	case Chunk::LayerChanged::ChangeState::added:
		addNewLayer(chunkX, chunkY, layer);
		break;
	case Chunk::LayerChanged::ChangeState::removed:
		removeLayer(chunkX, chunkY, layer);
		break;
	case Chunk::LayerChanged::ChangeState::heightChanged:
		updateLayerHeight(chunkX, chunkY, layer);
		break;
	default:
		assert(false);
		break;
	}
}

void WorldRenderBehaviour::onTileUpdate(int chunkX, int chunkY, size_t layer, size_t x, size_t y)
{
	auto it = std::find_if(m_chunkInfos.begin(), m_chunkInfos.end(), [chunkX, chunkY](const auto & c) {return c.x == chunkX && c.y == chunkY; });
	if (it == m_chunkInfos.end())
		return;

	assert(layer >= it->layers.size());

	auto map = it->layers[layer].tilemap;
	if (x >= map->width() || y >= map->height())
		drawLayer(map, chunkX * static_cast<int>(Chunk::chunkSize), chunkY * static_cast<int>(Chunk::chunkSize), layer);
	else drawTile(map, chunkX * static_cast<int>(Chunk::chunkSize), chunkY * static_cast<int>(Chunk::chunkSize), static_cast<int>(x), static_cast<int>(y), layer);
}

std::vector<Nz::Vector2i> WorldRenderBehaviour::getViewChunks(float x, float y)
{
	auto min = m_map.posToWorldChunkPos(x - m_viewSize, y - m_viewSize);
	auto max = m_map.posToWorldChunkPos(x + m_viewSize, y + m_viewSize);

	std::vector<Nz::Vector2i> chunks;
	for (int i = min.x; i <= max.x; i++)
		for (int j = min.y; j <= max.y; j++)
			chunks.push_back({ i, j });

	return chunks;
}

void WorldRenderBehaviour::addNewChunk(int x, int y)
{
	auto & chunk = m_map.getChunk(x, y);

	ChunkInfo infos{ x, y, &chunk };
	infos.layerModifierHolder = chunk.registerLayerChangedCallback([this, x, y](const Chunk::LayerChanged & c) {onLayerUpdate(x, y, c.layer, c.state); });

	m_chunkInfos.push_back(std::move(infos));

	for (size_t i = 0; i < chunk.layerCount(); i++)
		addNewLayer(x, y, i);
}

void WorldRenderBehaviour::removeChunk(size_t index)
{
	assert(index < m_chunkInfos.size());

	auto & c = m_chunkInfos[index];
	auto size = c.layers.size();
	for (size_t i = 0; i < size; i++)
		removeLayer(c.x, c.y, size - i - 1);

	m_chunkInfos.erase(m_chunkInfos.begin() + index);
}

void WorldRenderBehaviour::addNewLayer(int x, int y, size_t layer)
{
	auto it = std::find_if(m_chunkInfos.begin(), m_chunkInfos.end(), [x, y](const auto & c) {return c.x == x && c.y == y; });
	assert(it != m_chunkInfos.end());

	assert(it->layers.size() == layer);

	auto w = getEntity()->GetWorld();

	auto entity = w->CreateEntity();
	auto & node = entity->AddComponent<Ndk::NodeComponent>();
	auto & graph = entity->AddComponent<Ndk::GraphicsComponent>();
	auto & map = entity->AddComponent<TilemapComponent>();
	//auto & mapAnimation = entity->AddComponent<TilemapAnimationComponent>();
	//auto & mapCollisions = entity->AddComponent<TilemapColliderComponent>();

	node.SetPosition(Nz::Vector3f(static_cast<float>(x) * Chunk::chunkSize, static_cast<float>(y) * Chunk::chunkSize, it->chunk->layerHeight(layer)));

	auto tilemap = Nz::TileMap::New(Nz::Vector2ui(Chunk::chunkSize, Chunk::chunkSize), Nz::Vector2f(1, 1));
	auto tilemapDef = Tilemap::New(Chunk::chunkSize, Chunk::chunkSize, Chunk::tileSize, Chunk::tileDelta);

	//TODO -- Animation definition
	//TODO -- Managed materials and textures to better acces

	auto mat = Nz::Material::New("Translucent3D");
	mat->EnableDepthWrite(true);
	mat->SetDiffuseMap(Ressource<Nz::Texture>::get("Img/tile3.png"));
	tilemap->SetMaterial(0, mat);

	graph.Attach(tilemap);
	map.attachTilemap(tilemapDef);
	map.attachRenderer(tilemap);
	//mapAnimation.attachTilemap(tilemapDef);
	//mapAnimation.attachRenderer(tilemap);
	//mapAnimation.attachAnimations(....);
	//mapCollisions.attachTilemap(tilemapDef);

	ChunkLayerInfo infos{ tilemapDef, entity };
	infos.mapModifiedHolder = it->chunk->getMap(layer)->registerTilemapModifiedCallback([this, x, y, layer](const auto & t) {onTileUpdate(x, y, layer, t.x, t.y); });
	it->layers.push_back(std::move(infos));

	drawLayer(tilemapDef, x * static_cast<int>(Chunk::chunkSize), y * static_cast<int>(Chunk::chunkSize), layer);
}

void WorldRenderBehaviour::removeLayer(int x, int y, size_t layer)
{
	auto it = std::find_if(m_chunkInfos.begin(), m_chunkInfos.end(), [x, y](const auto & c) {return c.x == x && c.y == y; });
	assert(it != m_chunkInfos.end());

	assert(it->layers.size() == layer + 1);

	it->layers[layer].entity->Kill();
	it->layers.erase(it->layers.begin() + layer);
}

void WorldRenderBehaviour::updateLayerHeight(int x, int y, size_t layer)
{
	auto it = std::find_if(m_chunkInfos.begin(), m_chunkInfos.end(), [x, y](const auto & c) {return c.x == x && c.y == y; });
	assert(it != m_chunkInfos.end());

	assert(it->layers.size() == layer);

	auto & node = it->layers[layer].entity->GetComponent<Ndk::NodeComponent>();
	node.SetPosition(static_cast<float>(x), static_cast<float>(y), it->chunk->layerHeight(layer));
}

void WorldRenderBehaviour::drawLayer(TilemapRef map, int x, int y, size_t layer)
{
	TileRenderDefinitionRef def;
	if (layer < m_tilesDef.size())
		def = m_tilesDef[layer];
	if (!def)
		def = m_defaultTileDef;

	auto centerPos = m_map.posToWorldChunkPos(x, y);

	FixedMatrix<Chunk*, 3, 3> chunks(nullptr);
	for (int i = -1; i <= 1; i++)
		for (int j = -1; j <= 1; j++)
			chunks(i + 1, j + 1) = &m_map.getChunk(centerPos.x + i, centerPos.y + j);

	Matrix<Tile> tiles(Chunk::chunkSize + 2, Chunk::chunkSize + 2, {});
	for(int i = 0 ; i < tiles.width() ; i++)
		for (int j = 0; j < tiles.height(); j++)
		{
			int x = i - 1;
			int y = j - 1;
			auto cX = 1;
			auto cY = 1;
			if (x < 0)
			{
				x = Chunk::chunkSize - 1;
				cX = 0;
			}
			else if (x >= Chunk::chunkSize)
			{
				x = 0;
				cX = 2;
			}
			if (y < 0)
			{
				y = Chunk::chunkSize - 1;
				cY = 0;
			}
			else if (y >= Chunk::chunkSize)
			{
				y = 0;
				cY = 2;
			}
			auto * c = chunks(cX, cY);

			tiles(i, j) = c->getTile(x, y, layer);
		}

	for(size_t i = 0 ; i < map->width() ; i++)
		for (size_t j = 0; j < map->height(); j++)
		{
			if (!def)
				map->setTile(i, j, tiles(i + 1, j + 1));
			else
			{
				LocalTileMatrix mat;
				mat(0, 0) = tiles(i, j);
				mat(0, 1) = tiles(i, j + 1);
				mat(0, 2) = tiles(i, j + 2);
				mat(1, 0) = tiles(i + 1, j);
				mat(1, 1) = tiles(i + 1, j + 1);
				mat(1, 2) = tiles(i + 1, j + 2);
				mat(2, 0) = tiles(i + 2, j);
				mat(2, 1) = tiles(i + 2, j + 1);
				mat(2, 2) = tiles(i + 2, j + 2);
				map->setTile(i, j, def->renderTile(mat, x + static_cast<int>(i), y + static_cast<int>(j)));
			}
		}
}

void WorldRenderBehaviour::drawTile(TilemapRef map, int mapX, int mapY, int x, int y, size_t layer)
{
	TileRenderDefinitionRef def;
	if (layer < m_tilesDef.size())
		def = m_tilesDef[layer];
	if (!def)
		def = m_defaultTileDef;

	if (!def)
		map->setTile(x, y, m_map.getTile(x + mapX, y + mapY, layer));
	else
	{
		LocalTileMatrix mat;
		mat(0, 0) = m_map.getTile(x + mapX - 1, y + mapY - 1, layer);
		mat(0, 1) = m_map.getTile(x + mapX - 1, y + mapY, layer);
		mat(0, 2) = m_map.getTile(x + mapX - 1, y + mapY + 1, layer);
		mat(1, 0) = m_map.getTile(x + mapX, y + mapY - 1, layer);
		mat(1, 1) = m_map.getTile(x + mapX, y + mapY, layer);
		mat(1, 2) = m_map.getTile(x + mapX, y + mapY + 1, layer);
		mat(2, 0) = m_map.getTile(x + mapX + 1, y + mapY - 1, layer);
		mat(2, 1) = m_map.getTile(x + mapX + 1, y + mapY, layer);
		mat(2, 2) = m_map.getTile(x + mapX + 1, y + mapY + 1, layer);
		map->setTile(x, y, def->renderTile(mat, x + mapX, y + mapY));
	}
}