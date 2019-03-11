#pragma once

#include "Behaviour.h"
#include "Utility/Event/Event.h"
#include "Utility/Event/Args.h"
#include "GameData/TileDefinition.h"
#include "GameData/WorldMap.h"

#include <NDK/Entity.hpp>

#include <vector>

class WorldRenderBehaviour2 : public Behaviour
{
	struct ChunkInfo
	{
		Ndk::EntityHandle entity;
		int x;
		int y;
	};

public:
	WorldRenderBehaviour2(WorldMap & map, TileDefinitionRef definition, float viewSize);
	BehaviourRef clone() const override;

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