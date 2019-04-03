#pragma once

#include <Nazara/Core/ObjectRef.hpp>

#include <string>
#include <unordered_map>
#include <cassert>

template <typename T>
class Ressource
{
public:
	static void add(const std::string & name, Nz::ObjectRef<T> ressource)
	{
		assert(ressource);

		auto it = m_ressources.find(name);
		if (it == m_ressources.end())
			m_ressources.emplace(name, ressource);
		else it->second = ressource;
	}

	static void remove(const std::string & name)
	{
		auto it = m_ressources.find(name);
		if (it != m_ressources.end())
			m_ressources.erase(it);
	}

	static void clear()
	{
		m_ressources.clear();
	}

	static bool have(const std::string & name)
	{
		return m_ressources.find(name) != m_ressources.end();
	}

	static Nz::ObjectRef<T> get(const std::string & name)
	{
		auto it = m_ressources.find(name);
		if (it != m_ressources.end())
			return it->second;
		return {};
	}

	static std::vector<std::string> keys()
	{
		std::vector<std::string> keys;
		for (const auto & r : m_ressources)
			keys.push_back(r.first);
		return keys;
	}

private:
	Ressource() = delete;

	static std::unordered_map<std::string, Nz::ObjectRef<T>> m_ressources;
};

template <typename T>
std::unordered_map<std::string, Nz::ObjectRef<T>> Ressource<T>::m_ressources;