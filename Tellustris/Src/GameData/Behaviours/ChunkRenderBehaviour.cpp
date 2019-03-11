
#include "GameData/Behaviours/ChunkRenderBehaviour.h"
#include "Utility/Ressource.h"

#include <NDK/Components/GraphicsComponent.hpp>

#include <cassert>

ChunkRenderBehaviour::ChunkRenderBehaviour(Chunk & chunk, WorldMap & map, size_t chunkX, size_t chunkY, TileDefinitionRef definition)
	: m_chunk(chunk)
	, m_map(map)
	, m_chunkX(chunkX)
	, m_chunkY(chunkY)
	, m_definition(definition)
{
	m_layerChangedHolder = chunk.registerLayerChangedCallback([this](const auto & layerChanged) {onLayerChange(layerChanged.layer, layerChanged.state); });
}

BehaviourRef ChunkRenderBehaviour::clone() const
{
	auto render = std::make_unique<ChunkRenderBehaviour>(m_chunk, m_map, m_chunkX, m_chunkY, m_definition);
	return std::move(render);
}
	
void ChunkRenderBehaviour::onEnable()
{
	for (size_t i = 0; i < m_chunk.layerCount(); i++)
		onLayerAdd(i);
}

void ChunkRenderBehaviour::onDisable()
{
	auto & graph = getEntity()->GetComponent<Ndk::GraphicsComponent>();

	for (const auto & m : m_tilemaps)
		graph.Detach(m.tilemap);
	m_tilemaps.clear();

	for (const auto & m : m_tilemapsGround)
		graph.Detach(m.tilemap);
	m_tilemapsGround.clear();
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
	auto map = m_chunk.getMap(layer);
	if (layer == 0)
	{
		if (x >= map->width() || y >= map->height())
			onFullMapGroundChange();
		else onTileGroundChange(x, y);
	}
	else
	{
		if (x >= map->width() || y >= map->height())
			onFullMapChange(layer);
		else onTileChange(layer, x, y);
	}
}

void ChunkRenderBehaviour::onLayerAdd(size_t layer)
{
	if (layer == 0)
		onFullMapGroundChange();
	else
	{
		assert(layer == m_tilemaps.size() - 1);

		auto & graph = getEntity()->GetComponent<Ndk::GraphicsComponent>();
		
		std::vector<size_t> texturesIndexs;
		for (size_t i = 0; i < m_definition->materialCount(); i++)
		{
			if (!m_definition->isMaterialAllowedOnLayer(i, layer))
				continue;
			auto textures = m_definition->texturesIndexsForMaterial(i);
			for (auto t : textures)
				if (std::find(texturesIndexs.begin(), texturesIndexs.end(), t) == texturesIndexs.end())
					texturesIndexs.push_back(t);
		}

		auto tilemap = Nz::TileMap::New(Nz::Vector2ui(Chunk::chunkSize, Chunk::chunkSize), Nz::Vector2f(1, 1), texturesIndexs.size(), Nz::Vector2f(0, 1));
		
		for (size_t i = 0 ; i < texturesIndexs.size() ; i++)
		{
			auto mat = Nz::Material::New(*Ressource<Nz::Material>::get("default"));
			mat->SetDiffuseMap(m_definition->getTexture(texturesIndexs[i]));
			tilemap->SetMaterial(i, mat);
		}
		graph.Attach(tilemap, Nz::Matrix4f::Translate(Nz::Vector3f(0, 0, layer - 2.0f)));
		m_tilemaps.push_back(TilemapInfos{ tilemap, std::move(texturesIndexs) });
	}
}

void ChunkRenderBehaviour::onLayerRemove(size_t layer)
{
	auto & graph = getEntity()->GetComponent<Ndk::GraphicsComponent>();

	if (layer == 0)
	{
		for (const auto & m : m_tilemapsGround)
			graph.Detach(m.tilemap);
		m_tilemapsGround.clear();
	}
	else
	{
		assert(layer == m_tilemaps.size() - 2);

		graph.Detach(m_tilemaps.back().tilemap);
		m_tilemaps.pop_back();
	}
}

void ChunkRenderBehaviour::onTileChange(size_t x, size_t y, size_t layer)
{

}

void ChunkRenderBehaviour::onFullMapChange(size_t layer)
{

}

void ChunkRenderBehaviour::onTileGroundChange(size_t x, size_t y)
{

}

void ChunkRenderBehaviour::onFullMapGroundChange()
{

}