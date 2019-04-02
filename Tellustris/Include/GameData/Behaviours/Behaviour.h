#pragma once

#include "GameData/ContactArbiter2D.h"

#include <NDK/Entity.hpp>

#include <memory>

class Behaviour;
using BehaviourRef = std::unique_ptr<Behaviour>;

class Behaviour
{
	friend class BehaviourComponent;
public:
	Behaviour() = default;
	virtual ~Behaviour() = default;

	virtual BehaviourRef clone() const = 0;

	size_t index() const { return m_index; }

protected:
	bool haveEntity() const { return m_entity.IsValid(); }
	Ndk::EntityHandle getEntity() const { return m_entity; }

	virtual void onStart() {}
	virtual void onEnable() {}
	virtual void onDisable() {}
	virtual void onDestroy() {}
	virtual void onUpdate(float deltaTime) {}

	virtual void OnEntityComponentAttached(Ndk::BaseComponent& component) {}
	virtual void OnEntityComponentDetached(Ndk::BaseComponent& component) {}

	virtual void onContactStart(ContactArbiter2D & arbiter, const Ndk::EntityHandle & otherBody) {}
	virtual void onContactEnd(ContactArbiter2D & arbiter, const Ndk::EntityHandle & otherBody) {}
	virtual void onContactPreSolve(ContactArbiter2D & arbiter, const Ndk::EntityHandle & otherBody) {}
	virtual void onContactPostSolve(ContactArbiter2D & arbiter, const Ndk::EntityHandle & otherBody) {}

private:
	Ndk::EntityHandle m_entity;
	size_t m_index;
	bool m_started = false;
};