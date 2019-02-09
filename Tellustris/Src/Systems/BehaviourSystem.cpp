
#include "Systems/BehaviourSystem.h"
#include "Components/BehaviourComponent.h"

Ndk::SystemIndex BehaviourSystem::systemIndex;

BehaviourSystem::BehaviourSystem()
{
	Requires<BehaviourComponent>();
}

void BehaviourSystem::OnUpdate(float elapsedTime)
{
	for (const Ndk::EntityHandle& entity : GetEntities())
	{
		entity->GetComponent<BehaviourComponent>().update(elapsedTime);
	}
}