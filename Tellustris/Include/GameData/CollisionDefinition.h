#pragma once

#include "GameData/ContactArbiter2D.h"

#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <NDK/Systems/PhysicsSystem2D.hpp>

#include <array>

class CollisionDefinition;

using CollisionDefinitionRef = Nz::ObjectRef<CollisionDefinition>;
using CollisionDefinitionConstRef = Nz::ObjectRef<const CollisionDefinition>;

class CollisionDefinition : public Nz::RefCounted
{
	static const unsigned int maxLayerNb = 32u;

	struct CollisionLayerInfos
	{
		std::string name;
		std::array<ContactType, maxLayerNb> contacts;
	};

public:
	size_t addLayer(const std::string & name);
	void removeLayer(const std::string & name);
	void removeLayer(size_t layerIndex);
	bool haveLayer(const std::string & name) const;
	bool haveLayer(size_t layerIndex) const;
	size_t layerIndex(const std::string & name) const;
	size_t layerNb() const;

	unsigned int collisionAndTriggerMask(const std::string & name) const;
	unsigned int collisionAndTriggerMask(size_t layerIndex) const;
	unsigned int collisionMask(const std::string & name) const;
	unsigned int collisionMask(size_t layerIndex) const;
	unsigned int triggerMask(const std::string & name) const;
	unsigned int triggerMask(size_t layerIndex) const;

	void setCollision(const std::string & namelayer1, const std::string & layername2, ContactType type);
	void setCollision(size_t layer1, size_t layer2, ContactType type);
	ContactType getCollision(const std::string & namelayer1, std::string & name2) const;
	ContactType getCollision(size_t layer1, size_t layer2) const;

	template<typename... Args> static CollisionDefinitionRef New(Args&&... args)
	{
		auto object = std::make_unique<CollisionDefinition>(std::forward<Args>(args)...);
		object->SetPersistent(false);

		return object.release();
	}

private:
	void resetLayer(size_t index);

	size_t m_layerNb = 0;
	std::array<CollisionLayerInfos, maxLayerNb> m_layerInfos;
};

void createCollisionsCallback(CollisionDefinitionRef def, Ndk::PhysicsSystem2D & physics);

bool contactStart(Ndk::PhysicsSystem2D & physics, Nz::Arbiter2D & arbiter, const Ndk::EntityHandle & entity1, const Ndk::EntityHandle & entity2, ContactType type);
void contactEnd(Ndk::PhysicsSystem2D & physics, Nz::Arbiter2D & arbiter, const Ndk::EntityHandle & entity1, const Ndk::EntityHandle & entity2, ContactType type);
bool contactPreSolve(Ndk::PhysicsSystem2D & physics, Nz::Arbiter2D & arbiter, const Ndk::EntityHandle & entity1, const Ndk::EntityHandle & entity2, ContactType type);
void contactPostSolve(Ndk::PhysicsSystem2D & physics, Nz::Arbiter2D & arbiter, const Ndk::EntityHandle & entity1, const Ndk::EntityHandle & entity2, ContactType type);