#pragma once

#include "Event.h"

#include <algorithm>
#include <iostream>
#include <typeindex>
		

template<typename T>
EventImpl<T>::EventImpl(const std::function<void(const T &)> & _function)
	: function(_function)
	, blocked(false)
{
}

template<typename T>
inline EventHolder<T> Event<T>::connect(const std::function<void(const T&)>& function)
{
	auto func = std::make_shared<EventImpl<T>>(function);
	m_events.push_back(func);
	return EventHolder<T>(func);
}

template<typename T>
inline void Event<T>::send(const T & value)
{
	for (auto & e : m_events)
	{
		auto eLock = e.lock();
		if (!eLock || eLock->blocked || !eLock->function)
			continue;
		eLock->function(value);
	}

	m_events.erase(std::remove_if(m_events.begin(), m_events.end(), [](const auto & v) {return v.expired(); }), m_events.end());
}

template <typename T>
EventHolder<T> StaticEvent<T>::connect(const std::function<void(const T &)> & function)
{
	return instance().connect(function);
}

template<typename T>
void StaticEvent<T>::send(const T & value)
{
	instance().send(value);
}

template<typename T>
inline Event<T> & StaticEvent<T>::instance()
{
	static Event<T> ins;
	return ins;
}

template<typename T>
EventHolder<T>::EventHolder()
	: m_eventImpl(nullptr)
{
}

template<typename T>
EventHolder<T>::EventHolder(EventHolder<T> && e) noexcept
	: m_eventImpl(std::move(e.m_eventImpl))
{
	e.m_eventImpl = {};
}

template<typename T>
EventHolder<T> & EventHolder<T>::operator=(EventHolder<T> && e) noexcept
{
	disconnect();
	m_eventImpl = e.m_eventImpl;
	e.m_eventImpl = {};
	return *this;
}

template<typename T>
EventHolder<T>::operator bool() const
{
	return !isDisconnected();
}

template<typename T>
bool EventHolder<T>::isDisconnected() const
{
	return !m_eventImpl;
}

template<typename T>
bool EventHolder<T>::isBlocked() const
{
	if (!m_eventImpl)
		return false;
	return m_eventImpl->blocked;
}

template<typename T>
void EventHolder<T>::blockEvent(bool block)
{
	if (!m_eventImpl)
		return;
	m_eventImpl->blocked = block;
}

template<typename T>
void EventHolder<T>::disconnect()
{
	if (!m_eventImpl)
		return;
	m_eventImpl.reset();
}

template<typename T>
EventHolder<T>::~EventHolder()
{
	disconnect();
}

template<typename T>
EventHolder<T>::EventHolder(std::shared_ptr<EventImpl<T>> e)
	: m_eventImpl(e)
{
}
