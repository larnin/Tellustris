#pragma once

#include <vector>
#include <functional>
#include <memory>

using Id = size_t;

inline Id nextId()
{
	static Id i = 0;
	return i++;
}

template<class T>
Id getId()
{
	static auto id = nextId();
	return id;
}

class EventsHolder;

struct EventsImpl
{
	EventsImpl(const std::function<void(const void*)> & _function);

	std::function<void(const void*)> function;
	bool blocked;
};

class Events
{
	friend class EventsHolder;

	using EventList = std::vector<std::weak_ptr<EventsImpl>>;

public:
	template <typename T>
	EventsHolder connect(std::function<void(const T &)> function);

	template <typename T>
	void send(const T & value);

private:
	std::vector<EventList> m_functions;
};

class EventsHolder
{
	friend Events;
public:
	EventsHolder();
	EventsHolder(const EventsHolder &) = delete;
	EventsHolder(EventsHolder && e) noexcept;
	EventsHolder & operator=(const EventsHolder &) = delete;
	EventsHolder & operator=(EventsHolder && e) noexcept;
	operator bool() const;
	bool isDisconnected() const;
	bool isBlocked() const;

	void blockEvent(bool block);
	void disconnect();

	~EventsHolder();

private:
	EventsHolder(std::shared_ptr<EventsImpl> e);

	std::shared_ptr<EventsImpl> m_eventImpl;
};

#include "Events.inl"