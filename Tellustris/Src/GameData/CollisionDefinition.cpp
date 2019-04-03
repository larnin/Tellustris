#include "GameData/CollisionDefinition.h"
#include "Components/BehaviourComponent.h"

#include <cassert>
#include <algorithm>

size_t CollisionDefinition::addLayer(const std::string & name)
{
	assert(m_layerNb < maxLayerNb);
	assert(!haveLayer(name));

	resetLayer(m_layerNb);
	m_layerInfos[m_layerNb].name = name;
	m_layerNb++;

	return m_layerNb - 1;
}

void CollisionDefinition::removeLayer(const std::string & name)
{
	removeLayer(layerIndex(name));
}

void CollisionDefinition::removeLayer(size_t layerIndex)
{
	assert(layerIndex < m_layerNb);

	for (size_t i = layerIndex; i < m_layerNb - 1; i++)
		m_layerInfos[i] = std::move(m_layerInfos[i + 1]);

	m_layerNb--;
	resetLayer(m_layerNb);
}

bool CollisionDefinition::haveLayer(const std::string & name) const
{
	return std::find_if(m_layerInfos.begin(), m_layerInfos.end(), [name](const auto & l) {return l.name == name; }) != m_layerInfos.end();
}

bool CollisionDefinition::haveLayer(size_t layerIndex) const
{
	return layerIndex < m_layerNb;
}

size_t CollisionDefinition::layerIndex(const std::string & name) const
{
	assert(haveLayer(name));

	return std::find_if(m_layerInfos.begin(), m_layerInfos.end(), [name](const auto & l) {return l.name == name; }) - m_layerInfos.begin();
}

size_t CollisionDefinition::layerNb() const
{
	return m_layerNb;
}

unsigned int CollisionDefinition::collisionAndTriggerMask(const std::string & name) const
{
	return collisionAndTriggerMask(layerIndex(name));
}

unsigned int CollisionDefinition::collisionAndTriggerMask(size_t layerIndex) const
{
	assert(layerIndex < m_layerNb);

	unsigned int mask = 0;

	for (size_t i = 0; i < maxLayerNb; i++)
		if (m_layerInfos[layerIndex].contacts[i] != ContactType::None)
			mask |= 1 << i;

	return mask;
}

unsigned int CollisionDefinition::collisionMask(const std::string & name) const
{
	return collisionMask(layerIndex(name));
}

unsigned int CollisionDefinition::collisionMask(size_t layerIndex) const
{
	assert(layerIndex < m_layerNb);

	unsigned int mask = 0;

	for (size_t i = 0; i < maxLayerNb; i++)
		if (m_layerInfos[layerIndex].contacts[i] == ContactType::Collision)
			mask |= 1 << i;

	return mask;
}

unsigned int CollisionDefinition::triggerMask(const std::string & name) const
{
	return triggerMask(layerIndex(name));
}

unsigned int CollisionDefinition::triggerMask(size_t layerIndex) const
{
	assert(layerIndex < m_layerNb);

	unsigned int mask = 0;

	for (size_t i = 0; i < maxLayerNb; i++)
		if (m_layerInfos[layerIndex].contacts[i] == ContactType::Trigger)
			mask |= 1 << i;

	return mask;
}

void CollisionDefinition::setCollision(const std::string & name1, const std::string & name2, ContactType type)
{
	setCollision(layerIndex(name1), layerIndex(name2), type);
}

void CollisionDefinition::setCollision(size_t layer1, size_t layer2, ContactType type)
{
	assert(layer1 < m_layerNb);
	assert(layer2 < m_layerNb);

	m_layerInfos[layer1].contacts[layer2] = type;
	m_layerInfos[layer2].contacts[layer1] = type;
}

ContactType CollisionDefinition::getCollision(const std::string & name1, std::string & name2) const
{
	return getCollision(layerIndex(name1), layerIndex(name2));
}

ContactType CollisionDefinition::getCollision(size_t layer1, size_t layer2) const
{
	assert(layer1 < m_layerNb);
	assert(layer2 < m_layerNb);

	return m_layerInfos[layer1].contacts[layer2];
}

void CollisionDefinition::resetLayer(size_t index)
{
	assert(index < maxLayerNb);
	m_layerInfos[index].name = "";

	for (auto & v : m_layerInfos[index].contacts)
		v = ContactType::None;
}

