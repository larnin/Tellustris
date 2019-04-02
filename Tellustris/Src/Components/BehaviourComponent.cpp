
#include "Components/BehaviourComponent.h"

#include <cassert>

Ndk::ComponentIndex BehaviourComponent::componentIndex;


BehaviourComponent::BehaviourComponent(const BehaviourComponent & comp)
{
	*this = comp;
	m_haveEntity = false;
}

BehaviourComponent & BehaviourComponent::operator=(const BehaviourComponent & comp)
{
	for (const auto & b : m_behaviours)
		b->onDisable();

	m_haveEntity = comp.m_haveEntity;

	assert(m_haveEntity == GetEntity().IsValid());

	m_behaviours.clear();

	for (const auto & b : comp.m_behaviours)
	{
		m_behaviours.push_back(std::move(b->clone()));
		m_behaviours.back()->m_index = m_behaviours.size() - 1;
		if (m_haveEntity)
		{
			m_behaviours.back()->m_entity = GetEntity();
			m_behaviours.back()->onEnable();
		}
	}

	return *this;
}

void BehaviourComponent::update(float elapsedTime)
{
	if (!m_haveEntity)
		return;

	for (auto & b : m_behaviours)
		if (!b->m_started)
		{
			b->onStart();
			b->m_started = true;
		}

	for (auto & b : m_behaviours)
		b->onUpdate(elapsedTime);
}

void BehaviourComponent::attach(BehaviourRef behaviour)
{
	assert(behaviour);
	
	if (m_haveEntity)
	{
		assert(GetEntity().IsValid());
		behaviour->m_entity = GetEntity();
		behaviour->onEnable();
	}

	behaviour->m_index = m_behaviours.size();
	m_behaviours.push_back(std::move(behaviour));
}

void BehaviourComponent::detach(size_t index)
{
	assert(index < m_behaviours.size());

	m_behaviours.erase(m_behaviours.begin() + index);
	for (size_t i = index; i < m_behaviours.size(); i++)
		m_behaviours[i]->m_index = i;
}

void BehaviourComponent::detachAll()
{
	if (m_haveEntity)
		for (auto & b : m_behaviours)
			b->onDisable();
}

void BehaviourComponent::OnAttached() 
{
	assert(!m_haveEntity); 
	assert(GetEntity().IsValid());

	m_haveEntity = true;

	for (auto & b : m_behaviours)
	{
		b->m_entity = GetEntity();
		b->onEnable();
	}
}

void BehaviourComponent::OnComponentAttached(Ndk::BaseComponent& component)
{
	assert(m_haveEntity);
	for (auto & b : m_behaviours)
		b->OnEntityComponentAttached(component);
}

void BehaviourComponent::OnComponentDetached(Ndk::BaseComponent& component)
{
	assert(m_haveEntity);
	for (auto & b : m_behaviours)
		b->OnEntityComponentDetached(component);
}

void BehaviourComponent::OnDetached()
{
	assert(m_haveEntity);
	m_haveEntity = false;

	for (auto & b : m_behaviours)
	{
		b->onDisable();
		b->m_entity = nullptr;
	}
}

void BehaviourComponent::OnEntityDestruction()
{
	if (!m_haveEntity)
		return;

	for (auto & b : m_behaviours)
	{
		b->onDestroy();
		b->m_entity = nullptr;
	}
}

void BehaviourComponent::OnEntityDisabled()
{
	assert(m_haveEntity);

	for (auto & b : m_behaviours)
		b->onDisable();
}

void BehaviourComponent::OnEntityEnabled()
{
	assert(m_haveEntity);

	for (auto & b : m_behaviours)
		b->onEnable();
}

void BehaviourComponent::onContactStart(ContactArbiter2D & arbiter, const Ndk::EntityHandle & otherBody)
{
	for (auto & b : m_behaviours)
		b->onContactStart(arbiter, otherBody);
}

void BehaviourComponent::onContactEnd(ContactArbiter2D & arbiter, const Ndk::EntityHandle & otherBody)
{
	for (auto & b : m_behaviours)
		b->onContactEnd(arbiter, otherBody);
}

void BehaviourComponent::onContactPreSolve(ContactArbiter2D & arbiter, const Ndk::EntityHandle & otherBody)
{
	for (auto & b : m_behaviours)
		b->onContactPreSolve(arbiter, otherBody);
}

void BehaviourComponent::onContactPostSolve(ContactArbiter2D & arbiter, const Ndk::EntityHandle & otherBody)
{
	for (auto & b : m_behaviours)
		b->onContactPostSolve(arbiter, otherBody);
}
