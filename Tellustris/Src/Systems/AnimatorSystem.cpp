
#include "Systems/AnimatorSystem.h"
#include "Components/AnimatorComponent.h"

Ndk::SystemIndex AnimatorSystem::systemIndex;

AnimatorSystem::AnimatorSystem()
{
	Requires<AnimatorComponent>();
}

void AnimatorSystem::OnUpdate(float elapsedTime)
{
	for (const Ndk::EntityHandle& entity : GetEntities())
	{
		entity->GetComponent<AnimatorComponent>().update(elapsedTime);
	}
}