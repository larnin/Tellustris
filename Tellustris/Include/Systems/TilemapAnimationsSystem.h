#pragma once

#include <NDK/System.hpp>

class TilemapAnimationsSystem : public Ndk::System<TilemapAnimationsSystem>
{
public:
	TilemapAnimationsSystem();

	static Ndk::SystemIndex systemIndex;

protected:
	virtual void OnUpdate(float elapsedTime) override;
};