void createCollisionsCallback(CollisionDefinitionRef def, Ndk::PhysicsSystem2D & physics)
{
	for(size_t i = 0 ; i < def->layerNb() ; i++)
		for (size_t j = 0; j < def->layerNb(); j++)
		{
			auto type = def->getCollision(i, j);
			if (type == ContactType::None)
				continue;

			Ndk::PhysicsSystem2D::Callback callbacks;
			callbacks.startCallback = [type](Ndk::PhysicsSystem2D & physics, Nz::Arbiter2D & arbiter, const Ndk::EntityHandle & entity1, const Ndk::EntityHandle & entity2, void*)
			{ 
				return contactStart(physics, arbiter, entity1, entity2, type); 
			};
			callbacks.endCallback = [type](Ndk::PhysicsSystem2D & physics, Nz::Arbiter2D & arbiter, const Ndk::EntityHandle & entity1, const Ndk::EntityHandle & entity2, void*)
			{ 
				contactEnd(physics, arbiter, entity1, entity2, type); 
			};
			callbacks.preSolveCallback = [type](Ndk::PhysicsSystem2D & physics, Nz::Arbiter2D & arbiter, const Ndk::EntityHandle & entity1, const Ndk::EntityHandle & entity2, void*)
			{ 
				return contactPreSolve(physics, arbiter, entity1, entity2, type); 
			};
			callbacks.postSolveCallback = [type](Ndk::PhysicsSystem2D & physics, Nz::Arbiter2D & arbiter, const Ndk::EntityHandle & entity1, const Ndk::EntityHandle & entity2, void*)
			{ 
				contactPostSolve(physics, arbiter, entity1, entity2, type); 
			};

			physics.RegisterCallbacks(static_cast<unsigned int>(i), static_cast<unsigned int>(j), callbacks);
		}
}

bool contactStart(Ndk::PhysicsSystem2D & physics, Nz::Arbiter2D & arbiter, const Ndk::EntityHandle & entity1, const Ndk::EntityHandle & entity2, ContactType type)
{
	assert(entity1.IsValid());
	assert(entity2.IsValid());
	if (entity1->HasComponent<BehaviourComponent>())
	{
		auto & behaviour = entity1->GetComponent<BehaviourComponent>();
		ContactArbiter2D contactArbiter(arbiter, true, type);
		behaviour.onContactStart(contactArbiter, entity2);
	}
	if (entity2->HasComponent<BehaviourComponent>())
	{
		auto & behaviour = entity2->GetComponent<BehaviourComponent>();
		ContactArbiter2D contactArbiter(arbiter, false, type);
		behaviour.onContactStart(contactArbiter, entity1);
	}
	return type == ContactType::Collision;
}

void contactEnd(Ndk::PhysicsSystem2D & physics, Nz::Arbiter2D & arbiter, const Ndk::EntityHandle & entity1, const Ndk::EntityHandle & entity2, ContactType type)
{
	assert(entity1.IsValid());
	assert(entity2.IsValid());
	if (entity1->HasComponent<BehaviourComponent>())
	{
		auto & behaviour = entity1->GetComponent<BehaviourComponent>();
		ContactArbiter2D contactArbiter(arbiter, true, type);
		behaviour.onContactEnd(contactArbiter, entity2);
	}
	if (entity2->HasComponent<BehaviourComponent>())
	{
		auto & behaviour = entity2->GetComponent<BehaviourComponent>();
		ContactArbiter2D contactArbiter(arbiter, false, type);
		behaviour.onContactEnd(contactArbiter, entity1);
	}
}

bool contactPreSolve(Ndk::PhysicsSystem2D & physics, Nz::Arbiter2D & arbiter, const Ndk::EntityHandle & entity1, const Ndk::EntityHandle & entity2, ContactType type)
{
	assert(entity1.IsValid());
	assert(entity2.IsValid());
	if (entity1->HasComponent<BehaviourComponent>())
	{
		auto & behaviour = entity1->GetComponent<BehaviourComponent>();
		ContactArbiter2D contactArbiter(arbiter, true, type);
		behaviour.onContactPreSolve(contactArbiter, entity2);
	}
	if (entity2->HasComponent<BehaviourComponent>())
	{
		auto & behaviour = entity2->GetComponent<BehaviourComponent>();
		ContactArbiter2D contactArbiter(arbiter, false, type);
		behaviour.onContactPreSolve(contactArbiter, entity1);
	}
	return type == ContactType::Collision;
}

void contactPostSolve(Ndk::PhysicsSystem2D & physics, Nz::Arbiter2D & arbiter, const Ndk::EntityHandle & entity1, const Ndk::EntityHandle & entity2, ContactType type)
{
	assert(entity1.IsValid());
	assert(entity2.IsValid());
	if (entity1->HasComponent<BehaviourComponent>())
	{
		auto & behaviour = entity1->GetComponent<BehaviourComponent>();
		ContactArbiter2D contactArbiter(arbiter, true, type);
		behaviour.onContactPostSolve(contactArbiter, entity2);
	}
	if (entity2->HasComponent<BehaviourComponent>())
	{
		auto & behaviour = entity2->GetComponent<BehaviourComponent>();
		ContactArbiter2D contactArbiter(arbiter, false, type);
		behaviour.onContactPostSolve(contactArbiter, entity1);
	}
}