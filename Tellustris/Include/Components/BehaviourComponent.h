#pragma once

#include "GameData/Behaviours/Behaviour.h"

#include <NDK/Component.hpp>

#include <memory>
#include <vector>

class BehaviourComponent : public Ndk::Component<BehaviourComponent>
{
public:
	BehaviourComponent() = default;
	BehaviourComponent(const BehaviourComponent & comp);
	BehaviourComponent & operator=(const BehaviourComponent & comp);

	void attach(BehaviourRef behaviour);
	void detach(size_t index);
	void detachAll();

	void update(float elapsedTime);

	static Ndk::ComponentIndex componentIndex;
private:
	void OnAttached() override;
	void OnComponentAttached(Ndk::BaseComponent& component) override;
	void OnComponentDetached(Ndk::BaseComponent& component) override;
	void OnDetached() override;
	void OnEntityDestruction() override;
	void OnEntityDisabled() override;
	void OnEntityEnabled() override;

	bool m_haveEntity = false;

	std::vector<BehaviourRef> m_behaviours;
};