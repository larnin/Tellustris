#pragma once

#include "Tilemap/Tilemap.h"

#include <NDK/Component.hpp>
#include <Nazara/Graphics/TileMap.hpp>

#include <vector>

class TilemapComponent : public Ndk::Component<TilemapComponent>
{
public:
	TilemapComponent() = default;
	TilemapComponent(const TilemapComponent & map);
	TilemapComponent & operator=(const TilemapComponent & map);

	void attachTilemap(TilemapRef tilemap);
	void attachRenderer(Nz::TileMapRef renderer);
	bool detachRenderer(Nz::TileMapRef renderer);
	void detachAllRenderers();

	static Ndk::ComponentIndex componentIndex;

private:
	void updateRenderer(Nz::TileMapRef renderer);
	void onTilemapUpdate(size_t x, size_t y);

	TilemapRef m_tilemap;
	std::vector<Nz::TileMapRef> m_renderers;
	EventHolder<Tilemap::TilemapModified> m_modifiedEvent;
};