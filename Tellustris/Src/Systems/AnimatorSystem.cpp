
#include "Systems/AnimatorSystem.h"
#include "Components/AnimatorComponent.h"

#include <NDK/Components/NodeComponent.hpp>

Ndk::SystemIndex AnimatorSystem::systemIndex;

AnimatorSystem::AnimatorSystem()
{
	Requires<Ndk::NodeComponent, AnimatorComponent>();
}

void AnimatorSystem::OnUpdate(float elapsedTime)
{
	for (const Ndk::EntityHandle& entity : GetEntities())
	{
		entity->GetComponent<AnimatorComponent>().update(elapsedTime);
	}
}