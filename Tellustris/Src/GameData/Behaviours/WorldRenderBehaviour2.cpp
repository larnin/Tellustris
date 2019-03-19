
#include "GameData/Behaviours/WorldRenderBehaviour2.h"
#include "GameData/Behaviours/ChunkRenderBehaviour.h"
#include "GameData/Behaviours/ChunkGroundRenderBehaviour.h"
#include "Components/BehaviourComponent.h"

#include <NDK/World.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Components/GraphicsComponent.hpp>

#include <cassert>

WorldRenderBehaviour2::ChunkBorder::ChunkBorder(Chunk & chunk, WorldRenderBehaviour2 & render, int chunkX, int chunkY)
	: m_chunk(chunk)
	, m_worldRender(render)
	, m_chunkX(chunkX)
	, m_chunkY(chunkY)
{
	m_layerChangedHolder = m_chunk.registerLayerChangedCallback([this](const auto & layerChanged) {onLayerChange(layerChanged.layer, layerChanged.state); });
	for (size_t layer = 0; layer < m_chunk.layerCount(); layer++)
		onLayerAdd(layer);
}

WorldRenderBehaviour2::ChunkBorder::ChunkBorder(const ChunkBorder & border)
	: m_chunk(border.m_chunk)
	, m_worldRender(border.m_worldRender)
	, m_chunkX(border.m_chunkX)
	, m_chunkY(border.m_chunkY)
{
	m_layerChangedHolder = m_chunk.registerLayerChangedCallback([this](const auto & layerChanged) {onLayerChange(layerChanged.layer, layerChanged.state); });	
	for (size_t layer = 0; layer < m_chunk.layerCount(); layer++)
		onLayerAdd(layer);
}

void WorldRenderBehaviour2::ChunkBorder::onLayerChange(size_t layer, Chunk::LayerChanged::ChangeState state)
{
	if (state == Chunk::LayerChanged::ChangeState::added)
		onLayerAdd(layer);
	else onLayerRemove(layer);
}

void WorldRenderBehaviour2::ChunkBorder::onMapChange(size_t layer, size_t x, size_t y)
{
	if (x >= Chunk::chunkSize || y > Chunk::chunkSize)
		return;
	if (x > 0 && y > 0 && x < Chunk::chunkSize - 1 && y < Chunk::chunkSize - 1)
		return;
	//only update borders

	for(int i = static_cast<int>(x) - 1 ; i <= x + 1 ; i++)
		for (int j = static_cast<int>(y) - 1; j <= y + 1; j++)
		{
			if (i >= 0 && j >= 0 && i < Chunk::chunkSize && j < Chunk::chunkSize)
				continue;

			int chunkX = m_chunkX - (i < 0) + (i >= Chunk::chunkSize);
			int chunkY = m_chunkY - (j < 0) + (j >= Chunk::chunkSize);

			size_t newX = i < 0 ? Chunk::chunkSize - 1 : i > Chunk::chunkSize ? 0 : i;
			size_t newY = j < 0 ? Chunk::chunkSize - 1 : j > Chunk::chunkSize ? 0 : j;
			assert(!(chunkX == m_chunkX && chunkY == m_chunkY));
			m_worldRender.onBoderBlockUpdate(chunkX, chunkY, newX, newY, layer);
		}
}

void WorldRenderBehaviour2::ChunkBorder::onLayerAdd(size_t layer)
{
	assert(m_mapModified.size() == layer);

	m_mapModified.push_back(m_chunk.getMap(layer)->registerTilemapModifiedCallback([this, layer](const auto & e) {onMapChange(layer, e.x, e.y); }));
}

void WorldRenderBehaviour2::ChunkBorder::onLayerRemove(size_t layer)
{
	assert(m_mapModified.size() == layer + 1);

	m_mapModified.pop_back();
}

WorldRenderBehaviour2::WorldRenderBehaviour2(WorldMap & map, TileDefinitionRef definition, float viewSize)
	: m_definition(definition)
	, m_map(map)
	, m_viewSize(viewSize)
{
	m_CenterViewUpdateHolder = StaticEvent<CenterViewUpdate>::connect([this](const auto & e) {onCenterViewUpdate(e.x, e.y); });
}

BehaviourRef WorldRenderBehaviour2::clone() const
{
	auto render = std::make_unique<WorldRenderBehaviour2>(m_map, m_definition, m_viewSize);
	return std::move(render);
}

