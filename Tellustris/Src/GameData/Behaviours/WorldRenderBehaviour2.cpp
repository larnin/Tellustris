
#include "GameData/Behaviours/WorldRenderBehaviour2.h"
#include "GameData/Behaviours/ChunkRenderBehaviour.h"
#include "Components/BehaviourComponent.h"

#include <NDK/World.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Components/GraphicsComponent.hpp>

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

void WorldRenderBehaviour2::onEnable()
{
	for (auto & c : m_chunks)
		c.entity->Disable();
}

void WorldRenderBehaviour2::onDisable()
{
	for (auto & c : m_chunks)
		c.entity->Enable();
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
	behaviour.attach(std::make_unique<ChunkRenderBehaviour>(m_map.getChunk(x, y), m_map, x, y, m_definition));

	m_chunks.push_back(ChunkInfo{ entity, x, y });
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