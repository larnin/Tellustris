#pragma once

#include "Behaviour.h"
#include "GameData/Chunk.h"
#include "GameData/WorldMap.h"
#include "Utility/Event/Event.h"
#include "GameData/TileDefinition.h"
#include "Tilemap/Tilemap.h"

#include <Nazara/Graphics/TileMap.hpp>

#include <vector>

class WorldRenderBehaviour2;

class ChunkGroundRenderBehaviour : public Behaviour
{
	struct TilemapInfos
	{
		Nz::TileMapRef tilemap;
		std::vector<size_t> texturesIndexs;
		size_t materialIndex;
		size_t setTilesCount;
	};

public:
	ChunkGroundRenderBehaviour(Chunk & chunk, WorldMap & map, WorldRenderBehaviour2 & worldRender, int chunkX, int chunkY, TileDefinitionRef definition);

	BehaviourRef clone() const override;

	void onBoderBlockUpdate(size_t x, size_t y, size_t layer);

protected:
	void onEnable() override;
	void onDisable() override;

private:
	void onLayerChange(size_t layer, Chunk::LayerChanged::ChangeState state);
	void onMapChange(size_t layer, size_t x, size_t y);

	void onLayerAdd();
	void onLayerRemove();
	void onTileChange(size_t x, size_t y);
	void onFullMapChange();

	void setTile(size_t mat, TileConnexionType type, unsigned int x, unsigned int y);
	void clearTile(unsigned int x, unsigned int y);
	void cleanLayers();
	void drawTile(TilemapInfos & map, unsigned int x, unsigned int y, size_t id, size_t textureIndex);

	void updateMaterialsHeights();

	Chunk & m_chunk;
	WorldMap & m_map;
	WorldRenderBehaviour2 & m_worldRender;
	int m_chunkX;
	int m_chunkY;
	TileDefinitionRef m_definition;
	std::vector<TilemapInfos> m_tilemaps;

	EventHolder<Chunk::LayerChanged> m_layerChangedHolder;
	EventHolder<Tilemap::TilemapModified> m_mapModified;
};