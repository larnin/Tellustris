#pragma once

#include "Utility/Event/Event.h"

#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/ObjectRef.hpp>

#include <unordered_map>

class TilemapCollider;

using TilemapColliderRef = Nz::ObjectRef<TilemapCollider>;
using TilemapColliderConstRef = Nz::ObjectRef<const TilemapCollider>;

class TilemapCollider : public Nz::RefCounted
{

public:
	struct TilemapCollidierModified {};

	void addTile(unsigned int tileID, unsigned int layer);
	void removeTile(unsigned int tileID);
	bool haveTile(unsigned int tileID) const;
	unsigned int layer(unsigned int tileID) const;
	std::vector<unsigned int> tiles(unsigned int layer) const;

	template<typename... Args> static TilemapColliderRef New(Args&&... args)
	{
		auto object = std::make_unique<TilemapCollider>(std::forward<Args>(args)...);
		object->SetPersistent(false);

		return object.release();
	}

	EventHolder<TilemapCollidierModified> registerAnimationsModifiedCallback(std::function<void(const TilemapCollidierModified &)> callback)
	{
		return m_event.connect(callback);
	}

private:
	Event<TilemapCollidierModified> m_event;

	std::unordered_map<unsigned int, unsigned int> m_tiles;
};