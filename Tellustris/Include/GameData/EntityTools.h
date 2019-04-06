#pragma once

#include <NDK/Entity.hpp>

namespace EntityEvent
{
	template <typename T>
	void send(Ndk::EntityHandle entity, const T & value);
}

#include "EntityTools.inl"