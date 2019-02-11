#pragma once

#include "GameData/WorldMap.h"
#include "Behaviour.h"
#include "GameData/WorldMap.h"
#include "Utility/Event/Event.h"
#include "Utility/Event/Args.h"
#include "Tilemap/Tilemap.h"
#include "GameData/TileRenderDefinition.h"

#include <NDK/Entity.hpp>

#include <vector>

class WorldRenderBehaviour : public Behaviour
{
	struct ChunkLayerInfo
	{
		ChunkLayerInfo(const ChunkLayerInfo &) = delete;
		ChunkLayerInfo & operator=(const ChunkLayerInfo &) = delete;
		ChunkLayerInfo(ChunkLayerInfo &&) noexcept = default;
		ChunkLayerInfo & operator=(ChunkLayerInfo &&) noexcept = default;

		TilemapRef tilemap;
		Ndk::EntityHandle entity;
		EventHolder<Tilemap::TilemapModified> mapModifiedHolder;
	};

	struct ChunkInfo
	{
		ChunkInfo(const ChunkInfo &) = delete;
		ChunkInfo & operator=(const ChunkInfo &) = delete;
		ChunkInfo(ChunkInfo &&) noexcept = default;
		ChunkInfo & operator=(ChunkInfo &&) noexcept = default;

		int x;
		int y;
		Chunk * chunk;

		std::vector<ChunkLayerInfo> layers;
		EventHolder<Chunk::LayerChanged> layerModifierHolder;
	};

public:
	WorldRenderBehaviour(WorldMap & map, float viewSize);

	BehaviourRef clone() const override;

	void setDefaultTileDef(TileRenderDefinitionRef def);
	void setTileDef(TileRenderDefinitionRef def, size_t layer);

protected:
	void onEnable() override;
	void onDisable() override;

private:
	void onCenterViewUpdate(float x, float y);
	void onLayerUpdate(int chunkX, int chunkY, size_t layer, Chunk::LayerChanged::ChangeState state);
	void onTileUpdate(int chunkX, int chunkY, size_t layer, size_t x, size_t y);

	std::vector<Nz::Vector2i> getViewChunks(float x, float y);
	void addNewChunk(int x, int y);
	void removeChunk(size_t index);
	void addNewLayer(int x, int y, size_t layer);
	void removeLayer(int x, int y, size_t layer);
	void updateLayerHeight(int x, int y, size_t layer);

	void drawLayer(TilemapRef map, int x, int y, size_t layer);
	void drawTile(TilemapRef map, int mapX, int mapY, int x, int y, size_t layer);

	WorldMap & m_map;
	float m_viewSize;

	EventHolder<CenterViewUpdate> m_CenterViewUpdateHolder;
	
	std::vector<ChunkInfo> m_chunkInfos;

	std::vector<TileRenderDefinitionRef> m_tilesDef;
	TileRenderDefinitionRef m_defaultTileDef;
};