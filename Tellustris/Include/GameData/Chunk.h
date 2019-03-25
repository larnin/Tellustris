#pragma once

#include "Tilemap/Tilemap.h"
#include "Utility/Event/Event.h"

#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/ObjectRef.hpp>

#include <vector>

class Chunk;

using ChunkRef = Nz::ObjectRef<Chunk>;
using ChunkConstRef = Nz::ObjectRef<const Chunk>;

class Chunk : public Nz::RefCounted
{
	struct TilemapLayer
	{
		TilemapRef tilemap;
		float height = 0.f;
		unsigned int tileCount = 0;
	};

public:
	struct LayerChanged
	{
		enum ChangeState
		{
			added,
			removed,
			heightChanged,
		};

		size_t layer;
		ChangeState state;
	};

	static const size_t chunkSize = 32;
	static const unsigned int tileSize = 16;
	static const unsigned int tileDelta = 1;

	Chunk();
	Chunk(const Chunk &) = delete;
	Chunk & operator=(const Chunk &) = delete;

	Tile getTile(size_t x, size_t y, size_t layer) const;
	void setTile(size_t x, size_t y, Tile tile, size_t layer);
	TilemapRef getMap(size_t layer);
	bool haveLayer(size_t layer) const;
	void setLayerHeight(size_t layer, float height);
	float layerHeight(size_t layer) const;
	size_t layerCount() const;

	template<typename... Args> static ChunkRef New(Args&&... args)
	{
		auto object = std::make_unique<Chunk>(std::forward<Args>(args)...);
		object->SetPersistent(false);

		return object.release();
	}

	EventHolder<LayerChanged> registerLayerChangedCallback(std::function<void(const LayerChanged &)> callback) { return m_event.connect(callback); }

private:
	static bool tilesEqual(const Tile & t1, const Tile & t2);

	std::vector<TilemapLayer> m_tilemaps;

	Event<LayerChanged> m_event;
};