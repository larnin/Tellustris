#pragma once

#include <NDK/System.hpp>

class AnimatorSystem : public Ndk::System<AnimatorSystem>
{
public:
	AnimatorSystem();

	static Ndk::SystemIndex systemIndex;

protected:
	virtual void OnUpdate(float elapsedTime) override;
};