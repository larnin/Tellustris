#pragma once

#include "Tilemap/Tilemap.h"
#include "Tilemap/TilemapCollider.h"

#include <NDK/Component.hpp>

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
	void attachColliders(TilemapColliderRef collider);

	static Ndk::ComponentIndex componentIndex;

private:
	void onTilemapModified(size_t x, size_t y);
	void onColliderModified();

	void updateLayers();
	void updateLayer(unsigned int index);

	EventHolder<Tilemap::TilemapModified> m_mapModifiedEvent;
	EventHolder<TilemapCollider::TilemapCollidierModified> m_colliderModifiedEvent;

	TilemapRef m_tilemap;
	TilemapColliderRef m_colliders;

	std::vector<ColliderLayer> m_layers;
};