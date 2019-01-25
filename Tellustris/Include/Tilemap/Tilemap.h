#pragma once

#include "Utility/Matrix.h"
#include "Utility/Event/Event.h"

#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/ObjectRef.hpp>

#include <functional>

class Tilemap;

using TilemapRef = Nz::ObjectRef<Tilemap>;
using TilemapConstRef = Nz::ObjectRef<const Tilemap>;

class Tilemap : public Nz::RefCounted
{
public:
	using TileType = unsigned int;
	using iterator = std::vector<TileType>::iterator;
	using const_iterator = std::vector<TileType>::const_iterator;

	struct TilemapModified
	{
		size_t x;
		size_t y;
	};

	Tilemap(size_t width, size_t height, unsigned int tileSize = 1, unsigned int tileDelta = 0);
	Tilemap(const Tilemap &) = delete;
	Tilemap & operator=(const Tilemap &) = delete;

	TileType getTile(size_t x, size_t y) const;
	void setTile(size_t x, size_t y, TileType value);

	const_iterator begin() const { return m_matrix.begin(); }
	const_iterator end() const { return m_matrix.end(); }

	EventHolder<TilemapModified> registerTilemapModifiedCallback(std::function<void(const TilemapModified &)> callback)
	{
		return m_event.connect(callback);
	}

	template<typename... Args> static TilemapRef New(Args&&... args)
	{
		auto object = std::make_unique<Tilemap>(std::forward<Args>(args)...);
		object->SetPersistent(false);

		return object.release();
	}

	size_t width() const { return m_matrix.width(); }
	size_t height() const { return m_matrix.height(); }

	unsigned int tileSize() const { return m_tileSize; }
	unsigned int tileDelta() const { return m_tileDelta; }

	void setTileSize(unsigned int size);
	void setTileDelta(unsigned int delta);

private:
	Matrix<TileType> m_matrix;
	Event<TilemapModified> m_event;
	unsigned int m_tileSize = 1;
	unsigned int m_tileDelta = 0;
};