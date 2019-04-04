#pragma once

#include "Events.h"

template <typename T>
EventsHolder Events::connect(std::function<void(const T &)> function)
{
	auto id = getId<T>();
	if (m_functions.size() <= id)
	{
		m_functions.resize(id + 1);
	}
	auto func = std::make_shared<EventsImpl>([function = std::move(function)](const void* p){ function(*static_cast<const T*>(p)); });
	m_functions[id].push_back(func);
	return { func };
}

template <typename T>
void Events::send(const T & value)
{
	auto id = getId<T>();
	if (id >= m_functions.size())
		return;
	for (auto & e : m_functions[id])
	{
		auto eLock = e.lock();
		if (!eLock || eLock->blocked || !eLock->function)
			continue;
		eLock->function(&value);
	}

	m_functions[id].erase(std::remove_if(m_functions[id].begin(), m_functions[id].end(), [](const auto & v) {return v.expired(); }), m_functions[id].end());
}