#pragma once

#include "Tilemap/Tilemap.h"

#include <NDK/Component.hpp>
#include <Nazara/Graphics/TileMap.hpp>

#include <vector>

class TilemapComponent : public Ndk::Component<TilemapComponent>
{
public:
	TilemapComponent() = default;
	TilemapComponent(const TilemapComponent &) = delete;
	TilemapComponent & operator=(const TilemapComponent &) = delete;
	TilemapComponent(TilemapComponent &&) = default;
	TilemapComponent & operator=(TilemapComponent &&) = default;

	void attachTilemap(TilemapRef tilemap);
	void attachRenderer(Nz::TileMapRef renderer);
	bool detachRenderer(Nz::TileMapRef renderer);
	void detachAllRenderers();

	static Ndk::ComponentIndex componentIndex;

private:
	void updateRenderer(Nz::TileMapRef renderer);
	void onTilemapUpdate();

	TilemapRef m_tilemap;
	std::vector<Nz::TileMapRef> m_renderers;
	EventHolder<Tilemap::TilemapModified> m_modifiedEvent;
};