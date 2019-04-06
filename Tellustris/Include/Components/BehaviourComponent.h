#pragma once

#include "GameData/Behaviours/Behaviour.h"
#include "GameData/ContactArbiter2D.h"
#include "Utility/Event/Events.h"

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

	void onContactStart(ContactArbiter2D & arbiter, const Ndk::EntityHandle & otherBody);
	void onContactEnd(ContactArbiter2D & arbiter, const Ndk::EntityHandle & otherBody);
	void onContactPreSolve(ContactArbiter2D & arbiter, const Ndk::EntityHandle & otherBody);
	void onContactPostSolve(ContactArbiter2D & arbiter, const Ndk::EntityHandle & otherBody);

	template <typename T>
	EventsHolder connect(std::function<void(const T &)> func)
	{
		return m_events.connect(func);
	}

	template <typename T>
	void send(const T & value)
	{
		m_events.send(value);
	}

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

	Events m_events;
};