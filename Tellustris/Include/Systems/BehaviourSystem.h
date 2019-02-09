#pragma once

#include <NDK/System.hpp>

class BehaviourSystem : public Ndk::System<BehaviourSystem>
{
public:
	BehaviourSystem();

	static Ndk::SystemIndex systemIndex;

protected:
	virtual void OnUpdate(float elapsedTime) override;
};