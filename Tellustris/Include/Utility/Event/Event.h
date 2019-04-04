#pragma once

#include <functional>
#include <vector>
#include <memory>

template <typename T>
class EventHolder;

namespace
{
	template <typename T>
	struct EventImpl
	{
		EventImpl(const std::function<void(const T &)> & _function);

		std::function<void(const T &)> function;
		bool blocked;
	};
}

template <typename T>
class Event
{
	friend class EventHolder<T>;
public:
	Event() = default;

	EventHolder<T> connect(const std::function<void(const T &)> & function);
	void send(const T & value);

private:
	std::vector<std::weak_ptr<EventImpl<T>>> m_events;
};

template <typename T>
class StaticEvent
{
public:
	static EventHolder<T> connect(const std::function<void(const T &)> & function);
	static void send(const T & value);
	static Event<T> & instance();

private:
	StaticEvent() = delete;
};

template <typename T>
class EventHolder
{
	friend Event<T>;
public:
	EventHolder();
	EventHolder(const EventHolder<T> &) = delete;
	EventHolder(EventHolder<T> && e) noexcept;
	EventHolder<T> & operator=(const EventHolder<T> &) = delete;
	EventHolder<T> & operator=(EventHolder<T> && e) noexcept;
	operator bool() const;
	bool isDisconnected() const;
	bool isBlocked() const;

	void blockEvent(bool block);
	void disconnect();

	~EventHolder();

private:
	EventHolder(std::shared_ptr<EventImpl<T>> e);
	std::shared_ptr<EventImpl<T>> m_eventImpl;
};

#include "Event.inl"