void WorldRenderBehaviour2::onBoderBlockUpdate(size_t chunkX, size_t chunkY, size_t x, size_t y, size_t layer)
{
	auto it = std::find_if(m_chunks.begin(), m_chunks.end(), [chunkX, chunkY](const auto & c) {return c.x == chunkX && c.y == chunkY; });
	if (it == m_chunks.end())
		return;
	it->behaviour->onBoderBlockUpdate(x, y, layer);
	it->groundBehaviour->onBoderBlockUpdate(x, y, layer);
}

void WorldRenderBehaviour2::onEnable()
{
	for (auto & c : m_chunks)
	{
		c.entity->Disable();
		c.groundEntity->Disable();
	}
}

void WorldRenderBehaviour2::onDisable()
{
	for (auto & c : m_chunks)
	{
		c.entity->Enable();
		c.groundEntity->Enable();
	}
}

void WorldRenderBehaviour2::onCenterViewUpdate(float x, float y)
{
	auto viewChunks = getViewChunks(x, y);
	assert(!viewChunks.empty());

	for (const auto & c : viewChunks)
	{
		auto it = std::find_if(m_chunks.begin(), m_chunks.end(), [c](const auto & chunk) {return c.x == chunk.x && c.y == chunk.y; });
		if (it == m_chunks.end())
			addChunk(c.x, c.y);
	}

	for (size_t i = 0; i < m_chunks.size(); i++)
	{
		const auto & chunk = m_chunks[i];

		auto it = std::find_if(viewChunks.begin(), viewChunks.end(), [&chunk](const auto & c) {return c.x == chunk.x && c.y == chunk.y; });
		if (it == viewChunks.end())
		{
			removeChunk(i);
			i--;
		}
	}
}

void WorldRenderBehaviour2::addChunk(int x, int y)
{
	auto entity = getEntity()->GetWorld()->CreateEntity();
	auto & node = entity->AddComponent<Ndk::NodeComponent>();
	entity->AddComponent<Ndk::GraphicsComponent>();
	auto & behaviour = entity->AddComponent<BehaviourComponent>();
	node.SetParent(getEntity()->GetComponent<Ndk::NodeComponent>());
	node.SetPosition(static_cast<float>(x) * Chunk::chunkSize, static_cast<float>(y) * Chunk::chunkSize, static_cast<float>(y) * Chunk::chunkSize);
	auto chunkBehaviour = std::make_unique<ChunkRenderBehaviour>(m_map.getChunk(x, y), m_map, *this, x, y, m_definition);

	auto entity2 = getEntity()->GetWorld()->CreateEntity();
	auto & node2 = entity2->AddComponent<Ndk::NodeComponent>();
	entity2->AddComponent<Ndk::GraphicsComponent>();
	auto & behaviour2 = entity2->AddComponent<BehaviourComponent>();
	node2.SetParent(getEntity()->GetComponent<Ndk::NodeComponent>());
	node2.SetPosition(static_cast<float>(x) * Chunk::chunkSize, static_cast<float>(y) * Chunk::chunkSize, static_cast<float>(y) * Chunk::chunkSize);
	auto chunkBehaviour2 = std::make_unique<ChunkGroundRenderBehaviour>(m_map.getChunk(x, y), m_map, *this, x, y, m_definition);

	m_chunks.push_back(ChunkInfo{ entity, chunkBehaviour.get(), entity2, chunkBehaviour2.get(), x, y });
	behaviour.attach(std::move(chunkBehaviour));

}

void WorldRenderBehaviour2::removeChunk(size_t index)
{
	auto & c = m_chunks[index];
	c.entity->Kill();
	m_chunks.erase(m_chunks.begin() + index);
}

std::vector<Nz::Vector2i> WorldRenderBehaviour2::getViewChunks(float x, float y) const
{
	auto min = m_map.posToWorldChunkPos(x - m_viewSize, y - m_viewSize);
	auto max = m_map.posToWorldChunkPos(x + m_viewSize, y + m_viewSize);

	std::vector<Nz::Vector2i> chunks;
	for (int i = min.x; i <= max.x; i++)
		for (int j = min.y; j <= max.y; j++)
			chunks.push_back({ i, j });

	return chunks;
}