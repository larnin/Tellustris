#pragma once

#include "EntityTools.h"
#include "Components/BehaviourComponent.h"

namespace EntityEvent
{
	template <typename T>
	void send(Ndk::EntityHandle entity, const T & value)
	{
		assert(entity.IsValid());

		if (!entity->HasComponent<BehaviourComponent>())
			return;

		auto & comp = entity->GetComponent<BehaviourComponent>();
		comp.send(value);
	}
}
