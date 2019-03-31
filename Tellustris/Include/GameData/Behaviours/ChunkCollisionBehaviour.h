#pragma once

#include "Behaviour.h"
#include "GameData/Chunk.h"

#include <NDK/Entity.hpp>

class ChunkCollisionBehaviour : public Behaviour
{
	struct ColliderLayer
	{
		unsigned int id;
		Ndk::EntityHandle entity;
	};

public:
	ChunkCollisionBehaviour(Chunk & chunk);

	BehaviourRef clone() const override;

protected:
	void onEnable() override;
	void onDisable() override;

private:
	void updateCollisions();
	void clearCollisions();

	void createCollisionLayer(unsigned int collisionLayer);
	Ndk::EntityHandle createEntity();

	bool haveFullCollision(size_t x, size_t y, unsigned int collisionLayer) const;

	Chunk & m_chunk;

	EventHolder<Chunk::LayerChanged> m_layerChangedHolder;
	std::vector<EventHolder<Tilemap::TilemapModified>> m_mapModified;

	std::vector<ColliderLayer> m_layers;
};