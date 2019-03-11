#pragma once

#include "Behaviour.h"
#include "GameData/Chunk.h"
#include "GameData/WorldMap.h"
#include "Utility/Event/Event.h"
#include "GameData/TileDefinition.h"
#include "Tilemap/Tilemap.h"

#include <Nazara/Graphics/TileMap.hpp>

#include <vector>

class ChunkRenderBehaviour : public Behaviour
{
	struct TilemapInfos
	{
		Nz::TileMapRef tilemap;
		std::vector<size_t> texturesIndexs;
	};

public:
	ChunkRenderBehaviour(Chunk & chunk, WorldMap & map, size_t chunkX, size_t chunkY, TileDefinitionRef definition);

	BehaviourRef clone() const override;

protected:
	void onEnable() override;
	void onDisable() override;

private:
	void onLayerChange(size_t layer, Chunk::LayerChanged::ChangeState state);
	void onMapChange(size_t layer, size_t x, size_t y);

	void onLayerAdd(size_t layer);
	void onLayerRemove(size_t layer);
	void onTileChange(size_t x, size_t y, size_t layer);
	void onFullMapChange(size_t layer);
	void onTileGroundChange(size_t x, size_t y);
	void onFullMapGroundChange();

	Chunk & m_chunk;
	WorldMap & m_map;
	size_t m_chunkX;
	size_t m_chunkY;
	TileDefinitionRef m_definition;
	std::vector<TilemapInfos> m_tilemapsGround;
	std::vector<TilemapInfos> m_tilemaps;

	EventHolder<Chunk::LayerChanged> m_layerChangedHolder;
	std::vector<EventHolder<Tilemap::TilemapModified>> m_mapModified;
};