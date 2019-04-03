#pragma once

#include <Nazara/Core/ObjectRef.hpp>

#include <string>

template <typename T>
class Settings
{
public:
	static void set(const std::string & name, Nz::ObjectRef<T> value)
	{
		m_value = value;
		m_name = name;
	}

	static Nz::ObjectRef<const T> value()
	{
		return m_value;
	}

	static void reset()
	{
		set("", {});
	}

private:
	Settings() = delete;

	static Nz::ObjectRef<T> m_value;
	static std::string m_name;
};

template <typename T>
Nz::ObjectRef<T> Settings<T>::m_value;

template <typename T>
std::string Settings<T>::m_name;