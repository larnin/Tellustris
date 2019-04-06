
#include "Utility/Event/Events.h"
#include <iostream>

Id nextId()
{
	static Id i = 0;
	return i++;
}

EventsImpl::EventsImpl(const std::function<void(const void*)> & _function)
	: function(_function)
	, blocked(false)
{

}

EventsHolder::EventsHolder(std::shared_ptr<EventsImpl> e)
	: m_eventImpl(std::move(e))
{

}

EventsHolder::EventsHolder(EventsHolder && e) noexcept
	: m_eventImpl(std::move(e.m_eventImpl))
{
	e.m_eventImpl = {};
}

EventsHolder & EventsHolder::operator=(EventsHolder && e) noexcept
{
	disconnect();
	m_eventImpl = e.m_eventImpl;
	e.m_eventImpl = {};
	return *this;
}

EventsHolder::~EventsHolder()
{
	disconnect();
}

EventsHolder::operator bool() const
{
	return !isDisconnected();
}

bool EventsHolder::isDisconnected() const
{
	return !m_eventImpl;
}

bool EventsHolder::isBlocked() const
{
	if (!m_eventImpl)
		return false;
	return m_eventImpl->blocked;
}

void EventsHolder::blockEvent(bool block)
{
	if (!m_eventImpl)
		return;
	m_eventImpl->blocked = block;
}

void EventsHolder::disconnect()
{
	if (!m_eventImpl)
		return;
	m_eventImpl.reset();
}
