#pragma once

#include "Event.h"

#include <algorithm>
#include <iostream>
#include <typeindex>
		

template<typename T>
EventImpl<T>::EventImpl(const std::function<void(const T &)> & _function)
	: function(_function)
	, blocked(false)
	, disconnected(false)
{
}

template<typename T>
inline EventHolder<T> Event<T>::connect(const std::function<void(const T&)>& function)
{
	m_events.push_back(std::make_unique<EventImpl<T>>(function));
	return EventHolder<T>(m_events.back().get());
}

template<typename T>
inline void Event<T>::send(const T & value)
{
	for (auto & e : m_events)
	{
		if (e->disconnected || e->blocked || !e->function)
			continue;
		e->function(value);
	}

	m_events.erase(std::remove_if(m_events.begin(), m_events.end(), [](const auto & v) {return v->disconnected; }), m_events.end());
}

template <typename T>
EventHolder<T> StaticEvent<T>::connect(const std::function<void(const T &)> & function)
{
	return instance().connectInstance(function);
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
EventHolder<T>::EventHolder(EventHolder<T> && e)
	: m_eventImpl(e.m_eventImpl)
{
	e.m_eventImpl = nullptr;
}

template<typename T>
EventHolder<T> & EventHolder<T>::operator=(EventHolder<T> && e)
{
	if(m_eventImpl != nullptr)
		m_eventImpl->disconnected = true;
	m_eventImpl = e.m_eventImpl;
	e.m_eventImpl = nullptr;
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
	if (m_eventImpl == nullptr)
		return true;
	return m_eventImpl->disconnected;
}

template<typename T>
bool EventHolder<T>::isBlocked() const
{
	if (m_eventImpl == nullptr)
		return false;
	return m_eventImpl->blocked;
}

template<typename T>
void EventHolder<T>::blockEvent(bool block)
{
	if (m_eventImpl == nullptr)
		return;
	m_eventImpl->blocked = block;
}

template<typename T>
void EventHolder<T>::disconnect()
{
	if (m_eventImpl == nullptr)
		return;
	m_eventImpl->disconnected = true;
	m_eventImpl = nullptr;
}

template<typename T>
EventHolder<T>::~EventHolder()
{
	disconnect();
}

template<typename T>
EventHolder<T>::EventHolder(EventImpl<T> * e)
	: m_eventImpl(e)
{
}
