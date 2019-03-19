#pragma once

#include "Behaviour.h"
#include "Utility/Event/Event.h"
#include "Utility/Event/Args.h"
#include "GameData/TileDefinition.h"
#include "GameData/WorldMap.h"

#include <NDK/Entity.hpp>

#include <vector>

class ChunkRenderBehaviour;
class ChunkGroundRenderBehaviour;

class WorldRenderBehaviour2 : public Behaviour
{
	struct ChunkInfo
	{
		Ndk::EntityHandle entity;
		ChunkRenderBehaviour * behaviour;
		Ndk::EntityHandle groundEntity;
		ChunkGroundRenderBehaviour * groundBehaviour;
		int x;
		int y;
	};

	class ChunkBorder
	{
	public:
		ChunkBorder(Chunk & chunk, WorldRenderBehaviour2 & render, int chunkX, int chunkY);
		ChunkBorder(const ChunkBorder & border);

	private:
		void onLayerChange(size_t layer, Chunk::LayerChanged::ChangeState state);
		void onMapChange(size_t layer, size_t x, size_t y);
		void onLayerAdd(size_t layer);
		void onLayerRemove(size_t layer);

		Chunk & m_chunk;
		WorldRenderBehaviour2 & m_worldRender;
		int m_chunkX;
		int m_chunkY;

		EventHolder<Chunk::LayerChanged> m_layerChangedHolder;
		std::vector<EventHolder<Tilemap::TilemapModified>> m_mapModified;
	};

public:
	WorldRenderBehaviour2(WorldMap & map, TileDefinitionRef definition, float viewSize);
	BehaviourRef clone() const override;

	void onBoderBlockUpdate(size_t chunkX, size_t chunkY, size_t x, size_t y, size_t layer);

protected:
	void onEnable() override;
	void onDisable() override;

private:
	void onCenterViewUpdate(float x, float y);
	void addChunk(int x, int y);
	void removeChunk(size_t index); 
	std::vector<Nz::Vector2i> getViewChunks(float x, float y) const;

	EventHolder<CenterViewUpdate> m_CenterViewUpdateHolder;
	TileDefinitionRef m_definition;
	WorldMap & m_map;
	float m_viewSize;

	std::vector<ChunkInfo> m_chunks;
};