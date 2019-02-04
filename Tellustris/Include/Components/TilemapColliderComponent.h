#pragma once

#include "Tilemap/Tilemap.h"

#include <NDK/Component.hpp>

#include <Nazara/Math/Rect.hpp>

class TilemapColliderComponent : public Ndk::Component<TilemapColliderComponent>
{
	struct ColliderLayer
	{
		unsigned int id;
		Ndk::EntityHandle entity;
	};

public:
	TilemapColliderComponent() = default;
	TilemapColliderComponent(const TilemapColliderComponent & map);
	TilemapColliderComponent & operator=(const TilemapColliderComponent & map);

	void attachTilemap(TilemapRef tilemap);

	static Ndk::ComponentIndex componentIndex;
private:
	void onTilemapModified(size_t x, size_t y);

	void updateLayers();
	void updateLayer(unsigned int index);

	EventHolder<Tilemap::TilemapModified> m_mapModifiedEvent;

	TilemapRef m_tilemap;

	std::vector<ColliderLayer> m_layers;
};