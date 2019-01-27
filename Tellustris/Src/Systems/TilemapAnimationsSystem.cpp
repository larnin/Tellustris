
#include "Systems/TilemapAnimationsSystem.h"
#include "Components/TilemapAnimationComponent.h"

Ndk::SystemIndex TilemapAnimationsSystem::systemIndex;

TilemapAnimationsSystem::TilemapAnimationsSystem()
{
	Requires<TilemapAnimationComponent>();
}

void TilemapAnimationsSystem::OnUpdate(float elapsedTime)
{
	for (const Ndk::EntityHandle& entity : GetEntities())
	{
		entity->GetComponent<TilemapAnimationComponent>().update(elapsedTime);
	}
}